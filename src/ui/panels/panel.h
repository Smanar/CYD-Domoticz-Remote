#include "lvgl.h"

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

void settings_panel_init(lv_obj_t* panel);
void home_panel_init(lv_obj_t* panel);
void info_panel_init(lv_obj_t* panel);
void device_panel_init(lv_obj_t* panel);
void tools_panel_init(lv_obj_t* panel);

void Init_Info_Style(void);

void Select_deviceHP(int device);
void Select_deviceIDX(int idx);

lv_color_t Getcolor(int type);
const lv_img_dsc_t *Geticon(int type);
