#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#if 0
// Panel Settings
#define TFT_HSYNC_POLARITY 1
#define TFT_HSYNC_FRONT_PORCH 10
#define  TFT_HSYNC_PULSE_WIDTH 8
#define  TFT_HSYNC_BACK_PORCH 50
#define  TFT_VSYNC_POLARITY 1
#define TFT_VSYNC_FRONT_PORCH 10
#define  TFT_VSYNC_PULSE_WIDTH 8
#define  TFT_VSYNC_BACK_PORCH 20
#define TFT_PCLK_ACTIVE_NEG 1
#define  TFT_PREFER_SPEED 12000000
#define  TFT_AUTO_FLUSH 1
#endif

//Color
#define TFT_BLACK       0x0000
#define TFT_WHITE       0xFFFF

#define GFX_BL 21

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS , TFT_SCLK , TFT_MOSI , TFT_MISO);
Arduino_ILI9341 tft = Arduino_ILI9341(bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */, true );

