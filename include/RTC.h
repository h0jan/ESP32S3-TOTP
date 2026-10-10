#pragma once
#include <RTClib.h>

namespace RtcType {
enum {
    Internal,
    External
};
}

class RTC {
private:
    RTC_DS3231 m_rtc;
    time_t m_startSecond {};

    bool m_intertalSynchronized { false };
    bool m_externalSynchronized { false };

    //determine the start of a new second
    bool newSecondStarted(time_t& currentUnixTime);

public:

    //initialize the external RTC
    bool begin();

    //has the external RTC lost power?
    bool lostPower(); 

    //get the clock's synchronization state
    bool isSynchronized(bool type);

    //reset the clock's synchronization state
    void resetSyncState(bool type);
    
    //get current unix timestamp
    time_t getUnixTime();

    //Internal - external clock time is set into the internal clock
    //External - internal clock time is set into the external clock
    void synchronize(bool type);
};