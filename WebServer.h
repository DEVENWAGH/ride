#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "RideManager.h"
#include <httplib.h>
#include <json/json.h>
#include <string>
#include <memory>
#include <thread>
#include <atomic>

class WebServer {
private:
    httplib::Server server;
    std::thread serverThread;
    std::atomic<bool> running{false};
    int port;
    RideManager& rideManager;
    
    // JSON response helpers
    Json::Value createSuccessResponse(const Json::Value& data = Json::Value::null) {
        Json::Value response;
        response["success"] = true;
        response["data"] = data;
        return response;
    }
    
    Json::Value createErrorResponse(const std::string& message) {
        Json::Value response;
        response["success"] = false;
        response["error"] = message;
        return response;
    }
    
    // Convert objects to JSON
    Json::Value locationToJson(const Location& loc) {
        Json::Value json;
        json["latitude"] = loc.latitude;
        json["longitude"] = loc.longitude;
        json["address"] = loc.address;
        return json;
    }
    
    Json::Value vehicleToJson(const Vehicle& vehicle) {
        Json::Value json;
        json["vehicleId"] = vehicle.vehicleId;
        json["model"] = vehicle.model;
        json["licensePlate"] = vehicle.licensePlate;
        json["vehicleType"] = vehicle.vehicleType;
        json["capacity"] = vehicle.capacity;
        return json;
    }
    
    Json::Value driverToJson(std::shared_ptr<Driver> driver) {
        Json::Value json;
        json["userId"] = driver->getUserId();
        json["name"] = driver->getName();
        json["phone"] = driver->getPhone();
        json["vehicle"] = vehicleToJson(driver->getVehicle());
        json["currentLocation"] = locationToJson(driver->getCurrentLocation());
        json["status"] = static_cast<int>(driver->getStatus());
        json["rating"] = driver->getRating();
        return json;
    }
    
    Json::Value rideToJson(std::shared_ptr<Ride> ride) {
        Json::Value json;
        json["rideId"] = ride->getRideId();
        json["riderId"] = ride->getRider()->getUserId();
        json["riderName"] = ride->getRider()->getName();
        json["pickupLocation"] = locationToJson(ride->getPickupLocation());
        json["dropoffLocation"] = locationToJson(ride->getDropoffLocation());
        json["status"] = static_cast<int>(ride->getStatus());
        json["fare"] = ride->getFare();
        json["distance"] = ride->getDistance();
        json["vehicleType"] = static_cast<int>(ride->getRequestedVehicleType());
        
        if (ride->getDriver()) {
            json["driver"] = driverToJson(ride->getDriver());
        }
        return json;
    }
    
public:
    WebServer(int port = 8080) : port(port), rideManager(RideManager::getInstance()) {
        setupRoutes();
        setupCORS();
    }
    
    void setupCORS() {
        server.set_pre_routing_handler([](const httplib::Request&, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            return httplib::Server::HandlerResponse::Unhandled;
        });
    }
    
    void setupRoutes() {
        // Serve static files
        server.set_mount_point("/", "./web");
        
        // API Routes
        server.Get("/api/drivers", [this](const httplib::Request&, httplib::Response& res) {
            try {
                Json::Value drivers(Json::arrayValue);
                auto availableDrivers = rideManager.getAvailableDrivers();
                
                for (const auto& driver : availableDrivers) {
                    drivers.append(driverToJson(driver));
                }
                
                Json::StreamWriterBuilder builder;
                std::string jsonString = Json::writeString(builder, createSuccessResponse(drivers));
                res.set_content(jsonString, "application/json");
            } catch (const std::exception& e) {
                Json::StreamWriterBuilder builder;
                std::string jsonString = Json::writeString(builder, createErrorResponse(e.what()));
                res.set_content(jsonString, "application/json");
            }
        });
        
        server.Post("/api/rides", [this](const httplib::Request& req, httplib::Response& res) {
            try {
                Json::Reader reader;
                Json::Value requestData;
                
                if (!reader.parse(req.body, requestData)) {
                    throw std::runtime_error("Invalid JSON");
                }
                
                std::string riderId = requestData["riderId"].asString();
                Location pickup(
                    requestData["pickup"]["latitude"].asDouble(),
                    requestData["pickup"]["longitude"].asDouble(),
                    requestData["pickup"]["address"].asString()
                );
                Location dropoff(
                    requestData["dropoff"]["latitude"].asDouble(),
                    requestData["dropoff"]["longitude"].asDouble(),
                    requestData["dropoff"]["address"].asString()
                );
                RideType rideType = static_cast<RideType>(requestData["rideType"].asInt());
                VehicleType vehicleType = static_cast<VehicleType>(requestData["vehicleType"].asInt());
                
                std::string rideId = rideManager.requestRide(riderId, pickup, dropoff, rideType, vehicleType);
                
                Json::Value responseData;
                responseData["rideId"] = rideId;
                
                Json::StreamWriterBuilder builder;
                std::string jsonString = Json::writeString(builder, createSuccessResponse(responseData));
                res.set_content(jsonString, "application/json");
            } catch (const std::exception& e) {
                Json::StreamWriterBuilder builder;
                std::string jsonString = Json::writeString(builder, createErrorResponse(e.what()));
                res.set_content(jsonString, "application/json");
            }
        });
        
        server.Get("/api/rides/([^/]+)", [this](const httplib::Request& req, httplib::Response& res) {
            try {
                std::string rideId = req.matches[1];
                auto ride = rideManager.getRide(rideId);
                
                if (!ride) {
                    throw std::runtime_error("Ride not found");
                }
                
                Json::StreamWriterBuilder builder;
                std::string jsonString = Json::writeString(builder, createSuccessResponse(rideToJson(ride)));
                res.set_content(jsonString, "application/json");
            } catch (const std::exception& e) {
                Json::StreamWriterBuilder builder;
                std::string jsonString = Json::writeString(builder, createErrorResponse(e.what()));
                res.set_content(jsonString, "application/json");
            }
        });
        
        server.Put("/api/rides/([^/]+)/status", [this](const httplib::Request& req, httplib::Response& res) {
            try {
                std::string rideId = req.matches[1];
                Json::Reader reader;
                Json::Value requestData;
                
                if (!reader.parse(req.body, requestData)) {
                    throw std::runtime_error("Invalid JSON");
                }
                
                RideStatus newStatus = static_cast<RideStatus>(requestData["status"].asInt());
                rideManager.updateRideStatus(rideId, newStatus);
                
                Json::StreamWriterBuilder builder;
                std::string jsonString = Json::writeString(builder, createSuccessResponse());
                res.set_content(jsonString, "application/json");
            } catch (const std::exception& e) {
                Json::StreamWriterBuilder builder;
                std::string jsonString = Json::writeString(builder, createErrorResponse(e.what()));
                res.set_content(jsonString, "application/json");
            }
        });
    }
    
    void start() {
        if (running) return;
        
        running = true;
        serverThread = std::thread([this]() {
            std::cout << "🌐 Web server starting on http://localhost:" << port << std::endl;
            server.listen("localhost", port);
        });
    }
    
    void stop() {
        if (!running) return;
        
        running = false;
        server.stop();
        if (serverThread.joinable()) {
            serverThread.join();
        }
    }
    
    ~WebServer() {
        stop();
    }
};

#endif
