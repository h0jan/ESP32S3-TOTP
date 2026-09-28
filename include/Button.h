#pragma once
#include <Arduino.h>

struct ButtonConfig {
    gpio_num_t pin;
    uint32_t minValidDuration;
};

class Button {
private:

    const ButtonConfig& m_config;

    bool m_firstRun { true };
    bool m_lastState {};
    bool m_lastValidState { true };
    uint32_t m_stateStartMillis {};
    uint32_t m_stateDuration {};
    uint32_t m_validStateStartMillis {};
    uint32_t m_validStateDuration {};

public:

    Button(const ButtonConfig& config): m_config { config } 
    { 
        //set the button pin as input
        pinMode (m_config.pin, INPUT_PULLUP);
    };

    //press is represented as logic 0, release as logic 1
    enum ButtonState {
        Pressed,
        Released
    };

    void update() {

        //timers are cleared at the first call
        if(m_firstRun) {
            clearTimers();
            m_firstRun = false;
        }

        bool currentState { digitalRead(m_config.pin) };
        uint32_t currentMillis { millis() };

        //did the button state change?
        if(currentState != m_lastState) {

            //if so, we save the state and it's starting time
            m_lastState = currentState;
            m_stateStartMillis = currentMillis;
        }

        //calculation of the state duration
        m_stateDuration = currentMillis - m_stateStartMillis;

        //has the duration reached the validation threshold?
        if(m_stateDuration >= m_config.minValidDuration)
        {
            //if so, we make the state valid
            m_lastValidState = m_lastState;
            m_validStateStartMillis = m_stateStartMillis;
        }

        //calculation of the valid state duration
        m_validStateDuration = currentMillis - m_validStateStartMillis;
    }

    void clearTimers() {
        uint32_t nowMillis { millis() };
        m_stateStartMillis = nowMillis;
        m_validStateStartMillis = nowMillis;
    }

    bool getState() { return m_lastValidState; }
    uint32_t getDuration() { return m_validStateDuration; }

};