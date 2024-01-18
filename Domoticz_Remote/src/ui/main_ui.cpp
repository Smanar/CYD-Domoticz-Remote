#include "main_ui.h"
#include "../core/data_setup.h"
#include "../conf/global_config.h"
#include "../core/screen_driver.h"
#include "lvgl.h"
#include "navigation.h"

lv_style_t style_shadow;


void check_if_screen_needs_to_be_disabled(){

}

static void on_state_change(void * s, lv_msg_t * m)
{
    check_if_screen_needs_to_be_disabled();
}


void main_ui_setup(){
    lv_msg_subscribe(DATA_REMOTE_STATE, on_state_change, NULL);
    on_state_change(NULL, NULL);

    //defaut theme for button
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow,  8);
    lv_style_set_shadow_spread(&style_shadow,  4);
    lv_style_set_shadow_color(&style_shadow,  lv_palette_darken(LV_PALETTE_GREY, 2));
    lv_style_set_bg_color(&style_shadow, lv_palette_darken(LV_PALETTE_GREY, 4));

}
