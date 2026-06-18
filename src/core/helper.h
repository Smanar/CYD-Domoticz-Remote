#pragma once

#include "Arduino.h"
#include "data_setup.h"
#include "../conf/global_config.h"

void loadInfo(char* textChar, size_t textSize);
void getType(unsigned short type, char* typeText, size_t textLen);
void getPanelName(int widgetPageIndex, char* pageText, size_t textLen);
bool isActivePanel(int widgetPageIndex);
