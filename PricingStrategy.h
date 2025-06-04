#ifndef PRICING_STRATEGY_H
#define PRICING_STRATEGY_H

#include "RideTypes.h"
#include <memory>
#include <stdexcept>
#include <algorithm>

// Base pricing calculator interface
class PricingCalculator {
public:
    virtual ~PricingCalculator() = default;
    virtual double calculateFare(double distance, VehicleType vehicleType) = 0;
};

// Base implementation
class BasePricingCalculator : public PricingCalculator {
public:
    double calculateFare(double distance, VehicleType vehicleType) override {
        if (distance < 0) {
            throw std::invalid_argument("Distance cannot be negative");
        }
        
        double baseFare = VehicleTypeFactory::getBaseFare(vehicleType);
        double perKmRate = VehicleTypeFactory::getPerKmRate(vehicleType);
        
        // Minimum fare logic - at least base fare even for very short distances
        double calculatedFare = baseFare + (distance * perKmRate);
        return std::max(calculatedFare, baseFare);
    }
};

// Decorator base class
class PricingDecorator : public PricingCalculator {
protected:
    std::unique_ptr<PricingCalculator> baseCalculator;

public:
    PricingDecorator(std::unique_ptr<PricingCalculator> calculator)
        : baseCalculator(std::move(calculator)) {
        if (!baseCalculator) {
            throw std::invalid_argument("Base calculator cannot be null");
        }
    }
};

// Surge pricing decorator
class SurgePricingDecorator : public PricingDecorator {
private:
    double surgeMultiplier;

public:
    SurgePricingDecorator(std::unique_ptr<PricingCalculator> calculator, double multiplier)
        : PricingDecorator(std::move(calculator)), surgeMultiplier(multiplier) {
        if (multiplier <= 0) {
            throw std::invalid_argument("Surge multiplier must be positive");
        }
        if (multiplier > 5.0) {
            throw std::invalid_argument("Surge multiplier cannot exceed 5x for regulatory compliance");
        }
    }
    
    double calculateFare(double distance, VehicleType vehicleType) override {
        double baseFare = baseCalculator->calculateFare(distance, vehicleType);
        return baseFare * surgeMultiplier;
    }
    
    double getSurgeMultiplier() const { return surgeMultiplier; }
};

// Discount decorator
class DiscountDecorator : public PricingDecorator {
private:
    double discountPercentage;

public:
    DiscountDecorator(std::unique_ptr<PricingCalculator> calculator, double discount)
        : PricingDecorator(std::move(calculator)), discountPercentage(discount) {
        if (discount < 0 || discount > 100) {
            throw std::invalid_argument("Discount percentage must be between 0 and 100");
        }
    }
    
    double calculateFare(double distance, VehicleType vehicleType) override {
        double baseFare = baseCalculator->calculateFare(distance, vehicleType);
        double discountedFare = baseFare * (1.0 - discountPercentage / 100.0);
        
        // Ensure minimum fare even after discount
        double minimumFare = VehicleTypeFactory::getBaseFare(vehicleType) * 0.5; // 50% of base fare
        return std::max(discountedFare, minimumFare);
    }
    
    double getDiscountPercentage() const { return discountPercentage; }
};

// Additional decorator for toll charges
class TollDecorator : public PricingDecorator {
private:
    double tollAmount;

public:
    TollDecorator(std::unique_ptr<PricingCalculator> calculator, double toll)
        : PricingDecorator(std::move(calculator)), tollAmount(toll) {
        if (toll < 0) {
            throw std::invalid_argument("Toll amount cannot be negative");
        }
    }
    
    double calculateFare(double distance, VehicleType vehicleType) override {
        double baseFare = baseCalculator->calculateFare(distance, vehicleType);
        return baseFare + tollAmount;
    }
    
    double getTollAmount() const { return tollAmount; }
};

#endif
