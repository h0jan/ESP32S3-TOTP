#include "Display.h"

Display::Display(const DisplayConfig& config): m_config { config } {
    
    calculateSpeedVariables();
};

void Display::calculateSpeedVariables() {

    const uint8_t maxSpeed { 5 };

    //delay between indicator steps
    //level 5 means 50 ms, level 0 means 200 ms
    int maximum { 200 };
    int minimum { 50 };
    int delta { maximum - minimum };

    //calculating the step delay from the speed level
    m_indicatorStepDelay = delta * (1 - (static_cast<float>(m_config.indicatorSpeed)/maxSpeed)) + minimum;

    //row step span between the opening and closing animations
    //level 5 means a step of 7 rows, level 0 means a step of 1 row
    maximum = 7;
    minimum = 1;
    delta = maximum - minimum;

    //calculating the step span from the speed level
    m_animationStepRows = delta * static_cast<float>(m_config.animationSpeed)/maxSpeed + minimum;
}

void Display::stepWaitingIndicator() {

    //calculate duration since the last step
    uint32_t currentMillis { millis() };
    uint32_t delay { currentMillis - m_previousMillis };

    //did the duration exceed the step delay?
    if(delay >= m_indicatorStepDelay) {
        m_previousMillis = currentMillis;

        //move the index to next character
        ++m_currentSequenceIndex;

        //exceeding the last index moves the index back to the start
        if(m_currentSequenceIndex > 3) {
            m_currentSequenceIndex = 0;
        }
    }
}

void Display::prepareTopNotch(time_t currentUnixTime, uint8_t batteryPercentage) {

    //getting the local time
    struct tm* localTime { localtime(&currentUnixTime) };

    //conversion of "tm" structure to string in format HH:MM:SS
    char timeString[9];
    snprintf(timeString,sizeof(timeString),"%02d:%02d:%02d", localTime->tm_hour,localTime->tm_min,localTime->tm_sec);

    //conversion of battery percentage to string in format XX%
    char batteryString[8];
    snprintf(batteryString,sizeof(batteryString),"%02d%%", batteryPercentage);

    //show the current time in the upper left corner
    m_canvas.setCursor(0, 0);
    m_canvas.print(timeString);

    //show the battery icon and percentage in the upper right corner
    m_canvas.drawBitmap(alignment::batteryIcon_x, alignment::batteryIcon_y, m_baterie, 8, 8, SSD1306_WHITE);
    m_canvas.setCursor(alignment::batteryPercentage_x, alignment::batteryPercentage_y);
    m_canvas.print(batteryString);
}

void Display::prepareCode(const char* TOTPcode) {

    //we split the code to two parts for better reading
    char firstPart[4] { TOTPcode[0], TOTPcode[1], TOTPcode[2], '\0'};
    char secondPart[4] { TOTPcode[3], TOTPcode[4], TOTPcode[5], '\0'};

    //put the code in the center
    m_canvas.setTextSize(alignment::codeTextSize);
    m_canvas.setCursor(alignment::firstCodePart_x, alignment::firstCodePart_y);
    m_canvas.print(firstPart);
    m_canvas.setCursor(alignment::secondCodePart_x, alignment::secondCodePart_y);
    m_canvas.print(secondPart);
    m_canvas.setTextSize(alignment::defaultTextSize);
}

void Display::prepareTimeBar(time_t currentUnixTime) {

    //calculating the remaining time of the code validity
    time_t remainingTime = 30 - (currentUnixTime % 30);

    //convert the remaining time into a string in format XXs
    char timeString[4];
    snprintf(timeString, sizeof(timeString), "%02ds", remainingTime);

    //mapping the remaining time to the time bar width
    uint8_t barLength = map(remainingTime, 0, 30, 0, alignment::timeBarWidth);

    //show the remaining time
    m_canvas.setCursor(alignment::remainingTime_x, alignment::remainingTime_y);
    m_canvas.print(timeString);

    //show the time bar
    m_canvas.drawRoundRect(alignment::timeBar_x, alignment::timeBar_y, alignment::timeBarWidth, alignment::timeBarHeight, alignment::timeBarRounding, SSD1306_WHITE);
    m_canvas.fillRoundRect(alignment::timeBar_x, alignment::timeBar_y, barLength, alignment::timeBarHeight, alignment::timeBarRounding, SSD1306_WHITE);
}

