
void navigation_screen(unsigned char active_panel);
void ReturnPreviouspage(void);
void nav_style_setup();
int GetActiveWidgetPage(void);
int GetActivePanel(void);
void SetActivePanel(int);
void RefreshWidgetsPanel(void);
void RefreshScenePanel(void);
void RefreshDevicePanel(void);

typedef void (checkAdminCallback_t)(const int);

void checkAdminRights(const int pagePtr, checkAdminCallback_t allowedCallback, checkAdminCallback_t refusedCallback);
bool isPageProtected(int page);

#define DATA_REMOTE_STATE 1
#define DATA_REMOTE_DATA 2
#define DATA_REMOTE_PRESET 3

enum {
    TOOL_PANEL,
    HOMEPAGE_PANEL,
    LAST_PAGE_PANEL = (HOMEPAGE_PANEL + PAGES - 1),
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
