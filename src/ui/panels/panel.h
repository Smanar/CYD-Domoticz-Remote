#include <Esp.h>
#include "lvgl.h"
#include "../../core/data_setup.h"

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

void settings_panel_init(lv_obj_t* panel);
void home_panel_init(lv_obj_t* panel, Device d[], short page = 0);
void info_panel_init(lv_obj_t* panel);
void group_panel_init(lv_obj_t* panel);
void device_panel_init(lv_obj_t* panel);
void tools_panel_init(lv_obj_t* panel);

void Init_Info_Style(void);

void Select_deviceMemorised(void * device);
void Select_deviceIDX(int idx);

lv_color_t Getcolor(int type);
const lv_img_dsc_t *Geticon(int type);

//Define font.
// To make new one use :
// - Montserrat police https://fonts.google.com/specimen/Montserrat
// - Editor https://lvgl.io/tools/fontconverter
// With setting bpp = 4 , range 32-126, and with sepcial char €°µ¥£²³
// Montserrat-Regular.ttf and Montserrat-Bold.ttf

#if DEVICE_SIZE == 1
    LV_FONT_DECLARE(Montserrat_Bold_14)
    const lv_font_t font1 = Montserrat_Bold_14;
    #ifdef CUSTOM_FONT
        LV_FONT_DECLARE(Montserrat_12)
        const lv_font_t font3 = Montserrat_12;
        LV_FONT_DECLARE(Montserrat_10)
        const lv_font_t font2 = Montserrat_10;
    #else
        const lv_font_t font3 = lv_font_montserrat_12;
        const lv_font_t font2 = lv_font_montserrat_10;
    #endif
#else
    LV_FONT_DECLARE(Montserrat_Bold_18)
    const lv_font_t font1 = Montserrat_Bold_18;
    #ifdef CUSTOM_FONT
        LV_FONT_DECLARE(Montserrat_16)
        onst lv_font_t font3 = Montserrat_16;
        LV_FONT_DECLARE(Montserrat_14)
        const lv_font_t font2 = Montserrat_14;
    #else
        const lv_font_t font3 = lv_font_montserrat_16;
        const lv_font_t font2 = lv_font_montserrat_14;
    #endif
#endif

