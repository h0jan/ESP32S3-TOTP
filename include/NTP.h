#pragma once
#include <Arduino.h>
#include "esp_sntp.h"

//configuration constants (set in configuration.h)
struct NTPConfig {
    const char* timeZone;
    const char* ntpServer;
    const uint32_t responseTimeout;
};

namespace ntpStatus {
enum {
    Inactive,
    WaitingForResponse,
    Timeout,
    Success
};
}

class NTP {
private:
    const NTPConfig& m_config;

    uint32_t m_NTPSyncStartMillis {};
    int m_status { ntpStatus::Inactive };

    //updates the status of NTP synchronization
    void checkNtpSyncStatus();

public:
    NTP(const NTPConfig&);

    //requests the time from an NTP server
    void sync();

    //gets the status of NTP synchronization
    int status();

    //resets the status of NTP synchronization to "Inactive"
    void resetStatus();
};
