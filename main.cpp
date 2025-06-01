#include "RideManager.h"
#include "Observer.h"
#include "MatchingStrategy.h"
#include "PricingStrategy.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

void printSectionHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "🚗 " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void printSubSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---" << std::endl;
}

void simulateRideFlow() {
    printSectionHeader("RideEasy India Platform - Complete Simulation");
    
    // Get RideManager instance (Singleton Pattern)
    RideManager& rideManager = RideManager::getInstance();
    
    std::cout << "📋 Initializing system with Indian context (Mumbai)..." << std::endl;
    
    // Create riders with Indian names and Mumbai locations
    auto rider1 = std::make_shared<Rider>("R001", "Priya Sharma", "+91-9876543210",
                                         Location(19.0760, 72.8777, "Andheri West, Mumbai"));
    auto rider2 = std::make_shared<Rider>("R002", "Rahul Singh", "+91-9876543211",
                                         Location(19.0596, 72.8295, "Bandra West, Mumbai"));
    auto rider3 = std::make_shared<Rider>("R003", "Ananya Gupta", "+91-9876543215",
                                         Location(19.1176, 72.9060, "Thane West, Mumbai"));
    
    // Create drivers with different vehicles (Indian context)
    auto driver1 = std::make_shared<Driver>("D001", "Suresh Kumar", "+91-9876543212",
                                           Vehicle("V001", "Maruti Swift Dzire", "MH-01-AB-1234", "Sedan", 4),
                                           Location(19.0728, 72.8826, "Near Phoenix Mall, Mumbai"));
    
    auto driver2 = std::make_shared<Driver>("D002", "Lakshmi Iyer", "+91-9876543213",
                                           Vehicle("V002", "Toyota Innova Crysta", "MH-02-CD-5678", "SUV", 7),
                                           Location(19.0544, 72.8322, "Near Linking Road, Mumbai"));
    
    auto driver3 = std::make_shared<Driver>("D003", "Vikram Patel", "+91-9876543214",
                                           Vehicle("V003", "Bajaj Pulsar", "MH-03-EF-9012", "Bike", 1),
                                           Location(19.0760, 72.8320, "Near Juhu Circle, Mumbai"));
    
    auto driver4 = std::make_shared<Driver>("D004", "Mohammed Khan", "+91-9876543216",
                                           Vehicle("V004", "Bajaj Auto", "MH-04-GH-3456", "Auto-Rickshaw", 3),
                                           Location(19.0825, 72.8231, "Santacruz Station, Mumbai"));
    
    // Register users
    rideManager.registerRider(rider1);
    rideManager.registerRider(rider2);
    rideManager.registerRider(rider3);
    rideManager.registerDriver(driver1);
    rideManager.registerDriver(driver2);
    rideManager.registerDriver(driver3);
    rideManager.registerDriver(driver4);
    
    std::cout << "✅ Registered 3 riders and 4 drivers" << std::endl;
    
    // Create notification services (Observer Pattern)
    auto riderNotification1 = std::make_shared<RiderNotificationService>("R001");
    auto riderNotification2 = std::make_shared<RiderNotificationService>("R002");
    auto riderNotification3 = std::make_shared<RiderNotificationService>("R003");
    auto driverNotification1 = std::make_shared<DriverNotificationService>("D001");
    auto driverNotification2 = std::make_shared<DriverNotificationService>("D002");
    auto driverNotification3 = std::make_shared<DriverNotificationService>("D003");
    auto driverNotification4 = std::make_shared<DriverNotificationService>("D004");
    
    // Add observers
    rideManager.addObserver(riderNotification1);
    rideManager.addObserver(riderNotification2);
    rideManager.addObserver(riderNotification3);
    rideManager.addObserver(driverNotification1);
    rideManager.addObserver(driverNotification2);
    rideManager.addObserver(driverNotification3);
    rideManager.addObserver(driverNotification4);
    
    std::cout << "✅ Observer pattern: All notification services registered" << std::endl;
    
    // Scenario 1: Normal Sedan Ride (Nearest Driver Strategy)
    printSubSection("Scenario 1: Normal Sedan Ride - Nearest Driver Strategy");
    std::cout << "📍 Route: Andheri West → Bandra Kurla Complex" << std::endl;
    
    Location pickup1(19.0760, 72.8777, "Andheri West Metro Station");
    Location dropoff1(19.0596, 72.8295, "Bandra Kurla Complex");
    
    std::string rideId1 = rideManager.requestRide("R001", pickup1, dropoff1, 
                                                 RideType::NORMAL, VehicleType::SEDAN);
    
    // Simulate ride progression with realistic delays
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    rideManager.updateRideStatus(rideId1, RideStatus::DRIVER_ENROUTE);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    rideManager.updateRideStatus(rideId1, RideStatus::IN_PROGRESS);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    rideManager.updateRideStatus(rideId1, RideStatus::COMPLETED);
    
    // Scenario 2: Best Rated Driver Strategy
    printSubSection("Scenario 2: Bike Ride - Best Rated Driver Strategy");
    std::cout << "🏆 Switching to best-rated driver matching algorithm" << std::endl;
    
    // Change strategy to best rated driver (Strategy Pattern)
    rideManager.setMatchingStrategy(std::make_unique<BestRatedDriverStrategy>());
    
    // Set some ratings to demonstrate rating-based matching
    driver3->setRating(4.9); // Bike driver has highest rating
    driver1->setRating(4.5);
    driver2->setRating(4.7);
    driver4->setRating(4.6);
    
    std::cout << "📊 Driver ratings updated - Vikram (Bike): 4.9★" << std::endl;
    
    Location pickup2(19.0896, 72.8331, "Juhu Beach");
    Location dropoff2(19.1317, 72.8108, "Versova Metro Station");
    
    std::string rideId2 = rideManager.requestRide("R002", pickup2, dropoff2, 
                                                 RideType::NORMAL, VehicleType::BIKE);
    
    // Complete the bike ride
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    rideManager.updateRideStatus(rideId2, RideStatus::DRIVER_ENROUTE);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    rideManager.updateRideStatus(rideId2, RideStatus::IN_PROGRESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    rideManager.updateRideStatus(rideId2, RideStatus::COMPLETED);
    
    // Scenario 3: Surge Pricing (Decorator Pattern)
    printSubSection("Scenario 3: Peak Hour Surge Pricing - Monsoon Rush");
    std::cout << "🌧️ Applying 2x surge pricing during Mumbai monsoon peak hours" << std::endl;
    
    // Apply surge pricing (Decorator Pattern)
    auto surgePricing = std::make_unique<SurgePricingDecorator>(
        std::make_unique<BasePricingCalculator>(), 2.0);
    rideManager.setPricingCalculator(std::move(surgePricing));
    
    // Reset driver status for next ride
    driver1->setStatus(DriverStatus::AVAILABLE);
    
    Location pickup3(19.0176, 72.8562, "Churchgate Station");
    Location dropoff3(19.0330, 72.8697, "Nariman Point");
    
    std::string rideId3 = rideManager.requestRide("R001", pickup3, dropoff3, 
                                                 RideType::NORMAL, VehicleType::SEDAN);
    
    // Complete surge ride quickly to show pricing
    rideManager.updateRideStatus(rideId3, RideStatus::IN_PROGRESS);
    rideManager.updateRideStatus(rideId3, RideStatus::COMPLETED);
    
    // Scenario 4: Festival Discount (Decorator Pattern)
    printSubSection("Scenario 4: Festival Discount - Diwali Special 25% Off");
    std::cout << "🎉 Applying Diwali festival discount pricing" << std::endl;
    
    // Apply festival discount (Decorator Pattern)
    auto discountPricing = std::make_unique<DiscountDecorator>(
        std::make_unique<BasePricingCalculator>(), 25.0);
    rideManager.setPricingCalculator(std::move(discountPricing));
    
    // Reset driver status
    driver2->setStatus(DriverStatus::AVAILABLE);
    
    Location pickup4(19.0825, 72.8231, "Santacruz East");
    Location dropoff4(18.9220, 72.8347, "Worli Sea Link");
    
    std::string rideId4 = rideManager.requestRide("R002", pickup4, dropoff4, 
                                                 RideType::CARPOOL, VehicleType::SUV);
    
    rideManager.updateRideStatus(rideId4, RideStatus::IN_PROGRESS);
    rideManager.updateRideStatus(rideId4, RideStatus::COMPLETED);
    
    // Scenario 5: Edge Case - No Driver Available
    printSubSection("Scenario 5: Edge Case - High Demand, No Drivers Available");
    std::cout << "⚠️ Simulating peak demand when all drivers are busy" << std::endl;
    
    // Set all drivers to ON_TRIP to simulate high demand
    for (auto driver : {driver1, driver2, driver3, driver4}) {
        driver->setStatus(DriverStatus::ON_TRIP);
    }
    
    std::cout << "🚫 All drivers set to ON_TRIP status" << std::endl;
    
    Location pickup5(19.0748, 72.8856, "Powai Lake");
    Location dropoff5(19.1176, 72.9060, "Thane Station");
    
    try {
        std::string rideId5 = rideManager.requestRide("R003", pickup5, dropoff5, 
                                                     RideType::NORMAL, VehicleType::AUTO_RICKSHAW);
        std::cout << "❌ Unexpected: Ride was created despite no available drivers" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✅ Correctly handled: " << e.what() << std::endl;
    }
    
    // Scenario 6: Vehicle Type Demonstration (Factory Pattern)
    printSubSection("Scenario 6: Vehicle Type Factory Pattern Demonstration");
    std::cout << "🏭 Demonstrating Factory Pattern for vehicle types:" << std::endl;
    
    for (auto vType : {VehicleType::BIKE, VehicleType::SEDAN, VehicleType::SUV, VehicleType::AUTO_RICKSHAW}) {
        std::cout << "  " << VehicleTypeFactory::getVehicleTypeName(vType) 
                  << " - Base: Rs." << VehicleTypeFactory::getBaseFare(vType)
                  << ", Per km: Rs." << VehicleTypeFactory::getPerKmRate(vType) << std::endl;
    }
    
    // Reset pricing to normal for summary
    rideManager.setPricingCalculator(std::make_unique<BasePricingCalculator>());
    
    // Final Summary
    printSectionHeader("System Status Summary");
    
    std::cout << "📊 Available Drivers: " << rideManager.getAvailableDrivers().size() 
              << " out of 4 total" << std::endl;
    
    std::cout << "\n💰 Ride Fare Summary:" << std::endl;
    
    auto rides = {
        std::make_pair(rideId1, "Normal Sedan"),
        std::make_pair(rideId2, "Best Rated Bike"),
        std::make_pair(rideId3, "Surge 2x Sedan"),
        std::make_pair(rideId4, "Diwali 25% Off SUV")
    };
    
    for (const auto& [rideId, description] : rides) {
        auto ride = rideManager.getRide(rideId);
        if (ride) {
            std::cout << "  " << rideId << " (" << description << ") - "
                      << "Fare: Rs." << std::fixed << std::setprecision(2) << ride->getFare()
                      << ", Distance: " << ride->getDistance() << " km" << std::endl;
        }
    }
    
    std::cout << "\n🎯 Design Patterns Successfully Demonstrated:" << std::endl;
    std::cout << "  ✅ Singleton Pattern: RideManager centralized management" << std::endl;
    std::cout << "  ✅ Strategy Pattern: Nearest vs Best-rated driver matching" << std::endl;
    std::cout << "  ✅ Decorator Pattern: Surge pricing and discount decorators" << std::endl;
    std::cout << "  ✅ Observer Pattern: Decoupled notification system" << std::endl;
    std::cout << "  ✅ Factory Pattern: Vehicle type creation and configuration" << std::endl;
    
    std::cout << "\n📋 SOLID Principles Applied:" << std::endl;
    std::cout << "  ✅ SRP: Each class has single responsibility" << std::endl;
    std::cout << "  ✅ OCP: Open for extension (new strategies, decorators)" << std::endl;
    std::cout << "  ✅ LSP: All implementations substitutable" << std::endl;
    std::cout << "  ✅ ISP: Focused interfaces without forced dependencies" << std::endl;
    std::cout << "  ✅ DIP: High-level modules depend on abstractions" << std::endl;
    
    printSectionHeader("Mumbai RideEasy Simulation Complete");
    std::cout << "🎉 All scenarios executed successfully!" << std::endl;
    std::cout << "📱 Real-world ready architecture with Indian market adaptations" << std::endl;
}

int main() {
    try {
        simulateRideFlow();
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Critical Error: " << e.what() << std::endl;
        std::cerr << "🔧 Please check system configuration and try again." << std::endl;
        return 1;
    }
    
    return 0;
}
