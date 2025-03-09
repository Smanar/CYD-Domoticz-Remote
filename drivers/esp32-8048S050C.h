#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

class LGFX: public lgfx::LGFX_Device {
    public:

    lgfx::Bus_RGB _bus_instance;
    lgfx::Panel_RGB _panel_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_GT911 _touch_instance;

    LGFX(void) {
        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;
            cfg.pin_d0 = GPIO_NUM_8; // B0
            cfg.pin_d1 = GPIO_NUM_3; // B1
            cfg.pin_d2 = GPIO_NUM_46; // B2
            cfg.pin_d3 = GPIO_NUM_9; // B3
            cfg.pin_d4 = GPIO_NUM_1; // B4
            cfg.pin_d5 = GPIO_NUM_5; // G0
            cfg.pin_d6 = GPIO_NUM_6; // G1
            cfg.pin_d7 = GPIO_NUM_7; // G2
            cfg.pin_d8 = GPIO_NUM_15; // G3
            cfg.pin_d9 = GPIO_NUM_16; // G4
            cfg.pin_d10 = GPIO_NUM_4; // G5
            cfg.pin_d11 = GPIO_NUM_45; // R0
            cfg.pin_d12 = GPIO_NUM_48; // R1
            cfg.pin_d13 = GPIO_NUM_47; // R2
            cfg.pin_d14 = GPIO_NUM_21; // R3
            cfg.pin_d15 = GPIO_NUM_14; // R4

            cfg.pin_henable = GPIO_NUM_40;
            cfg.pin_vsync = GPIO_NUM_41;
            cfg.pin_hsync = GPIO_NUM_39;
            cfg.pin_pclk = 42;
            cfg.freq_write = 16000000;

            cfg.hsync_polarity = 0;
            cfg.hsync_front_porch = 8;
            cfg.hsync_pulse_width = 4;
            cfg.hsync_back_porch = 8;
            cfg.vsync_polarity = 0;
            cfg.vsync_front_porch = 8;
            cfg.vsync_pulse_width = 4;
            cfg.vsync_back_porch = 8;
            cfg.pclk_idle_high = 1;
            _bus_instance.config(cfg);

        }
        _panel_instance.setBus( &_bus_instance);

        {
          auto cfg = _panel_instance.config();
          cfg.memory_width = 800;
          cfg.memory_height = 480;
          cfg.panel_width = 800;
          cfg.panel_height = 480;
          cfg.offset_x = 1000;
          cfg.offset_y = 2000;

          _panel_instance.config(cfg);
        }

        {
          auto cfg = _panel_instance.config_detail();
          cfg.use_psram = 1;

          _panel_instance.config_detail(cfg);
        }

        {
          auto cfg = _light_instance.config();
          cfg.pin_bl = 2;
          //cfg.freq = 44100;
          //cfg.pwm_channel = 7;

          _light_instance.config(cfg);
        }

        _panel_instance.light( &_light_instance);

        {
          auto cfg = _touch_instance.config();
          cfg.x_min = 0;
          cfg.x_max = 480;
          cfg.y_min = 0;
          cfg.y_max = 800;
          cfg.pin_int = -1;
          cfg.bus_shared = false;
          cfg.offset_rotation = 0;
          // I2C
          cfg.i2c_port = I2C_NUM_0; // I2C(0 = SPI or 1 = Wire)
          cfg.pin_sda = 19; // SDA
          cfg.pin_scl = 20; // SCL
          cfg.pin_rst = 38; // RST
          cfg.freq = 100000; // I2C 8000 de incercat
          cfg.i2c_addr = 0x5D; // I2C 0x5D or 0x14
          _touch_instance.config(cfg);
          _panel_instance.setTouch( &_touch_instance);
        }

        setPanel( &_panel_instance);
      }
    };
