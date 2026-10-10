#pragma once
#include <Arduino.h>

struct BatteryConfig {
    gpio_num_t pin;
    uint8_t numSamples;
    float voltageDividerRatio;
    uint16_t minimumVoltage;
    uint16_t maximumVoltage;
};

class Battery {
private:

    uint8_t m_percentage {};
    const BatteryConfig& m_config;

public:
    Battery(const BatteryConfig&);

    //measures the battery percentage
    void measurePercentage();

    //returns percentage determined at the time of last "getPercentage()" call
    uint8_t getPercentage();
};