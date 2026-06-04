#pragma once

#include <Esp.h>

#ifndef _SCREEN_DRIVER_INIT
#define _SCREEN_DRIVER_INIT

#define CPU_FREQ_HIGH 240
#define CPU_FREQ_LOW 80


void touchscreen_calibrate(bool force = false);
void screen_setBrightness(byte brightness);
void screen_timer_setup();
void screen_timer_start();
void screen_timer_stop();
void screen_timer_period(uint32_t period);
void screen_timer_wake();
void set_screen_timer_period();
void home_timer_setup();
void home_timer_start();
void home_timer_stop();
void home_timer_period(uint32_t period);
void home_timer_wake();
void set_home_timer_period();
void set_color_scheme();
void screen_setup();
void set_invert_display();
void set_screen_brightness();
#ifdef ESP32_S3TOUCHLCD7
    void ioExpanderInit(void);
#endif

#endif // _SCREEN_DRIVER_INIT
