#include "lvgl.h"

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

void settings_panel_init(lv_obj_t* panel);
void home_panel_init(lv_obj_t* panel);
void info_panel_init(lv_obj_t* panel);
void power_panel_init(lv_obj_t* panel);
void progress_panel_init(lv_obj_t* panel);
void device_panel_init(lv_obj_t* panel);

void Select_device(int device);
lv_color_t Getcolor(int type);
const lv_img_dsc_t *Geticon(int type);
