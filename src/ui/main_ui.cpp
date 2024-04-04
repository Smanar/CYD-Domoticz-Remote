#include "main_ui.h"
#include "../core/data_setup.h"
#include "../conf/global_config.h"
#include "../core/screen_driver.h"
#include "lvgl.h"
#include "navigation.h"

lv_style_t style_shadow;
lv_color_t background;

void check_if_screen_needs_to_be_disabled(){

}

static void on_state_change(void * s, lv_msg_t * m)
{
    check_if_screen_needs_to_be_disabled();
}


void main_ui_setup()
{
    lv_msg_subscribe(DATA_REMOTE_STATE, on_state_change, NULL);
    on_state_change(NULL, NULL);

    //Style when pressed
    //lv_style_init(&style_btn_red);
    //lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
    //lv_style_set_bg_grad_color(&style_btn_red, lv_palette_lighten(LV_PALETTE_RED, 3));

    //Style for buttons
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow,  8);
    lv_style_set_shadow_spread(&style_shadow,  4);
    ResetColorTheme();

}

void ResetColorTheme(void)
{
    if (global_config.lightMode)
    {
        background = lv_color_hex(0xffffff);
        lv_style_set_shadow_color(&style_shadow,  lv_palette_lighten(LV_PALETTE_GREY, 1));
    }
    else
    {
        background = lv_palette_darken(LV_PALETTE_GREY, 4);
        lv_style_set_shadow_color(&style_shadow,  lv_palette_darken(LV_PALETTE_GREY, 2));
    }
    lv_style_set_bg_color(&style_shadow, background);
}