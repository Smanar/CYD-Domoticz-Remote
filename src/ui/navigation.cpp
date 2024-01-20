#include "lvgl.h"
#include "panels/panel.h"
#include "../core/data_setup.h"
#include "navigation.h"
#include <HTTPClient.h>

static lv_obj_t * tv;
static int actived_panel = 0;
static int ActiveLV = 0;

int GetActivePanel(void)
{
    return actived_panel;
}

void SetActivePanel(int p)
{
    actived_panel = p;
}


void RefreshHomePage(void)
{
    if ((ActiveLV == 1) && (actived_panel < 3))
    {
        navigation_screen(1);
    }

}

void ReturnPreviouspage(void)
{
    Serial.printf("Display LV page: %d\n", ActiveLV);
    navigation_screen(ActiveLV);
}

static void tv_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_VALUE_CHANGED) 
    {
        lv_obj_t *obj = lv_tileview_get_tile_act(tv);
        int tile_x = lv_obj_get_x(obj) / TFT_WIDTH;
        //Serial.printf("TV selected: %d\n", tile_x);

        ActiveLV = tile_x;
        actived_panel = ActiveLV;
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
        case 1:
        case 2:
            // Base
            {
                //Titleview mode
                tv = lv_tileview_create(lv_scr_act());
                lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);
                lv_obj_t * setting = lv_tileview_add_tile(tv, 0, 0, LV_DIR_RIGHT);
                settings_panel_init(setting);
                lv_obj_t * home = lv_tileview_add_tile(tv, 1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
                home_panel_init(home);
                lv_obj_t * info = lv_tileview_add_tile(tv, 2, 0, LV_DIR_LEFT );
                info_panel_init(info);

                lv_obj_set_tile_id(tv, active_panel, 0, LV_ANIM_OFF);

                lv_obj_add_event_cb(tv, tv_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

                ActiveLV = active_panel;
            }
            break;
        case 3:
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
        case 4:
            //
            {
                
            }
            break;
    }

}

void nav_style_setup()
{

}