#include <lvgl.h>
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

#if 0

//To test with my device, useless for you
#define GFX_BL 21

Arduino_DataBus *bus = new Arduino_ESP32SPI(2, 15 , 14 , 13 , 12);
Arduino_ILI9341 *gfx = new Arduino_ILI9341(bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */, true );

#else

#define GFX_BL 38 // default backlight pin,

// 9-bit mode SPI
Arduino_DataBus* bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO);

// panel (Hardware) specific
Arduino_ESP32RGBPanel* rgbpanel = new Arduino_ESP32RGBPanel(
    TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK, TFT_R0, TFT_R1, TFT_R2, TFT_R3, TFT_R4, TFT_G0, TFT_G1, TFT_G2, TFT_G3,
    TFT_G4, TFT_G5, TFT_B0, TFT_B1, TFT_B2, TFT_B3, TFT_B4, TFT_HSYNC_POLARITY, TFT_HSYNC_FRONT_PORCH,
    TFT_HSYNC_PULSE_WIDTH, TFT_HSYNC_BACK_PORCH, TFT_VSYNC_POLARITY, TFT_VSYNC_FRONT_PORCH, TFT_VSYNC_PULSE_WIDTH,
    TFT_VSYNC_BACK_PORCH);

// panel parameters & setup
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
    bus, GFX_NOT_DEFINED /* RST */, st7701_4848S040_init_operations, sizeof(st7701_4848S040_init_operations));
#endif

#ifndef LV_VDB_SIZE
#if defined(ARDUINO_ARCH_ESP8266)
#  define LV_VDB_SIZE    (8 * 1024U)   // Minimum 8 Kb
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#  define LV_VDB_SIZE    (16 * 1024U)  // 16kB draw buffer
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#  define LV_VDB_SIZE    (48 * 1024U)  // 16kB draw buffer
#elif defined(ARDUINO_ARCH_ESP32)
#  define LV_VDB_SIZE    (32 * 1024U)  // 32kB draw buffer
#else
#  define LV_VDB_SIZE    (128 * 1024U) // native app
#endif
#endif // LV_VDB_SIZE

// Virtual Device buffer, can be smaller than screen size.
// Lvgl will use this memory range to draw widgets to be refreshed: if a widget does not fit into the provided range it will be drawn and flushed in more than one go.
// To maximize performances one would use the entire screen size (in bytes, so pixels divided by 8, so W*H/8)
const size_t VDBsize = LV_VDB_SIZE / sizeof(lv_color_t);
//const size_t VDBsize = 480*480;

#define OLDCODE 1

#if OLDCODE
static lv_color_t *disp_draw_buf;
#else
static lv_color_t disp_draw_buf[BUFFSIZE];
#endif


/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{

  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}


void setup()
{
  Serial.begin(115200);
  Serial.println("Arduino_GFX LVGL Hello World example");

  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
   pinMode(GFX_BL, OUTPUT);
   digitalWrite(GFX_BL, HIGH);
#endif

  lv_init();

  screenWidth = gfx->width();
  screenHeight = gfx->height();

#ifdef ESP32
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * VDBsize, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!disp_draw_buf)
  {
    // remove MALLOC_CAP_INTERNAL flag try again
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * VDBsize, MALLOC_CAP_8BIT);
  }
#else
    disp_draw_buf = (lv_color_t*)malloc(sizeof(lv_color_t) * VDBsize);
#endif 


  if (!disp_draw_buf)
  {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  }
  else
  {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, VDBsize);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;

    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

    /* Create simple label */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_recolor(label, true); 
    lv_obj_set_style_text_color(label, LV_COLOR_MAKE(0xFF, 0x00, 0x00), 0);
    //lv_label_set_text(label, "Hello Arduino! (V" GFX_STR(LVGL_VERSION_MAJOR) "." GFX_STR(LVGL_VERSION_MINOR) "." GFX_STR(LVGL_VERSION_PATCH) ")");
    lv_label_set_text(label, "#0000ff Blue# #00ff00 green# #ff0000 RED# Text ");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    Serial.println("Setup done");
  }
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}