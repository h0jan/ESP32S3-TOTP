#pragma once
#include <Arduino.h>

namespace alignment {

    //configurable alignment related constants:

    //display size
    const uint8_t displayWidth { 128 }; 
    const uint8_t displayHeight { 64 }; 

    //font size
    const uint8_t defaultTextSize { 1 };  

    //time bar size
    const uint8_t spaceBeforeTimebar { 5 }; 
    const uint8_t timeBarRounding { 9 };    
    const uint8_t timeBarHeight { 11 };     

    //TOTP code
    const uint8_t codeTextSize { 2 };           
    const uint8_t spaceBetweenCodeParts { 7 };  

    //battery
    const uint8_t spaceBeforeBatPercentage { 3 };  



    
    //caltulations of object alignments (non-configurable):

    //size of characters (assumes we are using the default font)
    const uint8_t characterWidth { 5 * defaultTextSize };
    const uint8_t characterHeight { 7 * defaultTextSize };
    const uint8_t spaceBetweenCharacters { 1 * defaultTextSize };

    //alignment of the time bar
    const uint8_t timeBar_x { characterWidth * 3 + spaceBetweenCharacters * 2 + spaceBeforeTimebar };
    const uint8_t timeBar_y { displayHeight - timeBarHeight };
    const uint8_t timeBarWidth { displayWidth - timeBar_x };

    //alignment of the remaining time
    const uint8_t remainingTime_x { 0 };
    const uint8_t remainingTime_y { displayHeight - characterHeight - (timeBarHeight - characterHeight) / 2 };

    //alignment of the TOTP code
    const uint8_t relativeCenter_y { (displayHeight - characterHeight - timeBarHeight) / 2 + characterHeight };
    const uint8_t firstCodePart_x { displayWidth/2 - spaceBetweenCodeParts/2 - codeTextSize * (characterWidth * 3 + spaceBetweenCharacters * codeTextSize) };
    const uint8_t firstCodePart_y { relativeCenter_y - characterHeight/2 * codeTextSize };
    const uint8_t secondCodePart_x { displayWidth/2 + spaceBetweenCodeParts/2 };
    const uint8_t secondCodePart_y { firstCodePart_y };

    //alignment of the battery percentage and icon
    const uint8_t batteryPercentage_x { displayWidth-1 - characterWidth * 3 - spaceBetweenCharacters * 2 };
    const uint8_t batteryPercentage_y { 0 };
    const uint8_t batteryIcon_x { batteryPercentage_x - 8 - spaceBeforeBatPercentage };
    const uint8_t batteryIcon_y { 0 };
}
