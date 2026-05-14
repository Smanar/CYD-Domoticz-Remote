#include "lvgl.h"
#include "navigation.h"
#include "panels/panel.h"
#include "../core/data_setup.h"
#include "../core/ip_engine.h"
#include "../conf/global_config.h"

static lv_obj_t * tv;
static int actived_panel = 0;
static int master_panel = 0;

extern Device myDevices[];

int GetActiveDevicePage(void)
{
    return actived_panel - HOMEPAGE_PANEL;
}

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
    if (actived_panel >= HOMEPAGE_PANEL && actived_panel <= LAST_PAGE_PANEL)
    {                           
        navigation_screen(actived_panel);
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

    if (active_panel == TOOL_PANEL) {                               // Tools
        master_panel = active_panel;
        tools_panel_init(panel);
    } else if (active_panel >= HOMEPAGE_PANEL && active_panel <= LAST_PAGE_PANEL) {  // Homepage
        master_panel = active_panel;
        home_panel_init(panel, myDevices);
    #ifndef NO_GROUP_PAGE
    } else if (active_panel == GROUP_PANEL) {                       //Group/Scene panel
        master_panel = active_panel;
        group_panel_init(panel);
    #endif
    #ifndef NO_INFO_PAGE
    } else if (active_panel == INFO_PANEL) {                        //Info panel
        master_panel = active_panel;
        info_panel_init(panel);
    #endif
    } else if (active_panel == DEVICE_PANEL) {                      // Device panel    
        device_panel_init(panel);
    } else if (active_panel == SETTING_PANEL) {                     // Settings
        settings_panel_init(panel);
    }
}

void nav_style_setup()
{

}

// Check if a given page is protected
bool isPageProtected(int page) {
    if (page == TOOL_PANEL) {                                       // Tools
        return global_config.protectTool;
    } else if (page >= HOMEPAGE_PANEL && page <= LAST_PAGE_PANEL) { // Homepage
        return global_pages[page - HOMEPAGE_PANEL].isProtected;
    #ifndef NO_GROUP_PAGE
    } else if (page == GROUP_PANEL) {                               //Group/Scene panel
        return global_config.protectGroup;
    #endif
    #ifndef NO_INFO_PAGE
    } else if (page == INFO_PANEL) {                                //Info panel
        return global_config.protectInfo;
    #endif
    } else if (page == SETTING_PANEL) {                             // Settings
        return global_config.protectSetting;
    }
    return false;                                                   // By default (including DEVICE_PANEL)
}