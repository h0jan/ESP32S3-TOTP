#pragma once
#include "infotext-declarations.h"

const char* info::rtcInitFailed {
    "> Initialization of\n" 
    "  the external clock \n"
    "  failed!"
};

const char* info::rtcLostPower {
    "> The external clock\n"
    "  lost power, update\n"
    "  your time!"
};

const char* info::updateStarted {
    "> Time update\n"
    "  started!"
};

const char* info::connectingWifi {
    "> Connecting to\n"
    "  WiFi"
};

const char* info::wifiTimeout {
    "> Connection to\n"
    "  WiFi failed!"
};

const char* info::ntpWaiting {
    "> Waiting for NTP\n"
    "  server response"
};

const char* info::ntpTimeout {
    "> NTP server did not\n"
    "  respond!"
};

const char* info::updateSuccessful {
    "> Time update was\n"
    "  successful!"
};