#ifndef RIDE_MANAGER_H
#define RIDE_MANAGER_H

#include "User.h"
#include "Ride.h"
#include "RideTypes.h"
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
    std::unordered_map<std::string, std::vector<std::string>> carpoolRides; // driver -> ride IDs
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
        // Simple distance calculation with realistic scaling
        double latDiff = pickup.latitude - dropoff.latitude;
        double lngDiff = pickup.longitude - dropoff.longitude;
        return std::sqrt(latDiff * latDiff + lngDiff * lngDiff) * 111.0; // Convert to km (1 degree ≈ 111 km)
    }
    
    bool canDriverAcceptCarpool(std::shared_ptr<Driver> driver) {
        if (driver->getStatus() != DriverStatus::AVAILABLE && driver->getStatus() != DriverStatus::ON_TRIP) {
            return false;
        }
        
        auto it = carpoolRides.find(driver->getUserId());
        if (it == carpoolRides.end()) {
            return true; // No carpool rides
        }
        
        // Check if driver has capacity for more passengers
        int currentPassengers = it->second.size();
        return currentPassengers < driver->getVehicle().capacity;
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
        if (!rider) {
            throw std::invalid_argument("Cannot register null rider");
        }
        riders[rider->getUserId()] = rider;
        notifyObservers("USER_REGISTERED", "Rider " + rider->getName() + " registered successfully");
    }
    
    void registerDriver(std::shared_ptr<Driver> driver) {
        if (!driver) {
            throw std::invalid_argument("Cannot register null driver");
        }
        drivers[driver->getUserId()] = driver;
        notifyObservers("USER_REGISTERED", "Driver " + driver->getName() + " registered successfully");
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
            throw std::runtime_error("Rider not found: " + riderId);
        }
        
        // Validate locations
        if (pickup.latitude == dropoff.latitude && pickup.longitude == dropoff.longitude) {
            throw std::invalid_argument("Pickup and dropoff locations cannot be the same");
        }
        
        std::string rideId = generateRideId();
        auto ride = std::make_shared<Ride>(rideId, rider->second, pickup, dropoff, rideType, vehicleType);
        rides[rideId] = ride;
        
        notifyObservers("RIDE_REQUESTED", "New ride request: " + rideId + " for " + rider->second->getName());
        
        // Find available drivers based on ride type
        std::vector<std::shared_ptr<Driver>> availableDrivers;
        for (const auto& driverPair : drivers) {
            if (rideType == RideType::CARPOOL) {
                if (canDriverAcceptCarpool(driverPair.second)) {
                    availableDrivers.push_back(driverPair.second);
                }
            } else {
                if (driverPair.second->getStatus() == DriverStatus::AVAILABLE) {
                    availableDrivers.push_back(driverPair.second);
                }
            }
        }
        
        if (availableDrivers.empty()) {
            notifyObservers("NO_DRIVER_AVAILABLE", 
                          "No drivers available for ride " + rideId + ". Please try again later.");
            return rideId;
        }
        
        // Try to assign driver with improved fallback mechanism
        bool driverAssigned = false;
        std::shared_ptr<Driver> assignedDriver = nullptr;
        
        // Attempt assignment with up to 3 drivers
        int attempts = 0;
        while (!driverAssigned && !availableDrivers.empty() && attempts < 3) {
            assignedDriver = matchingStrategy->findBestDriver(availableDrivers, pickup, vehicleType);
            
            if (!assignedDriver) {
                break; // No suitable driver found
            }
            
            // Simulate driver acceptance (85% acceptance rate for first attempt, decreasing)
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(0.0, 1.0);
            
            double acceptanceRate = 0.85 - (attempts * 0.1); // 85%, 75%, 65%
            
            if (dis(gen) < acceptanceRate) {
                ride->assignDriver(assignedDriver);
                
                if (rideType == RideType::CARPOOL) {
                    carpoolRides[assignedDriver->getUserId()].push_back(rideId);
                    if (assignedDriver->getStatus() == DriverStatus::AVAILABLE) {
                        assignedDriver->setStatus(DriverStatus::ON_TRIP);
                    }
                } else {
                    assignedDriver->setStatus(DriverStatus::ON_TRIP);
                }
                
                driverAssigned = true;
                notifyObservers("DRIVER_ASSIGNED", 
                              "Driver " + assignedDriver->getName() + " assigned to ride " + rideId);
            } else {
                notifyObservers("DRIVER_REJECTED", 
                              "Driver " + assignedDriver->getName() + " rejected ride " + rideId);
                
                // Remove this driver from available list and try next
                availableDrivers.erase(
                    std::remove(availableDrivers.begin(), availableDrivers.end(), assignedDriver),
                    availableDrivers.end()
                );
                attempts++;
            }
        }
        
        if (!driverAssigned) {
            notifyObservers("NO_DRIVER_ASSIGNED", 
                          "Failed to assign driver for ride " + rideId + " after " + std::to_string(attempts) + " attempts");
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
        
        // Apply carpool discount if applicable
        if (ride->getRideType() == RideType::CARPOOL) {
            fare *= 0.8; // 20% carpool discount
        }
        
        ride->setFare(fare);
        
        if (ride->getDriver()) {
            auto driver = ride->getDriver();
            
            // Handle carpool cleanup
            if (ride->getRideType() == RideType::CARPOOL) {
                auto& driverCarpools = carpoolRides[driver->getUserId()];
                driverCarpools.erase(
                    std::remove(driverCarpools.begin(), driverCarpools.end(), rideId),
                    driverCarpools.end()
                );
                
                // If no more carpool rides, set driver to available
                if (driverCarpools.empty()) {
                    driver->setStatus(DriverStatus::AVAILABLE);
                }
            } else {
                driver->setStatus(DriverStatus::AVAILABLE);
            }
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
    
    // Enhanced status reporting
    std::vector<std::string> getSystemStatus() {
        std::vector<std::string> status;
        
        int availableDrivers = 0;
        int onTripDrivers = 0;
        int offlineDrivers = 0;
        
        for (const auto& driverPair : drivers) {
            switch (driverPair.second->getStatus()) {
                case DriverStatus::AVAILABLE: availableDrivers++; break;
                case DriverStatus::ON_TRIP: onTripDrivers++; break;
                case DriverStatus::OFFLINE: offlineDrivers++; break;
            }
        }
        
        status.push_back("Total Drivers: " + std::to_string(drivers.size()));
        status.push_back("Available: " + std::to_string(availableDrivers));
        status.push_back("On Trip: " + std::to_string(onTripDrivers));
        status.push_back("Offline: " + std::to_string(offlineDrivers));
        status.push_back("Total Rides: " + std::to_string(rides.size()));
        status.push_back("Active Carpool Groups: " + std::to_string(carpoolRides.size()));
        
        return status;
    }
};

// Static member definition
std::unique_ptr<RideManager> RideManager::instance = nullptr;

#endif
