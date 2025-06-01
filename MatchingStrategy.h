#ifndef MATCHING_STRATEGY_H
#define MATCHING_STRATEGY_H

#include "User.h"
#include "RideTypes.h"
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <limits>

// Strategy pattern for driver matching
class MatchingStrategy {
public:
    virtual ~MatchingStrategy() = default;
    virtual std::shared_ptr<Driver> findBestDriver(
        const std::vector<std::shared_ptr<Driver>>& availableDrivers,
        const Location& pickupLocation,
        VehicleType requestedVehicleType) = 0;
};

// Nearest driver strategy
class NearestDriverStrategy : public MatchingStrategy {
private:
    double calculateDistance(const Location& loc1, const Location& loc2) {
        // Simple Euclidean distance calculation
        double latDiff = loc1.latitude - loc2.latitude;
        double lngDiff = loc1.longitude - loc2.longitude;
        return std::sqrt(latDiff * latDiff + lngDiff * lngDiff);
    }

public:
    std::shared_ptr<Driver> findBestDriver(
        const std::vector<std::shared_ptr<Driver>>& availableDrivers,
        const Location& pickupLocation,
        VehicleType requestedVehicleType) override {
        
        std::shared_ptr<Driver> bestDriver = nullptr;
        double minDistance = std::numeric_limits<double>::max();
        
        for (const auto& driver : availableDrivers) {
            // Check if driver's vehicle type matches or is compatible
            std::string driverVehicleType = driver->getVehicle().vehicleType;
            std::string requestedType = VehicleTypeFactory::getVehicleTypeName(requestedVehicleType);
            
            if (driverVehicleType == requestedType) {
                double distance = calculateDistance(driver->getCurrentLocation(), pickupLocation);
                if (distance < minDistance) {
                    minDistance = distance;
                    bestDriver = driver;
                }
            }
        }
        
        return bestDriver;
    }
};

// Best rated driver strategy
class BestRatedDriverStrategy : public MatchingStrategy {
public:
    std::shared_ptr<Driver> findBestDriver(
        const std::vector<std::shared_ptr<Driver>>& availableDrivers,
        const Location& /*pickupLocation*/,
        VehicleType requestedVehicleType) override {
        
        std::shared_ptr<Driver> bestDriver = nullptr;
        double highestRating = 0.0;
        
        for (const auto& driver : availableDrivers) {
            // Check if driver's vehicle type matches
            std::string driverVehicleType = driver->getVehicle().vehicleType;
            std::string requestedType = VehicleTypeFactory::getVehicleTypeName(requestedVehicleType);
            
            if (driverVehicleType == requestedType) {
                if (driver->getRating() > highestRating) {
                    highestRating = driver->getRating();
                    bestDriver = driver;
                }
            }
        }
        
        return bestDriver;
    }
};

#endif
