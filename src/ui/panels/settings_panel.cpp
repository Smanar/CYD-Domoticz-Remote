#include "lvgl.h"
#include "panel.h"
#include "../../core/screen_driver.h"
#include "../../conf/global_config.h"
#include "../main_ui.h"
#include "../../ui/navigation.h"

void Init_data(void);

static int GetIntTok(const char * str, int t, const char c)
{
    int i, str_length = strlen(str);
    int v = 0, tot = 0;
 
    for (i = 0; i<str_length; i++) {
        if ((str[i] == c) || (str[i] == '\0'))
        {
            tot +=1;
            if (tot > t) break;
        }
        else if (tot == t)
        {
            v = v * 10 + (str[i]-48);
        }
    }
    return v;
}

static void invert_color_switch(lv_event_t * e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.invertColors = checked;
    WriteGlobalConfig();
    set_invert_display();
}

static void reset_calibration_click(lv_event_t * e){
    global_config.screenCalibrated = false;
    WriteGlobalConfig();
    ESP.restart();
}

static void reset_wifi_click(lv_event_t * e){
    global_config.wifiConfigured = false;
    global_config.ipConfigured = false;
    WriteGlobalConfig();
    ESP.restart();
}

static void exit_click(lv_event_t * e){
    navigation_screen(0);
}

static void light_mode_switch(lv_event_t * e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.lightMode = checked;
    WriteGlobalConfig();
    set_color_scheme();
}

static void theme_dropdown(lv_event_t * e){
    const lv_obj_t * dropdown = lv_event_get_target(e);
    auto selected = lv_dropdown_get_selected(dropdown);
    global_config.color_scheme = selected;
    set_color_scheme();
    WriteGlobalConfig();
}

const char* brightness_options = "100%\n75%\n50%\n25%";
const char  brightness_options_values[] = { 255, 192, 128, 64 };

static void brightness_dropdown(lv_event_t * e){
    const lv_obj_t * dropdown = lv_event_get_target(e);
    auto selected = lv_dropdown_get_selected(dropdown);
    global_config.brightness = brightness_options_values[selected];
    set_screen_brightness();
    WriteGlobalConfig();
}

const char* wake_timeout_options = "Disabled\n1m\n2m\n5m\n10m\n15m\n30m\n1h\n2h\n4h";
const char  wake_timeout_options_values[] = { 0, 1, 2, 5, 10, 15, 30, 60, 120, 240 };

static void wake_timeout_dropdown(lv_event_t * e){
    const lv_obj_t * dropdown = lv_event_get_target(e);
    auto selected = lv_dropdown_get_selected(dropdown);
    global_config.screenTimeout = wake_timeout_options_values[selected];
    set_screen_timer_period();
    WriteGlobalConfig();
}

static void rotate_screen_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.rotateScreen = checked;
    global_config.screenCalibrated = false;
    WriteGlobalConfig();
    ESP.restart();
}

static void not_used_yet_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.notused = checked;
    check_if_screen_needs_to_be_disabled();
    WriteGlobalConfig();
}

static lv_obj_t* kb;
static void edit_device_list_switch(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * display = (lv_obj_t *)lv_event_get_user_data(e);

    if (code == LV_EVENT_FOCUSED)
    {
        if (kb == NULL) {
            kb = lv_keyboard_create(lv_scr_act());
            lv_obj_set_style_max_height(kb, LV_HOR_RES * 2 / 3, 0);
        }

        lv_obj_update_layout(display);   /*Be sure the sizes are recalculated*/
        lv_obj_set_height(display, LV_VER_RES - lv_obj_get_height(kb));
        lv_indev_wait_release(lv_indev_get_act());
        lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);

        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        //lv_obj_move_foreground(kb); // Show the keyboard

        lv_keyboard_set_textarea(kb, ta);
    }
    else if (code == LV_EVENT_DEFOCUSED)
    {
        lv_obj_set_height(display, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    else if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        //lv_obj_move_background(kb); // Hide the keyboard
        lv_obj_set_height(display, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);   /*To forget the last clicked object to make it focusable again*/
        const char * str = lv_textarea_get_text(ta);

        for (int i=0; i<TOTAL_ICONX*TOTAL_ICONY; i++)
        {
            global_config.ListDevices[i] = GetIntTok(lv_textarea_get_text(ta), i,';');
        }
        WriteGlobalConfig();
        Init_data();
        navigation_screen(1);
 
    }
 }

int y_offset = 0;
const int y_element_size = 50;
const int y_seperator_size = 1;
const int y_seperator_x_padding = 50;
const int panel_width = TFT_HEIGHT - 40;
const int y_element_x_padding = 30;
const static lv_point_t line_points[] = { {0, 0}, {panel_width - y_seperator_x_padding, 0} };

