#ifndef USER_H
#define USER_H

#include <string>
#include <memory>

struct Location {
    double latitude;
    double longitude;
    std::string address;
    
    Location(double lat = 0.0, double lng = 0.0, const std::string& addr = "") 
        : latitude(lat), longitude(lng), address(addr) {}
};

// Base User class following Single Responsibility Principle
class User {
protected:
    std::string userId;
    std::string name;
    std::string phone;
    
public:
    User(const std::string& id, const std::string& name, const std::string& phone)
        : userId(id), name(name), phone(phone) {}
    
    virtual ~User() = default;
    
    // Getters
    const std::string& getUserId() const { return userId; }
    const std::string& getName() const { return name; }
    const std::string& getPhone() const { return phone; }
};

class Rider : public User {
private:
    Location defaultPickupLocation;
    double rating;
    
public:
    Rider(const std::string& id, const std::string& name, const std::string& phone,
          const Location& defaultLocation = Location())
        : User(id, name, phone), defaultPickupLocation(defaultLocation), rating(5.0) {}
    
    const Location& getDefaultPickupLocation() const { return defaultPickupLocation; }
    double getRating() const { return rating; }
    void setRating(double newRating) { rating = newRating; }
};

enum class DriverStatus {
    AVAILABLE,
    ON_TRIP,
    OFFLINE
};

struct Vehicle {
    std::string vehicleId;
    std::string model;
    std::string licensePlate;
    std::string vehicleType; // "Bike", "Sedan", "SUV", "Auto-Rickshaw"
    int capacity;
    
    Vehicle(const std::string& id, const std::string& model, const std::string& plate,
            const std::string& type, int cap)
        : vehicleId(id), model(model), licensePlate(plate), vehicleType(type), capacity(cap) {}
};

class Driver : public User {
private:
    Vehicle vehicle;
    Location currentLocation;
    DriverStatus status;
    double rating;
    
public:
    Driver(const std::string& id, const std::string& name, const std::string& phone,
           const Vehicle& vehicle, const Location& location)
        : User(id, name, phone), vehicle(vehicle), currentLocation(location), 
          status(DriverStatus::AVAILABLE), rating(5.0) {}
    
    const Vehicle& getVehicle() const { return vehicle; }
    const Location& getCurrentLocation() const { return currentLocation; }
    DriverStatus getStatus() const { return status; }
    double getRating() const { return rating; }
    
    void setLocation(const Location& location) { currentLocation = location; }
    void setStatus(DriverStatus newStatus) { status = newStatus; }
    void setRating(double newRating) { rating = newRating; }
};

#endif
