#include <Arduino.h>
#include <Arduino_GFX_Library.h>

//Color
#define TFT_BLACK       0x0000
#define TFT_WHITE       0xFFFF

//#define GFX_BL 45
//const int freq = 1000;
//const int ledChannel = 7;
//const int resolution = 8;


// Arduino_ESP32RGBPanel * bus = new Arduino_ESP32RGBPanel(
// 5 /* DE */, 3 /* VSYNC */, 46 /* HSYNC */, 7 /* PCLK */,
// 1 /* R0 */, 2 /* R1 */, 42 /* R2 */, 41 /* R3 */, 40 /* R4 */,
// 39 /* G0 */, 0 /* G1 */, 45 /* G2 */, 48 /* G3 */, 47 /* G4 */, 21 /* G5 */,
// 14 /* B0 */, 38 /* B1 */, 18 /* B2 */, 17 /* B3 */, 10 /* B4 */,
// 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
// 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
// 1 /* pclk_active_neg */, 16000000 /* prefer_speed */
// );


Arduino_ESP32RGBPanel * bus = new Arduino_ESP32RGBPanel(
5 /* DE */, 3 /* VSYNC */, 46 /* HSYNC */, 7 /* PCLK */,
14 /* R0 */, 38 /* R1 */, 18 /* R2 */, 17 /* R3 */, 10 /* R4 */,
39 /* G0 */, 0 /* G1 */, 45 /* G2 */, 48 /* G3 */, 47 /* G4 */, 21 /* G5 */,
1 /* B0 */, 2 /* B1 */, 42 /* B2 */, 41 /* B3 */, 40 /* B4 */,
0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
1 /* pclk_active_neg */, 16000000 /* prefer_speed */
);

Arduino_RGB_Display tft = Arduino_RGB_Display( 800 /* width */, 480 /* height */, bus);