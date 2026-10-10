
#include "Activities.h"

//you can configure the program here
#include "configuration.h"

//language of the displayed infotext can be changed by including the corresponding infotext header file
#include "infotexts/infotext-en.h"


ActivityHandler handler { Configuration::activity };

void setup() {
  handler.init();
}

void loop() {
  handler.loop();
}