void Display::openScreenAnimation() {

    //pointer to the starting address of the canvas
    const uint8_t *buffer = m_canvas.getBuffer();

    const int finalHeight { alignment::displayHeight / 2 };

    //we uncover a part of the main screen with each iteration
    for(uint8_t height = 0; height < finalHeight; height += m_animationStepRows) {

        //from which row do we start uncovering now?
        const int currentUpperRow { finalHeight - m_animationStepRows - height };
        const int currentLowerRow { finalHeight + height };

        //how many bytes correspond to the display width? (drawBitmap works with bytes)
        const int bytesPerRow { alignment::displayWidth / 8 };

        //calculate the first pixel address of each the rows, which we are uncovering from now
        const uint8_t* upStartAddress { buffer + ( currentUpperRow * bytesPerRow) };
        const uint8_t* downStartAddress { buffer + ( currentLowerRow * bytesPerRow) };

        //uncover the new rows
        m_display.drawBitmap(0, currentUpperRow, upStartAddress, alignment::displayWidth, m_animationStepRows, SSD1306_WHITE, SSD1306_BLACK);
        m_display.drawBitmap(0, currentLowerRow, downStartAddress, alignment::displayWidth, m_animationStepRows, SSD1306_WHITE, SSD1306_BLACK);

        //calculate indexes of the bordering lines
        const int upperLineRow { currentUpperRow - 1 };
        const int lowerLineRow { currentLowerRow + m_animationStepRows };

        //show the bordering lines
        //the bordering lines are intentionally drawn right below/above the uncovered rows
        //so they are overwritten by the newly uncovered rows in the next iteration
        m_display.drawFastHLine(0, upperLineRow, alignment::displayWidth, SSD1306_WHITE);
        m_display.drawFastHLine(0, lowerLineRow, alignment::displayWidth, SSD1306_WHITE);

        m_display.display();
    }
}

void Display::closeScreenAnimation() {

    const int finalHeight { alignment::displayHeight / 2 };

    //we cover a part of the main screen with each iteration
    for(uint8_t height = 0; height < finalHeight; height += m_animationStepRows) {

        //from which row do we start covering now?
        const int currentUpperRow { height };
        const int currentLowerRow { alignment::displayHeight - m_animationStepRows - height };

        //we cover a part of the screen
        m_display.fillRect(0, currentUpperRow, alignment::displayWidth, m_animationStepRows, SSD1306_BLACK);
        m_display.fillRect(0, currentLowerRow, alignment::displayWidth, m_animationStepRows, SSD1306_BLACK);

        //calculate indexes of the bordering lines
        const int upperLineRow { currentUpperRow + m_animationStepRows };
        const int lowerLineRow { currentLowerRow - 1 };

        //show the bordering lines
        m_display.drawFastHLine(0, upperLineRow, alignment::displayWidth, SSD1306_WHITE);
        m_display.drawFastHLine(0, lowerLineRow, alignment::displayWidth, SSD1306_WHITE);

        m_display.display();
    }

    m_display.clearDisplay();
    m_display.display();
}

void Display::begin() { 
    m_display.begin(SSD1306_SWITCHCAPVCC, m_config.address); 
    m_display.clearDisplay();
    m_display.setTextSize(alignment::defaultTextSize);
    m_display.setTextColor(SSD1306_WHITE);
};

//show a warning after opening the main screen
void Display::setWarning(const char* info, uint32_t duration) {
    m_warning = info;
    m_warningDuration = duration;
}

void Display::showWarning() {

    //for how long has the warning been displayed?
    uint32_t nowMillis { millis() };
    static uint32_t startMillis { nowMillis };
    uint32_t duration = nowMillis - startMillis;

    //put the warning on the canvas
    m_canvas.fillScreen(SSD1306_BLACK);
    m_canvas.setCursor(0,0);
    m_canvas.print(m_warning);

    //clear the warning after exceeding the displaying duration
    if(duration > m_warningDuration) {
        m_warning = nullptr;
    }
}

void Display::showInfo(const char* info, bool waitingIndicator) {
    m_display.clearDisplay();
    m_display.setCursor(0, 0);
    m_display.print(info);

    if(waitingIndicator) {
        stepWaitingIndicator();
        m_display.print(' ');
        m_display.print(m_waitingIndicatorSequence[m_currentSequenceIndex]);
    }

    m_display.display();
}

void Display::showMainScreen(time_t currentUnixTime, uint8_t batteryPercentage, const char* TOTPcode) {

    m_mainScreenWasShown = true;

    //show a warning, if it is present
    if(m_warning != nullptr) {
        showWarning();
    }

    //if no warning is present, proceed to the main screen
    else {
        m_canvas.fillScreen(SSD1306_BLACK);
        prepareTopNotch(currentUnixTime, batteryPercentage);
        prepareCode(TOTPcode);
        prepareTimeBar(currentUnixTime);
    }

    //opening animation is shown only after the first run
    static bool screenOpened { false };
    if(!screenOpened){
        screenOpened = true;
        openScreenAnimation();
    }

    else{
        m_display.drawBitmap(0, 0, m_canvas.getBuffer(), m_canvas.width(), m_canvas.height(), SSD1306_WHITE, SSD1306_BLACK);
        m_display.display();
    }
}

void Display::turnScreenOff() {

    //show the closing animation, if the main screen was on
    if(m_mainScreenWasShown) {
        closeScreenAnimation();
    }

    //turn off the display
    m_display.clearDisplay();
    m_display.display();
}