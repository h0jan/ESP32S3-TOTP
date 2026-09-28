#pragma once
#include "driver/rtc_io.h"

void sleep(gpio_num_t wakeupPin) {

  //prepare the interrupt pin, which wakes up the microcontroller
  rtc_gpio_init(wakeupPin);
  rtc_gpio_pulldown_dis(wakeupPin);
  rtc_gpio_pullup_en(wakeupPin);
  esp_sleep_enable_ext0_wakeup(wakeupPin, 0);

  //start sleeping
  esp_deep_sleep_start();
}