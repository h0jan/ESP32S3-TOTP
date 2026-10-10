#pragma once
#include <WiFi.h>

//configuration constants (set in configuration.h)
struct WiFiConfig {
    const char* ssid;
    const char* password;
    const uint32_t connectionTimeout; 
};

namespace wifiStatus {
enum {
    Connected,
    Disconnected,
    Connecting,
    Timeout
};
}

class WiFiCtrl {
private:

    const WiFiConfig& m_config;

    int m_status { wifiStatus::Disconnected };
    uint32_t m_connectingStartMillis {};

    void checkStatus();


public:

    WiFiCtrl(const WiFiConfig&);

    void connect();

    int status();

    void disconnect();
};