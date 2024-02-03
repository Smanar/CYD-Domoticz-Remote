#include "screen_driver.h"
#include <SPI.h>

#include "../conf/global_config.h"
#include "lvgl.h"

SPIClass touchscreen_spi = SPIClass(HSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

uint32_t LV_EVENT_GET_COMP_CHILD;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_WIDTH * TFT_HEIGHT / 10];

#ifdef TFT_ESPI
    static TFT_eSPI tft = TFT_eSPI();
#endif

#ifdef LOVYANGFX
//#define LGFX_USE_V1
//https://macsbug.wordpress.com/2022/08/20/web-radio-esp32-2432s028-i2s/



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

//Adapt
//#define LGFX_AUTODETECT
//#include <LovyanGFX.hpp>
//#include <LGFX_TFT_eSPI.hpp>
//static TFT_eSPI tft;               // TFT_eSPI is an alias for LGFX.
//static TFT_eSprite sprite(&tft);   // TFT_eSprite is alias for LGFX_Sprite

bool isScreenInSleep = false;
lv_timer_t *screenSleepTimer;

TS_Point touchscreen_point()
{
    TS_Point p = touchscreen.getPoint();
    p.x = round((p.x * global_config.screenCalXMult) + global_config.screenCalXOffset);
    p.y = round((p.y * global_config.screenCalYMult) + global_config.screenCalYOffset);
    return p;
}

void touchscreen_calibrate(bool force)
{
    if (global_config.screenCalibrated && !force)
        {
            return;
        }

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 140);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.println("Calibrate Screen");

    TS_Point p;
    int16_t x1, y1, x2, y2;

    while (touchscreen.touched())
        ;
    tft.drawFastHLine(0, 10, 20, TFT_WHITE);
    tft.drawFastVLine(10, 0, 20, TFT_WHITE);
    while (!touchscreen.touched())
        ;
    delay(50);
    p = touchscreen.getPoint();
    x1 = p.x;
    y1 = p.y;
    tft.drawFastHLine(0, 10, 20, TFT_BLACK);
    tft.drawFastVLine(10, 0, 20, TFT_BLACK);
    delay(500);

    while (touchscreen.touched())
        ;
    tft.drawFastHLine(300, 230, 20, TFT_WHITE);
    tft.drawFastVLine(310, 220, 20, TFT_WHITE);

    while (!touchscreen.touched())
        ;
    delay(50);
    p = touchscreen.getPoint();
    x2 = p.x;
    y2 = p.y;
    tft.drawFastHLine(300, 230, 20, TFT_BLACK);
    tft.drawFastVLine(310, 220, 20, TFT_BLACK);

    int16_t xDist = 320 - 40;
    int16_t yDist = 240 - 40;

    global_config.screenCalXMult = (float)xDist / (float)(x2 - x1);
    global_config.screenCalXOffset = 20.0 - ((float)x1 * global_config.screenCalXMult);

    global_config.screenCalYMult = (float)yDist / (float)(y2 - y1);
    global_config.screenCalYOffset = 20.0 - ((float)y1 * global_config.screenCalYMult);

    global_config.screenCalibrated = true;
    WriteGlobalConfig();
}

void screen_setBrightness(byte brightness)
{
    analogWrite(TFT_BL, brightness);
}

void set_screen_brightness()
{
    if (global_config.brightness < 32)
        screen_setBrightness(255);
    else
        screen_setBrightness(global_config.brightness);
}

void screen_timer_wake()
{
    lv_timer_reset(screenSleepTimer);
    isScreenInSleep = false;
    set_screen_brightness();

    // Reset cpu freq
    setCpuFrequencyMhz(CPU_FREQ_HIGH);
    Serial.printf("CPU Speed: %d MHz\n", ESP.getCpuFreqMHz());
}

void screen_timer_sleep(lv_timer_t *timer)
{
    screen_setBrightness(0);
    isScreenInSleep = true;

    // Screen is off, no need to make the cpu run fast, the user won't notice ;)
    setCpuFrequencyMhz(CPU_FREQ_LOW);
    Serial.printf("CPU Speed: %d MHz\n", ESP.getCpuFreqMHz());
}

void screen_timer_setup()
{
    screenSleepTimer = lv_timer_create(screen_timer_sleep, global_config.screenTimeout * 1000 * 60, NULL);
    lv_timer_pause(screenSleepTimer);
}

void screen_timer_start()
{
    if (global_config.screenTimeout > 0) lv_timer_resume(screenSleepTimer);
}

void screen_timer_stop()
{
    lv_timer_pause(screenSleepTimer);
}

void screen_timer_period(uint32_t period)
{
    lv_timer_set_period(screenSleepTimer, period);
}

void set_screen_timer_period()
{
    screen_timer_period(global_config.screenTimeout * 1000 * 60);
}


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

void screen_lv_touchRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{

    if (touchscreen.tirqTouched() && touchscreen.touched())
    {
        lv_timer_reset(screenSleepTimer);
        // dont pass first touch after power on
        if (isScreenInSleep)
        {
            screen_timer_wake();
            while (touchscreen.touched())
                ;
            return;
        }

        TS_Point p = touchscreen_point();
        data->state = LV_INDEV_STATE_PR;
        data->point.x = p.x;
        data->point.y = p.y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

void set_color_scheme(){
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(color_defs[global_config.color_scheme].primary_color), lv_palette_main(color_defs[global_config.color_scheme].secondary_color), !global_config.lightMode, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
}

void set_invert_display(){
    tft.invertDisplay(global_config.invertColors);
}

void screen_setup()
{
    touchscreen_spi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreen_spi);
    touchscreen.setRotation(global_config.rotateScreen ? 3 : 1);

    lv_init();

    tft.init();
#ifdef TFT_ESPI
    tft.setRotation(global_config.rotateScreen ? 3 : 1);
#else
    //tft.setRotation(4);
#endif
    tft.fillScreen(TFT_BLACK);
    set_screen_brightness();
    set_invert_display();

    touchscreen_spi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreen_spi);

    touchscreen_calibrate();

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


    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = screen_lv_touchRead;
    lv_indev_drv_register(&indev_drv);

    screen_timer_setup();
    screen_timer_start();

    /*Initialize the graphics library */
    LV_EVENT_GET_COMP_CHILD = lv_event_register_id();
    set_color_scheme();
}