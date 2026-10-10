#pragma once
#include "infotext-declarations.h"

const char* info::rtcInitFailed {
    "> Chyba inicializace\n" 
    "  externich hodin!"
};

const char* info::rtcLostPower {
    "> Externi hodiny\n"
    "  ztratily napajeni!\n"
    "  Aktualizujte cas!"
};

const char* info::updateStarted {
    "> Zahajeni\n"
    "  aktualizace casu!"
};

const char* info::connectingWifi {
    "> Pripojovani\n"
    "  k WiFi"
};

const char* info::wifiTimeout {
    "> Pripojeni k WiFi\n"
    "  se nezdarilo!"
};

const char* info::ntpWaiting {
    "> Cekani na odezvu\n"
    "  NTP serveru"
};

const char* info::ntpTimeout {
    "> Zadna odpoved NTP\n"
    "  serveru!"
};

const char* info::updateSuccessful {
    "> Aktualizace casu\n"
    "  uspesna!"
};
