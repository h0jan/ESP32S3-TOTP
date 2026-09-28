#pragma once
#include <Arduino.h>
#include <RTClib.h>


class RTC {
private:
    RTC_DS3231 m_rtc;
    time_t m_startSecond {};

    bool newSecondStarted(time_t& currentUnixTime) {

        //get the timestamp at which the synchronization startes
        if(!m_startSecond) {
            m_startSecond = currentUnixTime;
        }

        //did the timestamp change?
        if(m_startSecond != currentUnixTime) {

            //a new second has just begun!
            m_startSecond = 0;
            return true;
        }

        //we are still waiting for the beginning of a new second
        return false;
    }

public:

    bool begin() { return m_rtc.begin(); }
    bool lostPower() { return m_rtc.lostPower(); }
    
    time_t getUnixTime() {
        time_t currentTime;

        //get time from the internal RTC
        time(&currentTime);

        return currentTime;
    }

    bool syncExternalRTC() {
        time_t currentUnixTime { getUnixTime() };

        //right after a new second starts, we update the external RTC with the current time
        if(newSecondStarted(currentUnixTime)) {
            m_rtc.adjust(DateTime(currentUnixTime));
            return true;
        }

        //still waiting for synchronization
        return false;
    }

    bool syncInternalRTC() {
        DateTime currentDateTime { m_rtc.now() };

        //unixtime() returns uint32_t, converting it to time_t
        time_t currentUnixTime { static_cast<time_t>(currentDateTime.unixtime()) };

        //right after a new second begins, we update the external RTC
        if(newSecondStarted(currentUnixTime)) {

            //čas se uloží do struktury
            struct timeval t;
            t.tv_sec = currentUnixTime;
            t.tv_usec = 0;

            //updating the time of the internal RTC
            settimeofday(&t, NULL);
            return true;
        }

        //still waiting fot synchronization
        return false;
    }
};