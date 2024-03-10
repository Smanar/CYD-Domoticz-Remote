#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Bus Settings
#define TFT_CS 39
#define TFT_SCLK 48
#define TFT_MOSI 47
#define TFT_DE 18
#define TFT_VSYNC 17
#define TFT_HSYNC 16
#define TFT_PCLK 21
#define TFT_R0 11
#define TFT_R1 12
#define TFT_R2 13
#define TFT_R3 14
#define TFT_R4 0
#define TFT_G0 8
#define TFT_G1 20
#define TFT_G2 3
#define TFT_G3 46
#define TFT_G4 9
#define TFT_G5 10
#define TFT_B0 4
#define TFT_B1 5
#define TFT_B2 6
#define TFT_B3 7
#define TFT_B4 15
#define TFT_DC -1
#define TFT_MISO -1
#define TFT_RST -1
#define TFT_BUSY -1
#define TFT_BCKL 38

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

//Color
#define TFT_BLACK       0x0000
#define TFT_WHITE       0xFFFF


static const uint8_t st7701_4848S040_init_operations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x10,

    WRITE_C8_D16, 0xC0, 0x3B, 0x00,
    WRITE_C8_D16, 0xC1, 0x0D, 0x02,
    WRITE_C8_D16, 0xC2, 0x31, 0x05,
    WRITE_C8_D8, 0xCD, 0x00,//0x08

    WRITE_COMMAND_8, 0xB0, // Positive Voltage Gamma Control
    WRITE_BYTES, 16,
    0x00, 0x11, 0x18, 0x0E,
    0x11, 0x06, 0x07, 0x08,
    0x07, 0x22, 0x04, 0x12,
    0x0F, 0xAA, 0x31, 0x18,

    WRITE_COMMAND_8, 0xB1, // Negative Voltage Gamma Control
    WRITE_BYTES, 16,
    0x00, 0x11, 0x19, 0x0E,
    0x12, 0x07, 0x08, 0x08,
    0x08, 0x22, 0x04, 0x11,
    0x11, 0xA9, 0x32, 0x18,

    // PAGE1
    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x11,

    WRITE_C8_D8, 0xB0, 0x60, // Vop=4.7375v
    WRITE_C8_D8, 0xB1, 0x32, // VCOM=32
    WRITE_C8_D8, 0xB2, 0x07, // VGH=15v
    WRITE_C8_D8, 0xB3, 0x80,
    WRITE_C8_D8, 0xB5, 0x49, // VGL=-10.17v
    WRITE_C8_D8, 0xB7, 0x85,
    WRITE_C8_D8, 0xB8, 0x21, // AVDD=6.6 & AVCL=-4.6
    WRITE_C8_D8, 0xC1, 0x78,
    WRITE_C8_D8, 0xC2, 0x78,

    WRITE_COMMAND_8, 0xE0,
    WRITE_BYTES, 3, 0x00, 0x1B, 0x02,

    WRITE_COMMAND_8, 0xE1,
    WRITE_BYTES, 11,
    0x08, 0xA0, 0x00, 0x00,
    0x07, 0xA0, 0x00, 0x00,
    0x00, 0x44, 0x44,

    WRITE_COMMAND_8, 0xE2,
    WRITE_BYTES, 12,
    0x11, 0x11, 0x44, 0x44,
    0xED, 0xA0, 0x00, 0x00,
    0xEC, 0xA0, 0x00, 0x00,

    WRITE_COMMAND_8, 0xE3,
    WRITE_BYTES, 4, 0x00, 0x00, 0x11, 0x11,

    WRITE_C8_D16, 0xE4, 0x44, 0x44,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 16,
    0x0A, 0xE9, 0xD8, 0xA0,
    0x0C, 0xEB, 0xD8, 0xA0,
    0x0E, 0xED, 0xD8, 0xA0,
    0x10, 0xEF, 0xD8, 0xA0,

    WRITE_COMMAND_8, 0xE6,
    WRITE_BYTES, 4, 0x00, 0x00, 0x11, 0x11,

    WRITE_C8_D16, 0xE7, 0x44, 0x44,

    WRITE_COMMAND_8, 0xE8,
    WRITE_BYTES, 16,
    0x09, 0xE8, 0xD8, 0xA0,
    0x0B, 0xEA, 0xD8, 0xA0,
    0x0D, 0xEC, 0xD8, 0xA0,
    0x0F, 0xEE, 0xD8, 0xA0,

    WRITE_COMMAND_8, 0xEB,
    WRITE_BYTES, 7,
    0x02, 0x00, 0xE4, 0xE4,
    0x88, 0x00, 0x40,

    WRITE_C8_D16, 0xEC, 0x3C, 0x00,

    WRITE_COMMAND_8, 0xED,
    WRITE_BYTES, 16,
    0xAB, 0x89, 0x76, 0x54,
    0x02, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x20,
    0x45, 0x67, 0x98, 0xBA,

    //-----------VAP & VAN---------------
    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x13,

    WRITE_C8_D8, 0xE5, 0xE4,

    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x00,

    WRITE_COMMAND_8, 0x21,   // 0x20 normal, 0x21 IPS
    WRITE_C8_D8, 0x3A, 0x60, // 0x70 RGB888, 0x60 RGB666, 0x50 RGB565

    //WRITE_COMMAND_8, 0x21,0X00,
    //END_WRITE,
     
    WRITE_COMMAND_8, 0x11, // Sleep Out
    END_WRITE,

    DELAY, 120,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x29, // Display On
    END_WRITE,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x20, // Invert display
    END_WRITE
};

Arduino_DataBus* bus            = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO);


Arduino_ESP32RGBPanel* rgbpanel = new Arduino_ESP32RGBPanel(
    TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK, TFT_R0, TFT_R1, TFT_R2, TFT_R3, TFT_R4, TFT_G0, TFT_G1, TFT_G2, TFT_G3,
    TFT_G4, TFT_G5, TFT_B0, TFT_B1, TFT_B2, TFT_B3, TFT_B4, TFT_HSYNC_POLARITY, TFT_HSYNC_FRONT_PORCH,
    TFT_HSYNC_PULSE_WIDTH, TFT_HSYNC_BACK_PORCH, TFT_VSYNC_POLARITY, TFT_VSYNC_FRONT_PORCH, TFT_VSYNC_PULSE_WIDTH,
    TFT_VSYNC_BACK_PORCH);

Arduino_GFX* Display = new Arduino_RGB_Display(480, 480, rgbpanel, 0 /* rotation */, TFT_AUTO_FLUSH, bus, TFT_RST,
                              st7701_4848S040_init_operations, sizeof(st7701_4848S040_init_operations));

