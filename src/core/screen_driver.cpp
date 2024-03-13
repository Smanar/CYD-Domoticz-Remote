#include "screen_driver.h"
//#include <SPI.h>

#include "../conf/global_config.h"
#include "lvgl.h"

uint32_t LV_EVENT_GET_COMP_CHILD;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_WIDTH * TFT_HEIGHT / 10];

#ifdef TFT_ESPI
    #include <TFT_eSPI.h>
    static TFT_eSPI tft = TFT_eSPI();
#endif

#ifdef LOVYANGFX
    #include <LovyanGFX.hpp>

    // Driver by device
    #ifdef ESP32_E32S3RGB43
    #include "../drivers/esp32-E32S3RGB43.h"
    #endif
    #ifdef esp2432S028R
    #include "../drivers/esp32-2432S028R.h"
    #endif
    #ifdef ESP32_2432S024C
    #include "../drivers/esp32-2432S024C.h"
    #endif
    #ifdef ESP32_8048S043C
    #include "../drivers/esp32-8048S043C.h"
    #endif

    LGFX tft;
    static LGFX_Sprite sprite(&tft);
#endif

#ifdef ARDUINO_GFX

    #ifdef ESP32_4848S040
    #include "../drivers/esp32-4848S040.h"
    #endif
    #ifdef esp2432S028R
    #include "../drivers/esp32-2432S028R(gfx).h"
    #endif

#endif

#ifdef TOUCH_XPT2046
    #define XPT2046_IRQ 36
    #define XPT2046_MOSI 32
    #define XPT2046_MISO 39
    #define XPT2046_CLK 25
    #define XPT2046_CS 33
    #include <XPT2046_Touchscreen.h>
#endif


#ifdef TOUCH_XPT2046
    SPIClass touchscreen_spi = SPIClass(HSPI);
    XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
#endif
#ifdef TOUCH_LOVYAN
    class TS_Point
    {
        public:
            int x, y;
            TS_Point() : x(0), y(0) {}
            TS_Point(int x, int y) : x(x), y(y) {}
    };

    class _TC
    {
    public:
        bool touched();
        bool tirqTouched();
        TS_Point getPoint();
    private:
        uint16_t touchX, touchY;
        TS_Point p;
    };

    bool _TC::touched(void)
    {
        return tft.getTouch( &this->touchX, &this->touchY );
    }

    bool _TC::tirqTouched(void)
    {
        return tft.getTouch( &this->touchX, &this->touchY );
    }

    TS_Point _TC::getPoint(void)
    {
        this->p.x = touchX;
        this->p.y = touchY;
        return this->p;
    }

    _TC touchscreen;
#endif
#ifdef TOUCH_911

//https://github.com/nik-sharky/arduino-goodix/blob/master/examples/GT911_avr_touch/GT911_avr_touch.ino

#include <Wire.h>
#include "Goodix.h"

//#include "touch_driver.h" // base class
//#include "touch_helper.h" // i2c scanner

static Goodix touch = Goodix();

class TS_Point
{
    public:
        int x, y;
        TS_Point() : x(0), y(0) {}
        TS_Point(int x, int y) : x(x), y(y) {}
};

class _TC
{
public:
    _TC();
    bool touched();
    bool tirqTouched();
    TS_Point getPoint();
private:
    uint16_t touchX, touchY;
    TS_Point p;
};

_TC::_TC()
{

    Wire.begin(TOUCH_SDA, TOUCH_SCL, (uint32_t)I2C_TOUCH_FREQUENCY);
    //touch.setHandler(GT911_setXY); // not used
    GTInfo* info;

    if(touch.begin(TOUCH_IRQ, TOUCH_RST, I2C_TOUCH_ADDRESS))
    {
        info = touch.readInfo();
        if(info->xResolution > 0 && info->yResolution > 0) goto found;
    }
    
#if TOUCH_IRQ == -1
    // Probe both addresses if IRQ is not connected
    for(uint8_t i = 0; i < 4; i++)
        if(touch.begin(TOUCH_IRQ, TOUCH_RST, i < 2 ? 0x5d : 0x14)) {
            info = touch.readInfo();
            if(info->xResolution > 0 && info->yResolution > 0) goto found;
        }
#endif

found:
    if(info->xResolution != 0 && info->yResolution != 0) {
        Serial.printf("Driver GT911 started: (%dx%d)\n", info->xResolution, info->yResolution);
        // uint8_t len = touch.fwResolution(480, 272);
    } else {
        Serial.printf("Driver GT911 failed\n");
    }

    // Do it again ?
    Wire.begin(TOUCH_SDA, TOUCH_SCL, (uint32_t)I2C_TOUCH_FREQUENCY);

    Serial.println("Check ACK on addr request on 0x");
    Serial.println(touch.i2cAddr, HEX);

    Wire.beginTransmission(touch.i2cAddr);  
    int error = Wire.endTransmission();
    if (error == 0) {    
        Serial.println(": SUCCESS");   
    } else {
        Serial.print(": ERROR #");
        Serial.println(error);
    }

}


