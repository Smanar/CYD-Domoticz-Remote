
void navigation_screen(unsigned char active_panel);
void ReturnPreviouspage(void);
void nav_style_setup();
int GetActivePanel(void);
void SetActivePanel(int);
void RefreshHomePage(void);
void RefreshScenePage(void);
void RefreshDevicePanel(void);

#define DATA_REMOTE_STATE 1
#define DATA_REMOTE_DATA 2
#define DATA_REMOTE_PRESET 3

enum {
    TOOL_PANEL,
    HOMEPAGE_PANEL,
#ifndef NO_GROUP_PAGE
    GROUP_PANEL,
#endif
#ifndef NO_INFO_PAGE
    INFO_PANEL,
#endif
    MAX_PANEL_SCROLL,
    DEVICE_PANEL,
    SETTING_PANEL
};