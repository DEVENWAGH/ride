#ifndef PRICING_STRATEGY_H
#define PRICING_STRATEGY_H

#include "RideTypes.h"
#include <memory>

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
        double baseFare = VehicleTypeFactory::getBaseFare(vehicleType);
        double perKmRate = VehicleTypeFactory::getPerKmRate(vehicleType);
        return baseFare + (distance * perKmRate);
    }
};

// Decorator base class
class PricingDecorator : public PricingCalculator {
protected:
    std::unique_ptr<PricingCalculator> baseCalculator;

public:
    PricingDecorator(std::unique_ptr<PricingCalculator> calculator)
        : baseCalculator(std::move(calculator)) {}
};

// Surge pricing decorator
class SurgePricingDecorator : public PricingDecorator {
private:
    double surgeMultiplier;

public:
    SurgePricingDecorator(std::unique_ptr<PricingCalculator> calculator, double multiplier)
        : PricingDecorator(std::move(calculator)), surgeMultiplier(multiplier) {}
    
    double calculateFare(double distance, VehicleType vehicleType) override {
        double baseFare = baseCalculator->calculateFare(distance, vehicleType);
        return baseFare * surgeMultiplier;
    }
};

// Discount decorator
class DiscountDecorator : public PricingDecorator {
private:
    double discountPercentage;

public:
    DiscountDecorator(std::unique_ptr<PricingCalculator> calculator, double discount)
        : PricingDecorator(std::move(calculator)), discountPercentage(discount) {}
    
    double calculateFare(double distance, VehicleType vehicleType) override {
        double baseFare = baseCalculator->calculateFare(distance, vehicleType);
        return baseFare * (1.0 - discountPercentage / 100.0);
    }
};

#endif
