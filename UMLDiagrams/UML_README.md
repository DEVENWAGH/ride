# UML Diagram for RideEasy Platform

## Overview

This document describes the UML class diagram for the RideEasy ride-sharing platform, highlighting the implementation of various design patterns and SOLID principles.

```

## Diagram Structure

### Color Coding

- **Light Blue** (#E1F5FE): Design Pattern implementations
- **Light Purple** (#F3E5F5): Core business classes
- **Light Orange** (#FFF3E0): Enumerations
- **Light Green** (#E8F5E8): Data structures

### Design Patterns Visualized

#### 1. Singleton Pattern

- **Class**: `RideManager`
- **Visualization**: Single instance with private constructor
- **Purpose**: Centralized ride management

#### 2. Strategy Pattern

- **Classes**: `MatchingStrategy` interface with concrete implementations
- **Visualization**: Interface with multiple implementations
- **Purpose**: Pluggable driver matching algorithms

#### 3. Decorator Pattern

- **Classes**: `PricingCalculator` with decorators
- **Visualization**: Composition chain showing wrapping behavior
- **Purpose**: Dynamic pricing modifications

#### 4. Observer Pattern

- **Classes**: `Observer` interface, `Subject` base class
- **Visualization**: Observer list in Subject, multiple concrete observers
- **Purpose**: Decoupled event notifications

#### 5. Factory Pattern

- **Class**: `VehicleTypeFactory`
- **Visualization**: Static methods for object creation
- **Purpose**: Encapsulated vehicle type logic

### Key Relationships

#### Inheritance (IS-A)

- `Rider` and `Driver` inherit from `User`
- Concrete strategies implement `MatchingStrategy`
- Concrete decorators extend `PricingDecorator`
- Notification services implement `Observer`

#### Composition (HAS-A)

- `Driver` has a `Vehicle` and current `Location`
- `Rider` has a default pickup `Location`
- `Ride` has pickup and dropoff `Location`s
- `RideManager` has strategy and calculator instances

#### Association (USES)

- `Ride` references `Rider` and `Driver`
- `RideManager` manages collections of users and rides
- Decorators wrap `PricingCalculator` instances

## Class Responsibilities

### Core Business Classes

1. **User Hierarchy**

   - `User`: Base class with common user data
   - `Rider`: Adds rating and default location
   - `Driver`: Adds vehicle, location, and status

2. **Ride Management**
   - `Ride`: Represents a single ride transaction
   - `RideManager`: Coordinates all ride operations

### Design Pattern Classes

1. **Strategy Pattern**

   - `MatchingStrategy`: Defines driver selection interface
   - `NearestDriverStrategy`: Finds closest driver
   - `BestRatedDriverStrategy`: Finds highest-rated driver

2. **Decorator Pattern**

   - `PricingCalculator`: Base pricing interface
   - `BasePricingCalculator`: Standard fare calculation
   - `SurgePricingDecorator`: Applies surge multiplier
   - `DiscountDecorator`: Applies percentage discount

3. **Observer Pattern**
   - `Observer`: Event notification interface
   - `Subject`: Manages observer list and notifications
   - `RiderNotificationService`: Rider-specific notifications
   - `DriverNotificationService`: Driver-specific notifications

## SOLID Principles Compliance

### Single Responsibility Principle (SRP)

- Each class has one reason to change
- User classes handle user data only
- Pricing classes handle fare calculation only
- Notification classes handle messaging only

### Open/Closed Principle (OCP)

- System open for extension through new strategies
- New pricing decorators can be added
- New matching algorithms don't require existing code changes

### Liskov Substitution Principle (LSP)

- All strategy implementations are interchangeable
- Decorators can replace base calculator
- Observers can be substituted without breaking notifications

### Interface Segregation Principle (ISP)

- Focused interfaces: `MatchingStrategy`, `Observer`, `PricingCalculator`
- No forced dependencies on unused methods

### Dependency Inversion Principle (DIP)

- `RideManager` depends on abstractions (interfaces)
- Concrete implementations injected via setters
- High-level modules independent of low-level details

## Extension Points

The UML diagram shows clear extension points:

1. **New Matching Strategies**: Implement `MatchingStrategy`
2. **New Pricing Rules**: Create new decorators extending `PricingDecorator`
3. **New Notification Channels**: Implement `Observer` interface
4. **New Vehicle Types**: Extend `VehicleTypeFactory`
5. **New User Types**: Inherit from `User` base class

## Usage Flow

1. **Registration**: Users register through `RideManager`
2. **Ride Request**: Rider requests ride with parameters
3. **Driver Matching**: Strategy pattern finds suitable driver
4. **Pricing**: Decorator pattern calculates fare
5. **Notifications**: Observer pattern notifies all parties
6. **Completion**: Ride progresses through status updates

This UML diagram serves as both documentation and blueprint for the RideEasy platform implementation.
