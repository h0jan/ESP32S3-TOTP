#include "NTP.h"

NTP::NTP(const NTPConfig& config): m_config { config } {

    //set the timezone
    setenv("TZ", m_config.timeZone, 1);
    tzset();
};

void NTP::checkNtpSyncStatus() {

    //are we waiting for the server's response?
    if(m_status == ntpStatus::WaitingForResponse) {

        //calculate the time elapsed since starting the synchronization
        uint32_t syncDuration { millis() - m_NTPSyncStartMillis };

        //did we cross the sync duration limit?
        if(syncDuration >= m_config.responseTimeout) {
            m_status = ntpStatus::Timeout;
        }
    }

    //did we get the server's response?
    if(sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED) {
        m_status = ntpStatus::Success;
    }
}

void NTP::sync() {

    //send a request to NTP server and update the states
    configTzTime(m_config.timeZone, m_config.ntpServer);
    m_NTPSyncStartMillis = millis();
    m_status = ntpStatus::WaitingForResponse;
}

int NTP::status() {
    checkNtpSyncStatus();
    return m_status;
}

void NTP::resetStatus() { 
    m_status = ntpStatus::Inactive;
}