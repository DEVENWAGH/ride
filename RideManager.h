#ifndef RIDE_MANAGER_H
#define RIDE_MANAGER_H

#include "User.h"
#include "Ride.h"
#include "MatchingStrategy.h"
#include "PricingStrategy.h"
#include "Observer.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <random>
#include <cmath>
#include <stdexcept>

// Singleton pattern for ride management
class RideManager : public Subject {
private:
    static std::unique_ptr<RideManager> instance;
    std::unordered_map<std::string, std::shared_ptr<Driver>> drivers;
    std::unordered_map<std::string, std::shared_ptr<Rider>> riders;
    std::unordered_map<std::string, std::shared_ptr<Ride>> rides;
    std::unique_ptr<MatchingStrategy> matchingStrategy;
    std::unique_ptr<PricingCalculator> pricingCalculator;
    int rideCounter;
    
    RideManager() : rideCounter(0) {
        matchingStrategy = std::make_unique<NearestDriverStrategy>();
        pricingCalculator = std::make_unique<BasePricingCalculator>();
    }
    
    std::string generateRideId() {
        return "RIDE_" + std::to_string(++rideCounter);
    }
    
    double calculateDistance(const Location& pickup, const Location& dropoff) {
        // Simple distance calculation
        double latDiff = pickup.latitude - dropoff.latitude;
        double lngDiff = pickup.longitude - dropoff.longitude;
        return std::sqrt(latDiff * latDiff + lngDiff * lngDiff) * 100; // Convert to km
    }
    
public:
    static RideManager& getInstance() {
        if (!instance) {
            instance = std::unique_ptr<RideManager>(new RideManager());
        }
        return *instance;
    }
    
    // User management
    void registerRider(std::shared_ptr<Rider> rider) {
        riders[rider->getUserId()] = rider;
    }
    
    void registerDriver(std::shared_ptr<Driver> driver) {
        drivers[driver->getUserId()] = driver;
    }
    
    // Strategy setters
    void setMatchingStrategy(std::unique_ptr<MatchingStrategy> strategy) {
        matchingStrategy = std::move(strategy);
    }
    
    void setPricingCalculator(std::unique_ptr<PricingCalculator> calculator) {
        pricingCalculator = std::move(calculator);
    }
    
    // Core ride functionality
    std::string requestRide(const std::string& riderId, const Location& pickup,
                           const Location& dropoff, RideType rideType, VehicleType vehicleType) {
        
        auto rider = riders.find(riderId);
        if (rider == riders.end()) {
            throw std::runtime_error("Rider not found");
        }
        
        std::string rideId = generateRideId();
        auto ride = std::make_shared<Ride>(rideId, rider->second, pickup, dropoff, rideType, vehicleType);
        rides[rideId] = ride;
        
        // Find available driver
        std::vector<std::shared_ptr<Driver>> availableDrivers;
        for (const auto& driverPair : drivers) {
            if (driverPair.second->getStatus() == DriverStatus::AVAILABLE) {
                availableDrivers.push_back(driverPair.second);
            }
        }
        
        auto assignedDriver = matchingStrategy->findBestDriver(availableDrivers, pickup, vehicleType);
        
        if (assignedDriver) {
            ride->assignDriver(assignedDriver);
            assignedDriver->setStatus(DriverStatus::ON_TRIP);
            
            notifyObservers("DRIVER_ASSIGNED", 
                          "Driver " + assignedDriver->getName() + " assigned to ride " + rideId);
        } else {
            notifyObservers("NO_DRIVER_AVAILABLE", 
                          "No driver available for ride " + rideId);
        }
        
        return rideId;
    }
    
    void updateRideStatus(const std::string& rideId, RideStatus newStatus) {
        auto rideIt = rides.find(rideId);
        if (rideIt == rides.end()) {
            throw std::runtime_error("Ride not found");
        }
        
        auto ride = rideIt->second;
        ride->setStatus(newStatus);
        
        std::string statusMessage = "Ride " + rideId + " status updated";
        
        switch (newStatus) {
            case RideStatus::REQUESTED:
                statusMessage = "Ride has been requested";
                break;
            case RideStatus::DRIVER_ASSIGNED:
                statusMessage = "Driver has been assigned to the ride";
                break;
            case RideStatus::DRIVER_ENROUTE:
                statusMessage = "Driver is on the way to pickup location";
                break;
            case RideStatus::IN_PROGRESS:
                statusMessage = "Ride has started";
                ride->setStartTime();
                break;
            case RideStatus::COMPLETED:
                statusMessage = "Ride completed successfully";
                ride->setEndTime();
                completeRide(rideId);
                break;
            case RideStatus::CANCELLED:
                statusMessage = "Ride has been cancelled";
                if (ride->getDriver()) {
                    ride->getDriver()->setStatus(DriverStatus::AVAILABLE);
                }
                break;
        }
        
        notifyObservers("RIDE_STATUS_UPDATE", statusMessage);
    }
    
    void completeRide(const std::string& rideId) {
        auto rideIt = rides.find(rideId);
        if (rideIt == rides.end()) {
            return;
        }
        
        auto ride = rideIt->second;
        double distance = calculateDistance(ride->getPickupLocation(), ride->getDropoffLocation());
        ride->setDistance(distance);
        
        double fare = pricingCalculator->calculateFare(distance, ride->getRequestedVehicleType());
        ride->setFare(fare);
        
        if (ride->getDriver()) {
            ride->getDriver()->setStatus(DriverStatus::AVAILABLE);
        }
        
        notifyObservers("PAYMENT_COMPLETED", 
                      "Payment of Rs." + std::to_string(fare) + " completed for ride " + rideId);
    }
    
    std::shared_ptr<Ride> getRide(const std::string& rideId) {
        auto it = rides.find(rideId);
        return (it != rides.end()) ? it->second : nullptr;
    }
    
    std::vector<std::shared_ptr<Driver>> getAvailableDrivers() {
        std::vector<std::shared_ptr<Driver>> available;
        for (const auto& driverPair : drivers) {
            if (driverPair.second->getStatus() == DriverStatus::AVAILABLE) {
                available.push_back(driverPair.second);
            }
        }
        return available;
    }
};

// Static member definition
std::unique_ptr<RideManager> RideManager::instance = nullptr;

#endif
