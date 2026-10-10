#include "WiFiCtrl.h"
#include <string>

WiFiCtrl::WiFiCtrl(const WiFiConfig& config): m_config { config } {};

void WiFiCtrl::checkStatus() {

    //are we connecting now?
    if(m_status == wifiStatus::Connecting) {
        uint32_t connectingDuration { millis() - m_connectingStartMillis };

        //has the connection duration exceeded the maximum time?
        if(connectingDuration >= m_config.connectionTimeout) {
            disconnect();
            m_status = wifiStatus::Timeout;
        }
    }
    
    //are we connected yet?
    if (WiFi.status() == WL_CONNECTED) {
        m_status = wifiStatus::Connected;
    }

    //did we lose the connection?
    else if (WiFi.status() == WL_CONNECTION_LOST) {
        //turn off the WiFi interface
        disconnect();
    }
}

void WiFiCtrl::connect() {

    //start the connection
    WiFi.mode(WIFI_STA);
    WiFi.begin(m_config.ssid, m_config.password);

    //update the state variables
    m_connectingStartMillis = millis();
    m_status = wifiStatus::Connecting;
}

int WiFiCtrl::status() {

    //check the state of the connection
    checkStatus();
    return m_status;
}

void WiFiCtrl::disconnect() {

    //disconnect from wifi
    WiFi.disconnect(true);

    //turn off the WiFi interface
    WiFi.mode(WIFI_OFF);
    m_status = wifiStatus::Disconnected;
}