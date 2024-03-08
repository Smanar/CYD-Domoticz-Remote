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
    if (actived_panel == HOMEPAGE_PANEL)
    {                           
        navigation_screen(HOMEPAGE_PANEL);
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
        case TOOL_PANEL: // Tools
            master_panel = active_panel;
            tools_panel_init(panel);
            break;
        case HOMEPAGE_PANEL: // Homepage
            master_panel = active_panel;
            home_panel_init(panel, myDevices);
            break;
#ifndef NO_INFO_PAGE
        case INFO_PANEL: //Info panel
            master_panel = active_panel;
            info_panel_init(panel);
            break;
#endif
#if BONUSPAGE > 0
        case BONUSPAGE_PANEL1:// Second Device list page
            master_panel = active_panel;
            home_panel_init(panel, myDevicesP2);
            break;
#endif
#if BONUSPAGE > 1
        case BONUSPAGE_PANEL2:// third  Device list page
            master_panel = active_panel;
            home_panel_init(panel, myDevicesP2,1);
            break;
#endif
#if BONUSPAGE > 2
        case BONUSPAGE_PANEL3:// fourth Device list page
            master_panel = active_panel;
            home_panel_init(panel, myDevicesP2,2);
            break;
#endif
        case DEVICE_PANEL: // Device panel
            device_panel_init(panel);
            break;
        case SETTING_PANEL:// Settings
            settings_panel_init(panel);
            break;
    }

}

void nav_style_setup()
{

}