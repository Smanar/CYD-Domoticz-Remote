#include "lvgl.h"
#include "navigation.h"
#include "panels/panel.h"
#include "../core/data_setup.h"
#include "../core/ip_engine.h"

static lv_obj_t * tv;
static int actived_panel = 0;
static int master_panel = 0;

extern Device myDevices[];

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

void RefreshDevicePanel(void)
{
    if (actived_panel == DEVICE_PANEL)
    {                           
        navigation_screen(DEVICE_PANEL);
    }
}

#ifndef NO_GROUP_PAGE
void RefreshScenePage(void)
{
    if (actived_panel == GROUP_PANEL)
    {                           
        navigation_screen(GROUP_PANEL);
    }
}
#endif



void navigation_screen(unsigned char active_panel)
{
    actived_panel = active_panel;

    //Clear the display
    lv_obj_clean(lv_scr_act());

    lv_obj_t * panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_align(panel, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    //lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

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
#ifndef NO_GROUP_PAGE
        case GROUP_PANEL: //Group/Scene panel
            master_panel = active_panel;
            group_panel_init(panel);
            break;
#endif
#ifndef NO_INFO_PAGE
        case INFO_PANEL: //Info panel
            master_panel = active_panel;
            info_panel_init(panel);
            break;
#endif
        case DEVICE_PANEL: // Device panel
            device_panel_init(panel);
            break;
        case SETTING_PANEL:// Settings
            settings_panel_init(panel);
            break;

        default:
            settings_panel_init(panel);
            break;
    }

}

void nav_style_setup()
{

}