#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "alignmentConstants.h"

struct DisplayConfig {
    uint8_t address;
    uint8_t indicatorSpeed;
    uint8_t animationSpeed;
};

class Display {
private:
    
    const DisplayConfig& m_config;

    //battery icon
    const uint8_t PROGMEM m_baterie[8] = { 0x18, 0x3C, 0x24, 0x2C, 0x34, 0x24, 0x3C, 0x00 };

    //animation and indicator speed variables
    uint16_t m_indicatorStepDelay;
    uint8_t m_animationStepRows;

    bool m_mainScreenWasShown { false };

    const char* m_warning { nullptr };
    uint32_t m_warningDuration {};

    //variables for stepping the indicator
    uint32_t m_previousMillis {};
    const char* m_waitingIndicatorSequence { "|/-\\" };
    uint8_t m_currentSequenceIndex { 0 };

    Adafruit_SSD1306 m_display{ alignment::displayWidth, alignment::displayHeight, &Wire, -1 };

    //canvas for the screen opening animation
    //it is used for an optimization the animation's speed
    GFXcanvas1 m_canvas{ alignment::displayWidth, alignment::displayHeight };

    //calculation of variables determining the speed of animations
    void calculateSpeedVariables();

    //steps the waiting indicator in configured time intervals
    void stepWaitingIndicator();

    //show a warning message
    void showWarning();

    //formats and adds the top part of the main screen to the canvas
    void prepareTopNotch(time_t currentUnixTime, uint8_t batteryPercentage);

    //formats and adds the TOTP code to the canvas (main screen)
    void prepareCode(const char* TOTPcode);

    //formats and adds the time bar to the canvas (main screen)
    void prepareTimeBar(time_t currentUnixTime);

    //handles the screen opening animation
    void openScreenAnimation();

    //handles the screen closing animation
    void closeScreenAnimation();


public:
    Display(const DisplayConfig&);

    //initializes the display
    void begin();

    //shows a string and an optional waiting indicator
    //needs to be called repeatedly if showing the waiting indicator
    void showInfo(const char* info, bool waitingIndicator = false);

    //set a warning to be shown at the beginning of displaying the mainscreen
    void setWarning(const char* info, uint32_t duration);

    //shows the main screen (also plays the opening animation, if it has not been played yet)
    void showMainScreen(time_t currentUnixTime, uint8_t batteryPercentage, const char* TOTPcode);

    //plays the closing animations and turns off the screen
    void turnScreenOff();
};