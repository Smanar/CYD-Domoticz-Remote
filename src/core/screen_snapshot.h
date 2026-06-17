#pragma once

#include "lvgl.h"
#if LV_USE_SNAPSHOT != 0
    #include "Arduino.h"

    void takeSnapshot(void);
#endif
