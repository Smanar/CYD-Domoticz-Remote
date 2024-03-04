#include <HTTPClient.h>

#include "lvgl.h"
#include "navigation.h"
#include "panels/panel.h"
#include "../core/data_setup.h"


static lv_obj_t * tv;
static int actived_panel = 0;
static int master_panel = 0;

extern Device myDevices[];
#if BONUSPAGE > 0
extern Device myDevicesP2[];
#endif

int GetActivePanel(void)
{
    return actived_panel;
}

void SetActivePanel(int p)
{
    actived_panel = p;
}

void ReturnPreviouspage(void)
{
    navigation_screen(master_panel);
}

void RefreshHomePage(void)
{
    if (actived_panel == 1)
    {                           
        navigation_screen(1);
    }
}

static int swipe = 0;



void navigation_screen(unsigned char active_panel)
{
    actived_panel = active_panel;

    //Clear the display
    lv_obj_clean(lv_scr_act());

    lv_obj_t * panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, TFT_HEIGHT, TFT_WIDTH);
    lv_obj_align(panel, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);

    switch (active_panel)
    {
        case 0: // Tools
            master_panel = active_panel;
            tools_panel_init(panel);
            break;
        case 1: // Homepage
            master_panel = active_panel;
            home_panel_init(panel, myDevices);
            break;
        case 2: //Info panel
            master_panel = active_panel;
            info_panel_init(panel);
            break;
        case 3:// Second Device list page
            master_panel = active_panel;
#if BONUSPAGE > 0
            home_panel_init(panel, myDevicesP2);
#endif
            break;
        case 5: // Device panel
            device_panel_init(panel);
            break;
        case 6:// Settings
            settings_panel_init(panel);
            break;
    }

}

void nav_style_setup()
{

}