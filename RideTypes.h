#ifndef RIDE_TYPES_H
#define RIDE_TYPES_H

#include <string>

enum class RideType {
    NORMAL,
    CARPOOL
};

enum class VehicleType {
    BIKE,
    SEDAN,
    SUV,
    AUTO_RICKSHAW
};

enum class RideStatus {
    REQUESTED,
    DRIVER_ASSIGNED,
    DRIVER_ENROUTE,
    IN_PROGRESS,
    COMPLETED,
    CANCELLED
};

// Factory pattern for vehicle type creation
class VehicleTypeFactory {
public:
    static std::string getVehicleTypeName(VehicleType type) {
        switch(type) {
            case VehicleType::BIKE: return "Bike";
            case VehicleType::SEDAN: return "Sedan";
            case VehicleType::SUV: return "SUV";
            case VehicleType::AUTO_RICKSHAW: return "Auto-Rickshaw";
            default: return "Unknown";
        }
    }
    
    static double getBaseFare(VehicleType type) {
        switch(type) {
            case VehicleType::BIKE: return 15.0;           // ₹15 base fare
            case VehicleType::SEDAN: return 40.0;          // ₹40 base fare
            case VehicleType::SUV: return 60.0;            // ₹60 base fare
            case VehicleType::AUTO_RICKSHAW: return 25.0;  // ₹25 base fare
            default: return 40.0;
        }
    }
    
    static double getPerKmRate(VehicleType type) {
        switch(type) {
            case VehicleType::BIKE: return 6.0;            // ₹6 per km
            case VehicleType::SEDAN: return 10.0;          // ₹10 per km
            case VehicleType::SUV: return 12.0;            // ₹12 per km
            case VehicleType::AUTO_RICKSHAW: return 8.0;   // ₹8 per km
            default: return 10.0;
        }
    }
};

#endif
