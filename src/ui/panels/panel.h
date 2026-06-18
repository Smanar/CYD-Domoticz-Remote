#include <Esp.h>
#include "lvgl.h"
#include "../../core/data_setup.h"

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

void settings_panel_init(lv_obj_t* panel);
void widget_panel_init(lv_obj_t* panel);
void info_panel_init(lv_obj_t* panel);
void group_panel_init(lv_obj_t* panel);
void device_panel_init(lv_obj_t* panel);
void tools_panel_init(lv_obj_t* panel);

void Init_Info_Style(void);

void Select_deviceMemorised(void * device);
void Select_deviceIDX(int idx);
int GetSelectedDeviceIdx(void);

lv_color_t Getcolor(int type);
const lv_img_dsc_t *Geticon(int type);

//Define font.
// To make new one use :
// - Montserrat police https://fonts.google.com/specimen/Montserrat
// - Editor https://lvgl.io/tools/fontconverter
// With setting bpp = 4 , range 32-126, and with special char €°µ¥£²³
// Montserrat-Regular.ttf and Montserrat-Bold.ttf

#if DEVICE_SIZE == 1

    #if FONT_TO_USE == 2
        // Custom
        const lv_font_t small_font = Montserrat_10_user;
        const lv_font_t medium_font = Montserrat_12_user;
        const lv_font_t big_font_bold = Montserrat_Bold_14_user;
    #elif FONT_TO_USE == 3
        //French
        const lv_font_t small_font = Montserrat_10_custom;
        const lv_font_t medium_font = Montserrat_12_custom;
        const lv_font_t big_font_bold = Montserrat_Bold_14_custom;
    #else
        // Native
        const lv_font_t small_font = lv_font_montserrat_10;
        //Custom
        const lv_font_t medium_font = Montserrat_12;
        const lv_font_t big_font_bold = Montserrat_Bold_14;
    #endif


#else

    #if FONT_TO_USE == 2
        //Custom
        const lv_font_t small_font = Montserrat_14_user;
        const lv_font_t medium_font = Montserrat_16_user;
        const lv_font_t big_font_bold = Montserrat_Bold_18_user;
    #elif FONT_TO_USE == 3
        //French
        const lv_font_t small_font = Montserrat_14_custom;
        const lv_font_t medium_font = Montserrat_16_custom;
        const lv_font_t big_font_bold = Montserrat_Bold_18_custom;
    #else
        // Native
        const lv_font_t small_font = lv_font_montserrat_14;
        //Custom
        const lv_font_t medium_font = Montserrat_16;
        const lv_font_t big_font_bold = Montserrat_Bold_18;
    #endif

#endif

