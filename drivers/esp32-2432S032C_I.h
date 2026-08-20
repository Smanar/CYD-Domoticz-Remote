class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
    lgfx::Light_PWM     _light_instance;
    lgfx::Touch_GT911   _touch_instance;
  public:
    LGFX(void)
    {
      {
        auto cfg = _bus_instance.config();

        cfg.spi_host         = HSPI_HOST;
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
//        cfg.memory_width     =    TFT_WIDTH;
//        cfg.memory_height    =    TFT_HEIGHT;
//        cfg.panel_width      =    TFT_WIDTH;
//        cfg.panel_height     =    TFT_HEIGHT;
		cfg.memory_width     =    320;
		cfg.memory_height    =    240;
		cfg.panel_width      =    320;
		cfg.panel_height     =    240;
        cfg.offset_x         =     0;
        cfg.offset_y         =     0;
        cfg.offset_rotation  =     0;
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

        cfg.x_min      = 0;
        cfg.x_max      = 239;
        cfg.y_min      = 0;
        cfg.y_max      = 319;
        cfg.bus_shared = true;
        cfg.offset_rotation = TOUCH_OFFSET_ROTATION;

        cfg.i2c_port = I2C_TOUCH_PORT;
        cfg.i2c_addr = I2C_TOUCH_ADDRESS;
        cfg.pin_sda  = TOUCH_SDA;
        cfg.pin_scl  = TOUCH_SCL;
        cfg.pin_int  = -1;
        cfg.freq     = I2C_TOUCH_FREQUENCY;

        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
      }

      setPanel(&_panel_instance);
    }
};