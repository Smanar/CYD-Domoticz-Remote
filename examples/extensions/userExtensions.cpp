#include "userExtensions.h"

// This defines a timer to implement userLoop (demo, to be removed)
unsigned long userExtentionsTimer = 0;

// This user extension setup does nothing but displaying a message
// That's a good place to init your devices or data
void userSetup(void) {
    Serial.println("Starting user setup extension");
}

// This user extension loop displays a message every minute
void userLoop(void) {
    if (millis() - userExtentionsTimer > 60000) {
        Serial.println("User loop timer expired...");
        userExtentionsTimer = millis();
    }
}