class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
    lgfx::Light_PWM     _light_instance;
    lgfx::Touch_FT5x06 _touch_instance;
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
        cfg.dma_channel      = 1; // Better to use 0 ?
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
        cfg.rgb_order        = TFT_RGB_ORDER;
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

        cfg.x_min      = 0;              // The minimum X value obtained from the touchscreen (raw value).
        cfg.x_max      = TFT_HEIGHT - 1;  // The maximum X value obtained from the touchscreen (raw value).
        cfg.y_min      = 0;              // The minimum Y value obtained from the touchscreen (raw value).
        cfg.y_max      = TFT_WIDTH - 1; // The maximum Y value obtained from the touchscreen (raw value).
        cfg.bus_shared = true;           // If using a common bus with the screen, set it to true.
        cfg.offset_rotation = TOUCH_OFFSET_ROTATION; // Adjustment when the display orientation does not match the touch orientation. Set in values from 0 to 7.

        cfg.i2c_port = I2C_TOUCH_PORT;
        cfg.i2c_addr = I2C_TOUCH_ADDRESS;
        cfg.pin_sda  = TOUCH_SDA; 
        cfg.pin_scl  = TOUCH_SCL;
        cfg.pin_int  = TOUCH_IRQ;
        cfg.freq     = I2C_TOUCH_FREQUENCY;

        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
      }

      setPanel(&_panel_instance);
    }
};