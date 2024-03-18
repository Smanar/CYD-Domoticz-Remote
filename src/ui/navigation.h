void navigation_screen(unsigned char active_panel);
void ReturnPreviouspage(void);
void nav_style_setup();
int GetActivePanel(void);
void SetActivePanel(int);
void RefreshHomePage(void);

#define DATA_REMOTE_STATE 1
#define DATA_REMOTE_DATA 2
#define DATA_REMOTE_PRESET 3

enum {
    TOOL_PANEL,
    HOMEPAGE_PANEL,
#if BONUSPAGE > 0
    BONUSPAGE_PANEL1,
#endif
#if BONUSPAGE > 1
    BONUSPAGE_PANEL2,
#endif
#if BONUSPAGE > 2
    BONUSPAGE_PANEL3,
#endif
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