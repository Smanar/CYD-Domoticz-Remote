
void navigation_screen(unsigned char active_panel, bool dontLoadData = false);
void ReturnPreviouspage(void);
void nav_style_setup();
int GetActiveWidgetPage(void);
int GetActivePanel(void);
void SetActivePanel(int);
void RefreshWidgetsPanel(bool dontLoadData = false);
void RefreshScenePanel(void);
void RefreshDevicePanel(void);
bool screen_ignore_action(void);

int checkAdminRights(const int, const int);
bool isPageProtected(int page);
void password_keyboard_display(lv_obj_t* panel);

static unsigned long screenRestartTime = 0;

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
    SETTING_PANEL,
    PASS_PANEL
};
