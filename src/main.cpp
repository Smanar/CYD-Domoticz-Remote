#include <SPI.h>

#include "lvgl.h"
uint32_t LV_EVENT_GET_COMP_CHILD;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_WIDTH * TFT_HEIGHT / 10];

#ifdef TFT_ESPI
    #include <TFT_eSPI.h>
    static TFT_eSPI tft = TFT_eSPI();
#endif

#ifdef LOVYANGFX
    //#define LGFX_AUTODETECT
    #define LGFX_USE_V1
    #include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
    lgfx::Light_PWM     _light_instance;
    lgfx::Touch_XPT2046 _touch_instance;
  public:
    LGFX(void)
    {
      {
        auto cfg = _bus_instance.config();

        cfg.spi_host         = HSPI_HOST;// VSPI_HOST or HSPI_HOST (SPI2_HOST)
        cfg.spi_mode         = 0;
        cfg.freq_write       = SPI_FREQUENCY;
        cfg.freq_read        = SPI_READ_FREQUENCY;
        cfg.spi_3wire        = false;
        cfg.use_lock         = true;
        cfg.dma_channel      = 1;
        cfg.pin_sclk         = TFT_SCLK;
        cfg.pin_mosi         = TFT_MOSI;
        cfg.pin_miso         = TFT_MISO;
        cfg.pin_dc           = TFT_DC;

        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
      }

      {
        auto cfg = _panel_instance.config();

        cfg.pin_cs           =    TFT_CS;
        cfg.pin_rst          =    TFT_RST;
        cfg.pin_busy         =    TFT_BUSY;
        cfg.memory_width     =    TFT_WIDTH;
        cfg.memory_height    =    TFT_HEIGHT;
        cfg.panel_width      =    TFT_WIDTH;
        cfg.panel_height     =    TFT_HEIGHT;
        cfg.offset_x         =     0;
        cfg.offset_y         =     0;
        cfg.offset_rotation  =     TFT_ROTATION;
        cfg.dummy_read_pixel =     8;
        cfg.dummy_read_bits  =     1;
        cfg.readable         = true;
        cfg.invert           = TFT_INVERSION_ON;
        cfg.rgb_order        = !TFT_RGB_ORDER;
        cfg.dlen_16bit       = false;
        cfg.bus_shared       = false;

        _panel_instance.config(cfg);
      }

      {
        auto cfg = _light_instance.config();

        cfg.pin_bl = TFT_BL;
        cfg.invert = false;
        cfg.freq   = 44100;           
        cfg.pwm_channel = 7;

        _light_instance.config(cfg);
        _panel_instance.setLight(&_light_instance);
      }

      {
        auto cfg = _touch_instance.config();

        cfg.x_min      =  300;
        cfg.x_max      = 3900;
        cfg.y_min      = 3700;
        cfg.y_max      =  200;
        cfg.pin_int    = -1;
        cfg.bus_shared = false;
        cfg.offset_rotation = 0;

        cfg.spi_host = HSPI_HOST;
        cfg.freq = 1000000;
        cfg.pin_sclk = 25;
        cfg.pin_mosi = 32;
        cfg.pin_miso = 39;
        cfg.pin_cs   = 33;

        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
      }

      setPanel(&_panel_instance);
    }
};

    LGFX tft;
    static LGFX_Sprite sprite(&tft);

#endif



void screen_lv_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
#ifdef TFT_ESPI
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
#else
    tft.pushPixels((uint16_t *)&color_p->full, w * h, true);
#endif
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void set_invert_display(){
    tft.invertDisplay(true);
}


void setup() {
    Serial.begin(115200);
    Serial.println("Starting application");
    
   lv_init();

   tft.init();

#ifdef TFT_ESPI
    tft.setRotation(1);
#endif

    tft.fillScreen(TFT_WHITE);
    //screen_setBrightness(100);
    //set_invert_display();


    lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * TFT_HEIGHT / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
#ifdef TFT_ESPI
    disp_drv.hor_res = TFT_HEIGHT;
    disp_drv.ver_res = TFT_WIDTH;
#else
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
#endif
    disp_drv.flush_cb = screen_lv_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);


}

void loop()
{
  tft.fillScreen(random(0xFFFF));
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
  tft.println("Hello World!");

  tft.setTextColor(TFT_GREEN,TFT_BLACK);
  tft.println("Groop");
  tft.println("I implore thee,");
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee in the gobberwarts with my blurglecruncheon, see if I don't!");


  delay(1000);
}

