#pragma once
#include "Battery.h"
#include "Button.h"
#include "WiFiCtrl.h"
#include "NTP.h"
#include "Display.h"

namespace Configuration {

//the TOTP secret 
//(TOTP library requires the secret not to be a constant)
uint8_t TOTPsecret[20] 
{ 
    0x4a, 0x75, 0x73, 0x74, 0x20, 
    0x61, 0x20, 0x64, 0x75, 0x6d, 
    0x6d, 0x79, 0x20, 0x6B, 0x65, 
    0x79, 0x20, 0x3a, 0x28, 0x20
};

//button configuration
constexpr ButtonConfig button {
    .pin = GPIO_NUM_9,          //button pin (must be one of the GPIO_RTC)
    .minValidDuration = 50      //the minimal duration to mark the state as valid (ms)
};

//battery configuration
constexpr BatteryConfig battery {
    .pin = GPIO_NUM_1,              //battery voltage measurement pin (must support ADC !)
    .numSamples = 5,                //number of voltage samples taken during a measurement (must be higher than 3 !)
    .voltageDividerRatio = 0.5f,    //ratio of the voltage divider [ R1 / (R1+R2) ]
    .minimumVoltage = 3050,         //voltage of drained battery (microcontroller stops working below 3000 mV, + 50mV reserve)
    .maximumVoltage = 4200          //voltage of fully charged battery (the charge controller stops charging at 4200 mV)
};

//WiFi configuration
constexpr WiFiConfig wifi {
    .ssid = "SSID" ,            //SSID
    .password = "PASSWORD" ,    //password
    .connectionTimeout = 10000  //maximum connecting time (ms)
};

//NTP configuration
constexpr NTPConfig ntp {
    .timeZone = "CET-1CEST,M3.5.0,M10.5.0/3",   //time zone   https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
    .ntpServer = "ntp.cesnet.cz",               //address of NTP server    
    .responseTimeout = 10000                    //maximum waiting time for a response from the server (ms)
};

//display configuration
constexpr DisplayConfig display {
    .address = 0x3C,        //I2C display address
    .indicatorSpeed = 3,    //speed of the waiting indicator rotation (0 - 5)
    .animationSpeed = 2     //speed of the display opening/closing animation (0 - 5)
};

//action times configuration
constexpr uint16_t TOTPHoldMs { 750 };          //duration of a button press for showing the main screen (ms)
constexpr uint16_t NTPSyncHoldMs { 7000 };      //duration of a button press for synchronizing the time (ms)
constexpr uint16_t sleepTimeoutMs { 7000 };     //duration for which to keep the device awake after releasing the button (ms)

}