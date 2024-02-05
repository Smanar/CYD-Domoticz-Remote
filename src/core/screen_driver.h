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
void set_color_scheme();
void screen_setup();
void set_invert_display();
void screen_timer_wake();
void set_screen_timer_period();
void set_screen_brightness();

#endif // _SCREEN_DRIVER_INIT