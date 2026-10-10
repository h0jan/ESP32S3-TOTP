#pragma once
#include <Arduino.h>

//configuration constants (set in configuration.h)
struct ButtonConfig {
    const gpio_num_t pin;
    const uint32_t minValidDuration;
};

//press is represented as logic 0, release as logic 1
namespace ButtonState {
enum {
    Pressed,
    Released
};
}


class Button {
private:

    const ButtonConfig& m_config;

    bool m_lastState {};
    bool m_lastValidState { true };

    uint32_t m_stateStartMillis {};
    uint32_t m_stateDuration {};
    uint32_t m_validStateStartMillis {};
    uint32_t m_validStateDuration {};

    //updates the state and timers
    void updateStates();

public:
    Button(const ButtonConfig&);

    //returns button state determined at the time of last "update()" call
    //bool getState();

    void getStates(bool& state, uint32_t& duration);

    //returns state duration determined at the time of last "update() call"
    //uint32_t getDuration();

    //resets the state timers (state duration becomes 0 at the moment of calling)
    void clearTimers();
};