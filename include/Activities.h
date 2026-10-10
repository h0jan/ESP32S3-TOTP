#pragma once
#include <Arduino.h>

struct ActivityConfig {
    uint32_t totpHoldMs;
    uint32_t ntpSyncHoldMs;
    uint32_t sleepTimeoutMs;
};

//activity functions

//synchronize internal RTC with external RTC when the program starts
void syncInternalRtc();

//show the main screen with 
void showMainScreen();

//update the time using an NTP server
void updateTime();

//turn off the screen and go to sleep
void sleep();


class Activity {
private:
    void (*m_activity) (void);

public:
    Activity(void (*activity) (void));

    void execute() {
        m_activity();
    }
};

class ActivityHandler {
private:

    const ActivityConfig& m_config;

    Activity* m_currentActivity;
    
    struct Activities {
        Activity nothing { nullptr };
        Activity synchronizingRtc { syncInternalRtc };
        Activity showingMainScreen { showMainScreen };
        Activity updatingTime { updateTime };
        Activity sleeping { sleep };
    } m_activities;

    //what activitiey are we doing next?
    void resolveActivity();

    //set the activity, which will be executed next
    void setActivity(Activity&);

public:
    ActivityHandler(const ActivityConfig&);

    void init();

    void loop();
};


