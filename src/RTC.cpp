#include <RTC.h>

bool RTC::newSecondStarted(time_t& currentUnixTime) {

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

bool RTC::begin() {     
    return m_rtc.begin();
}

bool RTC::lostPower() {
    return m_rtc.lostPower();
}

bool RTC::isSynchronized(bool type) {
    if(type == RtcType::Internal) {
        return m_intertalSynchronized;
    }
    else {
        return m_externalSynchronized;
    }
}

void RTC::resetSyncState(bool type) {
    if (type == RtcType::Internal) {
        m_intertalSynchronized = false;
    }
    else {
        m_externalSynchronized = false;
    }
}

time_t RTC::getUnixTime() {
    time_t currentTime;

    //get time from the internal RTC
    time(&currentTime);

    return currentTime;
}

void RTC::synchronize(bool type) {
    if (type == RtcType::Internal) {

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

            m_intertalSynchronized = true;
        }
    }
    else {
        
        time_t currentUnixTime { getUnixTime() };

        //right after a new second starts, we update the external RTC with the current time
        if(newSecondStarted(currentUnixTime)) {
            m_rtc.adjust(DateTime(currentUnixTime));

            m_externalSynchronized = true;
        }
    }
}