#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

// Observer pattern for notifications
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& event, const std::string& message) = 0;
};

class Subject {
private:
    std::vector<std::shared_ptr<Observer>> observers;
    
public:
    virtual ~Subject() = default;
    
    void addObserver(std::shared_ptr<Observer> observer) {
        observers.push_back(observer);
    }
    
    void removeObserver(std::shared_ptr<Observer> observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }
    
    void notifyObservers(const std::string& event, const std::string& message) {
        for (const auto& observer : observers) {
            observer->update(event, message);
        }
    }
};

class RiderNotificationService : public Observer {
private:
    std::string riderId;
    
public:
    RiderNotificationService(const std::string& id) : riderId(id) {}
    
    void update(const std::string& event, const std::string& message) override {
        std::cout << "[RIDER NOTIFICATION - " << riderId << "] " 
                  << event << ": " << message << std::endl;
    }
};

class DriverNotificationService : public Observer {
private:
    std::string driverId;
    
public:
    DriverNotificationService(const std::string& id) : driverId(id) {}
    
    void update(const std::string& event, const std::string& message) override {
        std::cout << "[DRIVER NOTIFICATION - " << driverId << "] " 
                  << event << ": " << message << std::endl;
    }
};

#endif
