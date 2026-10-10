#include "Activities.h"

#include <TOTP.h>
#include "infotexts/infotext-declarations.h"
#include "Battery.h"
#include "Button.h"
#include "configuration.h"
#include "Display.h"
#include "driver/rtc_io.h"
#include "Key.h"
#include "NTP.h"
#include "RTC.h"
#include "WiFiCtrl.h"


Button button { Configuration::button };
Battery battery { Configuration::battery };
Display display { Configuration::display };
NTP ntp { Configuration::ntp };
Key key { Configuration::secret };
RTC rtc;
WiFiCtrl wifi { Configuration::wifi };
TOTP totp { key.getKey(), key.getKeyLength() };


Activity::Activity(void (*activity) (void)): m_activity { activity } {};


ActivityHandler::ActivityHandler(const ActivityConfig& config):  m_config { config } 
{
    setActivity(m_activities.nothing);
};

void ActivityHandler::init() {
    Wire.begin();
    display.begin();

    //try to initialize RTC
    if(!rtc.begin()) {
        display.setWarning(info::rtcInitFailed, 3000);
    }

    //check if RTC lost power during sleep
    else if(rtc.lostPower()) {
        display.setWarning(info::rtcLostPower, 3000);
    }

    //battery percentage measurement
    battery.measurePercentage();
}

void ActivityHandler::resolveActivity() {

    //synchronize the internal rtc, if it has not been done yet
    if(!rtc.isSynchronized(RtcType::Internal)) {
        setActivity(m_activities.synchronizingRtc);
    }

    //after updating the time, we need to discard the accumulated durations in the button timers
    //we then proceed by returning to the main screen
    if(m_currentActivity == &m_activities.updatingTime) {
        button.clearTimers();
        setActivity(m_activities.showingMainScreen);
    }

    bool buttonState;
    uint32_t stateDuration;
    button.getStates(buttonState, stateDuration);

    //button is pressed
    if(buttonState == ButtonState::Pressed) {

        //long press shows the main screen
        if((stateDuration >= m_config.totpHoldMs) && (stateDuration < m_config.ntpSyncHoldMs)) {
            setActivity(m_activities.showingMainScreen);
        }

        //even longer press initiates time synchronization with an NTP server
        else if (stateDuration >= m_config.ntpSyncHoldMs) {
            setActivity(m_activities.updatingTime);
        }
    }

    //button is released
    else {
        //after some time of inactivity, the device is put to sleep
        if(stateDuration >= m_config.sleepTimeoutMs) {
            setActivity(m_activities.sleeping);
        }
    }
}

void ActivityHandler::setActivity(Activity& activity) {
    m_currentActivity = &activity;
}

void ActivityHandler::loop() {

    //find out, which activity are we doing now and execute it
    resolveActivity();
    m_currentActivity->execute();
}

//WiFi connection control
int connectToWiFi() {

    int connectionState { wifi.status() };

    switch(connectionState) {
    case(wifiStatus::Disconnected):
        wifi.connect();
    break;
    case(wifiStatus::Connecting):
        display.showInfo(info::connectingWifi, true);
    break;
    case(wifiStatus::Timeout):
        wifi.disconnect();
        display.showInfo(info::wifiTimeout);
        delay(5000);
    break;
    }

    return connectionState;
}

//NTP synchronization control
int getNtpTime() {

    int syncState { ntp.status() };
    
    switch(syncState) {
    case(ntpStatus::Inactive):
        ntp.sync();
    break;
    case(ntpStatus::WaitingForResponse):
        display.showInfo(info::ntpWaiting, true);
    break;
    case(ntpStatus::Timeout):
        //resetujeme stav pro případnou další synchronizaci
        ntp.resetStatus();
        display.showInfo(info::ntpTimeout);
        delay(5000);
    break;
    }

    return syncState;
}

void updateTime() {

    display.showInfo(info::updateStarted);
    delay(3000);

    int status{};

    //try to connect to the WiFi
    do {
        status = connectToWiFi();

        if(status == wifiStatus::Timeout) {
        return;
        }

    } while(status != wifiStatus::Connected);

    //request time from NTP server
    do {
        status = getNtpTime();

        if(status == ntpStatus::Timeout) { 
            return; 
        };

    } while(status != wifiStatus::Connected);


    //clear the clock's synchronization status (= not synchronized)
    rtc.resetSyncState(RtcType::External);

    //save the new time to the external RTC
    while(!rtc.isSynchronized(RtcType::External)) {
        rtc.synchronize(RtcType::External);
    }

    wifi.disconnect();

    display.showInfo(info::updateSuccessful);
    delay(3000);
}

void syncInternalRtc() {
    rtc.synchronize(RtcType::Internal);
}

void showMainScreen() {

    //prepare needed values
    time_t currentUnixTime { rtc.getUnixTime() };
    const char* code { totp.getCode(currentUnixTime) };
    int batteryPercentage { battery.getPercentage() };

    //pass the values and show the main screen
    display.showMainScreen(currentUnixTime, batteryPercentage, code);
};

void sleep() {

    //show the screen closing animation
    display.turnScreenOff();

    //prepare the interrupt pin, which wakes up the microcontroller
    rtc_gpio_init(Configuration::button.pin);
    rtc_gpio_pulldown_dis(Configuration::button.pin);
    rtc_gpio_pullup_en(Configuration::button.pin);
    esp_sleep_enable_ext0_wakeup(Configuration::button.pin, 0);

    //start sleeping
    esp_deep_sleep_start();
}