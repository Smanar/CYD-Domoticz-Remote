#include <Arduino.h>
#include <Arduino_GFX_Library.h>

//Color
#define TFT_BLACK       0x0000
#define TFT_WHITE       0xFFFF

#define GFX_BL 45
//const int freq = 1000;
//const int ledChannel = 7;
//const int resolution = 8;

Arduino_ESP32RGBPanel * bus = new Arduino_ESP32RGBPanel(
39 /* DE */, 38 /* VSYNC */, 5 /* HSYNC */, 9 /* PCLK */,
10 /* R0 */, 11 /* R1 */, 12 /* R2 */, 13 /* R3 */, 14 /* R4 */,
21 /* G0 */, 0 /* G1 */, 46 /* G2 */, 3 /* G3 */, 8 /* G4 */, 18 /* G5 */,
17 /* B0 */,16 /* B1 */, 15 /* B2 */, 7 /* B3 */,6 /* B4 */,
0 /* hsync_polarity /, 0 /* hsync_front_porch */, 210 /* hsync_pulse_width */, 30 /* hsync_back_porch */,
0 /* vsync_polarity */, 0 /* vsync_front_porch */, 22 /* vsync_pulse_width */, 13 /* vsync_back_porch */,
1 /* pclk_active_neg */, 16000000 /* prefer_speed */
);

Arduino_RGB_Display tft = Arduino_RGB_Display( 800 /* width */, 480 /* height */, bus);