#pragma once
#include <Arduino.h>
#include "esp_sntp.h"

struct NTPConfig {
    const char* timeZone;
    const char* ntpServer;
    const uint32_t responseTimeout;
};

class NTP {
private:
    const NTPConfig& m_config;

    uint32_t m_NTPSyncStartMillis {};
    int m_status { Inactive };


    void checkNTPSyncStatus() {

        //are we waiting for the server's response?
        if(m_status == WaitingForResponse) {

            //calculate the time elapsed since starting the synchronization
            uint32_t syncDuration { millis() - m_NTPSyncStartMillis };

            //did we cross the sync duration limit?
            if(syncDuration >= m_config.responseTimeout) {
                m_status = Timeout;
            }
        }

        //did we get the server's response?
        if(sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED) {
            m_status = Success;
        }
    }

public:

    NTP(const NTPConfig& config): m_config { config }
    {
        //set the timezone
        setenv("TZ", m_config.timeZone, 1);
        tzset();
    };

    enum NTPstatus {
        Inactive,
        WaitingForResponse,
        Timeout,
        Success
    };

    void sync() {
        //send a request to NTP server and update the states
        configTzTime(m_config.timeZone, m_config.ntpServer);
        m_NTPSyncStartMillis = millis();
        m_status = WaitingForResponse;
    }

    int status() {
        checkNTPSyncStatus();
        return m_status;
    }

    void resetStatus() { m_status = Inactive; }
};
