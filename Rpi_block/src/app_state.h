#pragma once

#include <mutex>
#include <nlohmann/json.hpp>

class AppState {
public:
    void setTemperature(double value);
    nlohmann::json snapshot() const;

private:
    mutable std::mutex mutex_;
    double temperature_c_ = 20.0;
};

void AppState::setTemperature(double value) {
    std::lock_guard<std::mutex> lock(mutex_);
    temperature_c_ = value;
}

nlohmann::json AppState::snapshot() const { // {"temperature_c": 20.0}
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json j;
    j["temperature_c"] = temperature_c_;
    return j;
}
