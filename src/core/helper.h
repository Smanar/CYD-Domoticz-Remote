#pragma once

#include "Arduino.h"
#include "data_setup.h"
#include "../conf/global_config.h"

//  Used to debug routine entry and exit, as well as generic data
#define trace_enter() Serial.printf("Entering %s\n", __func__)
#define trace_exit() Serial.printf("Exiting %s:%d\n", __func__, __LINE__);
#define trace_params(fmt, ...) Serial.printf("%s:%d " fmt "\n", __func__, __LINE__, __VA_ARGS__)

void loadInfo(char* textChar, size_t textSize);
void getType(unsigned short type, char* typeText, size_t textLen);
void getPanelName(int widgetPageIndex, char* pageText, size_t textLen);
bool isActivePanel(int widgetPageIndex);