void create_settings_widget(const char* label_text, lv_obj_t* object, lv_obj_t* root_panel){
    lv_obj_t * panel = lv_obj_create(root_panel);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, y_offset);
    lv_obj_set_size(panel, panel_width - y_element_x_padding, y_element_size);

    lv_obj_t * line = lv_line_create(panel);
    lv_line_set_points(line, line_points, 2);
    lv_obj_set_style_line_width(line, y_seperator_size, 0);
    lv_obj_set_style_line_color(line, lv_color_hex(0xAAAAAA), 0);
    lv_obj_align(line, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t * label = lv_label_create(panel);
    lv_label_set_text(label, label_text);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_set_parent(object, panel);
    lv_obj_align(object, LV_ALIGN_RIGHT_MID, 0, 0);
    y_offset += y_element_size;
}

void settings_panel_init(lv_obj_t* panel)
{
    y_offset = 0;
    kb = nullptr;

    lv_obj_add_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * btn = lv_btn_create(panel);
    lv_obj_add_event_cb(btn, reset_wifi_click, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text_static(label, "Restart");
    lv_obj_center(label);
    create_settings_widget("Configure Network", btn, panel);

    btn = lv_btn_create(panel);
    lv_obj_add_event_cb(btn, reset_calibration_click, LV_EVENT_CLICKED, NULL);
    label = lv_label_create(btn);
    lv_label_set_text_static(label, "Restart");
    lv_obj_center(label);
    create_settings_widget("Calibrate Touch", btn, panel);

    lv_obj_t * toggle = lv_switch_create(panel);
    lv_obj_add_event_cb(toggle, invert_color_switch, LV_EVENT_VALUE_CHANGED, NULL);
    if (global_config.invertColors)
        lv_obj_add_state(toggle, LV_STATE_CHECKED);
    create_settings_widget("Invert Colors", toggle, panel);

    toggle = lv_switch_create(panel);
    lv_obj_add_event_cb(toggle, light_mode_switch, LV_EVENT_VALUE_CHANGED, NULL);
    if (global_config.lightMode)
        lv_obj_add_state(toggle, LV_STATE_CHECKED);
    create_settings_widget("Light Mode", toggle, panel);

    lv_obj_t * dropdown = lv_dropdown_create(panel);
    lv_dropdown_set_options(dropdown, "Blue\nGreen\nGrey\nYellow\nOrange\nRed\nPurple");
    lv_dropdown_set_selected(dropdown, global_config.color_scheme);
    lv_obj_add_event_cb(dropdown, theme_dropdown, LV_EVENT_VALUE_CHANGED, NULL);
    create_settings_widget("Theme", dropdown, panel);

    dropdown = lv_dropdown_create(panel);
    lv_dropdown_set_options(dropdown, brightness_options);
    lv_obj_add_event_cb(dropdown, brightness_dropdown, LV_EVENT_VALUE_CHANGED, NULL);
    for (int i = 0; i < SIZEOF(brightness_options_values); i++){
        if (brightness_options_values[i] == global_config.brightness){
            lv_dropdown_set_selected(dropdown, i);
            break;
        }
    }
    create_settings_widget("Brightness", dropdown, panel);

    dropdown = lv_dropdown_create(panel);
    lv_dropdown_set_options(dropdown, wake_timeout_options);
    lv_obj_add_event_cb(dropdown, wake_timeout_dropdown, LV_EVENT_VALUE_CHANGED, NULL);
    for (int i = 0; i < SIZEOF(wake_timeout_options_values); i++){
        if (wake_timeout_options_values[i] == global_config.screenTimeout){
            lv_dropdown_set_selected(dropdown, i);
            break;
        }
    }
    create_settings_widget("Wake Timeout", dropdown, panel);

    toggle = lv_switch_create(panel);
    lv_obj_add_event_cb(toggle, rotate_screen_switch, LV_EVENT_VALUE_CHANGED, NULL);
    if (global_config.rotateScreen)
        lv_obj_add_state(toggle, LV_STATE_CHECKED);
    create_settings_widget("Rotate Screen", toggle, panel);

    toggle = lv_switch_create(panel);
    lv_obj_add_event_cb(toggle, not_used_yet_switch, LV_EVENT_VALUE_CHANGED, NULL);
    if (global_config.notused)
        lv_obj_add_state(toggle, LV_STATE_CHECKED);
    create_settings_widget("Not used yet", toggle, panel);

    lv_obj_t * textarea = lv_textarea_create(panel);
    lv_obj_add_event_cb(textarea, edit_device_list_switch, LV_EVENT_ALL, panel);
    String text;
    for (int i = 0; i < sizeof(global_config.ListDevices) / sizeof(*global_config.ListDevices); i = i + 1)
    {
        if (i > 0) text = text + ";";
        text = text + String(global_config.ListDevices[i]);
    }
    lv_textarea_add_text(textarea, text.c_str());
    lv_textarea_set_one_line(textarea, true);
    lv_obj_set_width(textarea, lv_pct(60));
    create_settings_widget("Dft devices", textarea, panel);

    btn = lv_btn_create(panel);
    lv_obj_add_event_cb(btn, exit_click, LV_EVENT_CLICKED, NULL);
    label = lv_label_create(btn);
    lv_label_set_text_static(label, "Quit");
    lv_obj_center(label);
    create_settings_widget("Exit Option", btn, panel);

}