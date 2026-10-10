#include "Battery.h"

Battery::Battery(const BatteryConfig& config): m_config { config } {};

uint8_t Battery::getPercentage() { return m_percentage; };

void Battery::measurePercentage() {

    uint32_t samples[m_config.numSamples] {};
    uint8_t min { 0 };
    uint8_t max { 0 };

    //measuring samples
    for(uint8_t sample = 0; sample < m_config.numSamples; sample++) {
        uint32_t reading = analogReadMilliVolts(m_config.pin);
        samples[sample] = reading;

        //get the sample with the lowest voltage
        if(reading < samples[min]) {
            min = sample;
        }
        
        //get the sample with the highest voltage
        if(reading > samples[max]) {
            max = sample;
        }

        //stretching the measurement time a bit
        delay(10);
    }

    //remove the extremes (highest and lowest voltage)
    samples[min] = 0;
    samples[max] = 0;
    uint8_t validSamples = m_config.numSamples - 2;

    //sum the remaining voltages
    uint32_t voltageSum { 0 };
    for(uint8_t sample = 0; sample < m_config.numSamples; sample++)
    {
        voltageSum += samples[sample];
    }

    //calculation of voltage average (static_cast, we do not care about the lost decimal digits)
    uint16_t averagedVoltage { static_cast<uint16_t>((voltageSum/validSamples)*(1/m_config.voltageDividerRatio)) };

    //we need to constrain the voltage between the maximum and minimum voltage
    //so the percentage stays in the desired range
    averagedVoltage = constrain(averagedVoltage, m_config.minimumVoltage, m_config.maximumVoltage);
    
    //map the averaged voltage from 1 to 99 %
    m_percentage = map(averagedVoltage, m_config.minimumVoltage, m_config.maximumVoltage, 1, 99);

}