bool _TC::touched(void)
{
    static GTPoint points[5];
    //GTPoint *points


    if(touch.readInput((uint8_t*)&points) > 0)
    {
        this->p.x = points[0].x;
        this->p.y = points[0].y;

        return true;
    }

    return false;
}

bool _TC::tirqTouched(void)
{
    //return tft.getTouch( &this->touchX, &this->touchY );
    return false;
}

TS_Point _TC::getPoint(void)
{
    this->p.x = touchX;
    this->p.y = touchY;
    return this->p;
}

_TC touchscreen;

#endif




bool isScreenInSleep = false;
lv_timer_t *screenSleepTimer;


void touchscreen_calibrate(bool force)
{
    if (global_config.screenCalibrated && !force)
    {
        return;
    }

    tft.fillScreen(TFT_BLACK);
#ifdef ARDUINO_GFX
    //tft.setFont(2);
#else
    tft.setTextFont(2); // Needed on some library
#endif
    tft.setTextSize(2);
    tft.setCursor(20, 140);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);

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
    tft.drawFastHLine(TFT_HEIGHT-20, TFT_WIDTH-10, 20, TFT_WHITE);
    tft.drawFastVLine(TFT_HEIGHT-10, TFT_WIDTH-20, 20, TFT_WHITE);

    while (!touchscreen.touched())
        ;
    delay(50);
    p = touchscreen.getPoint();
    x2 = p.x;
    y2 = p.y;
    tft.drawFastHLine(TFT_HEIGHT-20, TFT_WIDTH-10, 20, TFT_BLACK);
    tft.drawFastVLine(TFT_HEIGHT-10, TFT_WIDTH-20, 20, TFT_BLACK);

    int16_t xDist = TFT_HEIGHT - 40;
    int16_t yDist = TFT_WIDTH - 40;

    global_config.screenCalXMult = (float)xDist / (float)(x2 - x1);
    global_config.screenCalXOffset = 20.0 - ((float)x1 * global_config.screenCalXMult);

    global_config.screenCalYMult = (float)yDist / (float)(y2 - y1);
    global_config.screenCalYOffset = 20.0 - ((float)y1 * global_config.screenCalYMult);

    Serial.printf("Calibration coef: %.1f , %.1f\n", global_config.screenCalXMult,global_config.screenCalYMult);
    Serial.printf("Calibration offset: %.1f , %.1f\n", global_config.screenCalXOffset, global_config.screenCalYOffset);


#if 0
//Tool to test the touchpad, need to be disabled, only for debug purpose
while (true)
{
    p = touchscreen.getPoint();
    p.x = round((p.x * global_config.screenCalXMult) + global_config.screenCalXOffset);
    p.y = round((p.y * global_config.screenCalYMult) + global_config.screenCalYOffset);

    tft.drawFastHLine(p.x,p.y,2,TFT_RED);
}
#endif

    global_config.screenCalibrated = true;
    WriteGlobalConfig();

}

void screen_setBrightness(byte brightness)
{
#ifdef ARDUINO_GFX
    return; // Not supported by this lib
#else
    #ifdef TFT_BL
    analogWrite(TFT_BL, brightness);
    #else
    tft.setBrightness(brightness);
    #endif
#endif
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

#ifdef TFT_ESPI
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
#endif
#ifdef LOVYANGFX
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushPixels((uint16_t *)&color_p->full, w * h, true);
#endif
#ifdef ARDUINO_GFX
    #if (LV_COLOR_16_SWAP != 0)
        tft.draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
    #else
        tft.draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
    #endif
#endif

    tft.endWrite();

    lv_disp_flush_ready(disp);
}


void screen_lv_touchRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    TS_Point p;

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

        p = touchscreen.getPoint();
        p.x = round((p.x * global_config.screenCalXMult) + global_config.screenCalXOffset);
        p.y = round((p.y * global_config.screenCalYMult) + global_config.screenCalYOffset);

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
    tft.invertDisplay(!global_config.invertColors);
}

void screen_setup()
{
#ifdef TOUCH_XPT2046
    touchscreen_spi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreen_spi);
    touchscreen.setRotation(global_config.rotateScreen ? 3 : 1);
#endif

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

    lv_init();
#ifdef ARDUINO_GFX
    tft.begin();
#else
    tft.init();
#endif
#ifdef TFT_ESPI
    tft.setRotation(global_config.rotateScreen ? 3 : 1);
#else
    //tft.setRotation(4);
#endif
    tft.fillScreen(TFT_BLACK);
    set_screen_brightness();
    set_invert_display();

    touchscreen_calibrate(FORCECALIBRATE);

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * TFT_HEIGHT / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_HEIGHT;
    disp_drv.ver_res = TFT_WIDTH;
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