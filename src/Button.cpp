#include "Button.h"

Button::Button(const ButtonConfig& config): m_config { config } { 
    //set the button pin as input
    pinMode (m_config.pin, INPUT_PULLUP);
};

void Button::getStates(bool& state, uint32_t& duration) {

    updateStates();
    state = m_lastValidState;
    duration = m_validStateDuration;
}

void Button::clearTimers() {

    uint32_t nowMillis { millis() };
    m_stateStartMillis = nowMillis;
    m_validStateStartMillis = nowMillis;
}

void Button::updateStates() {

    //timers are cleared at the first call
    static bool firstRun { true };
    if(firstRun) {
        clearTimers();
        firstRun = false;
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