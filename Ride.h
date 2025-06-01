#ifndef RIDE_H
#define RIDE_H

#include "User.h"
#include "RideTypes.h"
#include <memory>
#include <chrono>

class Ride {
private:
    std::string rideId;
    std::shared_ptr<Rider> rider;
    std::shared_ptr<Driver> driver;
    Location pickupLocation;
    Location dropoffLocation;
    RideType rideType;
    VehicleType requestedVehicleType;
    RideStatus status;
    double fare;
    double distance;
    std::chrono::system_clock::time_point requestTime;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
public:
    Ride(const std::string& id, std::shared_ptr<Rider> rider,
         const Location& pickup, const Location& dropoff,
         RideType type, VehicleType vehicleType)
        : rideId(id), rider(rider), pickupLocation(pickup), dropoffLocation(dropoff),
          rideType(type), requestedVehicleType(vehicleType), status(RideStatus::REQUESTED),
          fare(0.0), distance(0.0), requestTime(std::chrono::system_clock::now()) {}
    
    // Getters
    const std::string& getRideId() const { return rideId; }
    std::shared_ptr<Rider> getRider() const { return rider; }
    std::shared_ptr<Driver> getDriver() const { return driver; }
    const Location& getPickupLocation() const { return pickupLocation; }
    const Location& getDropoffLocation() const { return dropoffLocation; }
    RideType getRideType() const { return rideType; }
    VehicleType getRequestedVehicleType() const { return requestedVehicleType; }
    RideStatus getStatus() const { return status; }
    double getFare() const { return fare; }
    double getDistance() const { return distance; }
    
    // Setters
    void assignDriver(std::shared_ptr<Driver> assignedDriver) { 
        driver = assignedDriver; 
        status = RideStatus::DRIVER_ASSIGNED;
    }
    void setStatus(RideStatus newStatus) { status = newStatus; }
    void setFare(double calculatedFare) { fare = calculatedFare; }
    void setDistance(double rideDistance) { distance = rideDistance; }
    void setStartTime() { startTime = std::chrono::system_clock::now(); }
    void setEndTime() { endTime = std::chrono::system_clock::now(); }
};

#endif
