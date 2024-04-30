#include <Arduino.h>
#include <Arduino_GFX_Library.h>

//Color
#define TFT_BLACK       0x0000
#define TFT_WHITE       0xFFFF

#define GFX_BL 21

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS , TFT_SCLK , TFT_MOSI , TFT_MISO);
Arduino_ILI9342 tft = Arduino_ILI9342(bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */, false );

