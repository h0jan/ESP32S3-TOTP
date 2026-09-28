#pragma once
#include <WiFi.h>

struct WiFiConfig {
    const char* ssid;
    const char* password;
    const uint32_t connectionTimeout; 
};

class WiFiCtrl {
private:

    const WiFiConfig& m_config;

    int m_status { Disconnected };
    uint32_t m_connectingStartMillis {};

    void checkStatus() {

        //are we connecting now?
        if(m_status == Connecting) {
            uint32_t connectingDuration { millis() - m_connectingStartMillis };

            //has the connection duration exceeded the maximum time?
            if(connectingDuration >= m_config.connectionTimeout) {
                disconnect();
                m_status = Timeout;
            }
        }
        
        //are we connected yet?
        if (WiFi.status() == WL_CONNECTED) {
            m_status = Connected;
        }

        //did we lose the connection?
        else if (WiFi.status() == WL_CONNECTION_LOST) {
            //turn off the WiFi interface
            disconnect();
        }
    }


public:

    WiFiCtrl(const WiFiConfig& config): m_config { config } {};

    enum ConnectionState {
        Connected,
        Disconnected,
        Connecting,
        Timeout
    };

    void connect() {

        //start the connection
        WiFi.mode(WIFI_STA);
        WiFi.begin(m_config.ssid, m_config.password);

        //update the state variables
        m_connectingStartMillis = millis();
        m_status = Connecting;
    }

    int status() {

        //check the state of the connection
        checkStatus();
        return m_status;
    }

    void disconnect() {

        //disconnect from wifi
        WiFi.disconnect(true);

        //turn off the WiFi interface
        WiFi.mode(WIFI_OFF);
        m_status = Disconnected;
    }
};