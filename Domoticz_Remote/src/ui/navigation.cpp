#include "lvgl.h"
#include "panels/panel.h"
#include "../core/data_setup.h"
#include "navigation.h"
#include <HTTPClient.h>

static lv_style_t nav_button_style;
static lv_style_t nav_button_text_style;

int actived_panel = 0;

int GetActivePanel(void)
{
    return actived_panel;
}


void RefreshHomePage(void)
{
    if (actived_panel == 0)
    {
       navigation_screen(0);
    }
}

void navigation_screen(unsigned char active_panel)
{
    actived_panel = active_panel;

    lv_obj_clean(lv_scr_act());
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

    switch (active_panel)
    {
        case 0:
            // Base
            {
                //Titleview mode
                lv_obj_t * tv = lv_tileview_create(lv_scr_act());
                lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);
                lv_obj_t * home = lv_tileview_add_tile(tv, 0, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
                home_panel_init(home);
                lv_obj_t * setting = lv_tileview_add_tile(tv, 1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
                settings_panel_init(setting);
            }
            break;
        case 1:
            //Device panel
            {
                lv_obj_t * panel = lv_obj_create(lv_scr_act());
                lv_obj_set_size(panel, TFT_HEIGHT, TFT_WIDTH);
                lv_obj_align(panel, LV_ALIGN_TOP_RIGHT, 0, 0);
                lv_obj_set_style_border_width(panel, 0, 0);
                lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
                lv_obj_set_style_pad_all(panel, 0, 0);
                device_panel_init(panel);
            }
            break;
    }


}

void nav_style_setup(){
    lv_style_init(&nav_button_style);
    lv_style_set_radius(&nav_button_style, 0);

    lv_style_init(&nav_button_text_style);
    lv_style_set_text_font(&nav_button_text_style, &lv_font_montserrat_10);
}