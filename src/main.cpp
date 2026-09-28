#include <Arduino.h>
#include <TOTP.h>
#include <Wire.h>

#include "Battery.h"
#include "Button.h"
#include "Display.h"
#include "NTP.h"
#include "RTC.h"
#include "sleep.h"
#include "WiFiCtrl.h"

//you can configure the program here
#include "configuration.h"

//language of the displayed infotext can be changed by including the corresponding header file
#include "infotexts/infotext-en.h"


Battery battery { Configuration::battery };
Button button { Configuration::button };
Display display { Configuration::display };
NTP ntp { Configuration::ntp };
RTC rtc;
WiFiCtrl wifi { Configuration::wifi };
TOTP totp { Configuration::secret, 20 };


enum Actions {
  Nothing,
  ShowingMainScreen,
  UpdatingTime,
  Sleeping
};


void setup() {
  Wire.begin();
  
  display.begin();

  //try to initialize RTC
  if(!rtc.begin())
  {
    display.showInfo(info::rtcInitFailed);
    delay(2000);
  
  }

  //check if RTC lost power during sleep
  else if(rtc.lostPower()) {
    display.showInfo(info::rtcLostPower);
    delay(2000);
  }

  //battery percentage measurement
  battery.measurePercentage();
}

//what activity is currently selected by holding the button?
void getAction(int& action) {

  //read state and calculate duration
  button.update();

  bool buttonState { button.getState() };
  uint32_t buttonStateDuration { button.getDuration() };

  //button is pressed
  if(buttonState == Button::Pressed) {

    //long press shows the main screen
    if((buttonStateDuration >= Configuration::TOTPHoldMs) && (buttonStateDuration < Configuration::NTPSyncHoldMs)) {
      action = ShowingMainScreen;
    }

    //even longer press initiates time synchronization with an NTP server
    else if (buttonStateDuration >= Configuration::NTPSyncHoldMs) {
      action = UpdatingTime;
    }
  }

  //button is released
  else {
    //after some time of inactivity, the device is put to sleep
    if(buttonStateDuration >= Configuration::sleepTimeoutMs) {
      action = Sleeping;
    }
  }
}

//WiFi connection control
int connectToWiFi() {

  int connectionState { wifi.status() };

  switch(connectionState) {
  case(WiFiCtrl::Disconnected):
    wifi.connect();
  break;
  case(WiFiCtrl::Connecting):
    display.showInfo(info::connectingWifi, true);
  break;
  case(WiFiCtrl::Timeout):
    wifi.disconnect();
    display.showInfo(info::wifiTimeout);
    delay(5000);
  break;
  }

  return connectionState;
}

//NTP synchronization control
int getNtpTime() {

  int syncState { ntp.status() };
  
  switch(syncState) {
  case(NTP::Inactive):
    ntp.sync();
  break;
  case(NTP::WaitingForResponse):
    display.showInfo(info::ntpWaiting, true);
  break;
  case(NTP::Timeout):
    //resetujeme stav pro případnou další synchronizaci
    ntp.resetStatus();
    display.showInfo(info::ntpTimeout);
    delay(5000);
  break;
  }

  return syncState;
}

//update the time using an NTP server
void updateTime() {

  display.showInfo(info::updateStarted);
  delay(3000);

  //try to connect to WiFi
  int status { connectToWiFi() };
  while(status != WiFiCtrl::Connected) {
    status = connectToWiFi();

    if(status == WiFiCtrl::Timeout) {
      return;
    }
  }

  //request time from NTP server
  status = getNtpTime();
  while(status != NTP::Success) {
    status = getNtpTime();

    if(status == NTP::Timeout) {
      return;
    }
  }

  //save the new time to external RTC
  while(!rtc.syncExternalRTC());

  wifi.disconnect();

  display.showInfo(info::updateSuccessful);
  delay(3000);
}

//synchronize internal RTC with external RTC when the program starts
void syncRtcAtStart() {
  static bool internalRtcSynchronized { false };

  //after the synchronization is completed, "rtc.syncInternalRTC()" returns "true"
  if(!internalRtcSynchronized) {
    internalRtcSynchronized = rtc.syncInternalRTC();
  }
}

void showMainScreen() {

  //prepare needed values
  time_t currenUnixTime { rtc.getUnixTime() };
  const char* code { totp.getCode(currenUnixTime) };
  int batteryPercentage { battery.getPercentage() };

  //pass the values and show the main screen
  display.showMainScreen(currenUnixTime, batteryPercentage, code);
};

//get the current action and act upon it
void handleAction() {
  static int currentAction { Nothing };
  static int mainScreenWasShown { false };

  //get the current action
  getAction(currentAction);

  //which activity are we doing right now?
  switch(currentAction) {
  case(ShowingMainScreen):
    mainScreenWasShown = true;
    showMainScreen();
  break;

  case(UpdatingTime):
    updateTime();
    //we need to clear the button timers
    //we do not want to account for the time, which has passed during the update
    button.clearTimers();
    //continue to the main screen
    currentAction = ShowingMainScreen;
  break;

  case(Sleeping):
  //we close the creen only if the main screen was shown before
  //if no main screen was shown, there is nothing to close
  if(mainScreenWasShown) {
    display.closeScreen();
  }
    sleep(Configuration::button.pin);
  break;
  }
}

void loop() {
  syncRtcAtStart();
  handleAction();
}
