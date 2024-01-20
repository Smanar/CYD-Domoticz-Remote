#include "lvgl.h"
#include "panels/panel.h"
#include "../core/data_setup.h"
#include "nav_buttons.h"
#include <HTTPClient.h>

static lv_style_t nav_button_style;

static char temp_buffer[10];
static char z_pos_buffer[10];
static char time_buffer[10];

static lv_style_t nav_button_text_style;


static void update_setting(lv_event_t * e) {
    lv_obj_t * label = lv_event_get_target(e);

    lv_label_set_text(label, "ee");
}

static void btn_click_home(lv_event_t * e){
    //lv_obj_t * label = lv_event_get_target(e);
    nav_buttons_setup(0);
}

static void btn_click_settings(lv_event_t * e){
    nav_buttons_setup(1);
}


static void btn_click_information(lv_event_t * e){
    nav_buttons_setup(2);
}

static void btn_click_power(lv_event_t * e){
    nav_buttons_setup(3);
}

void RefreshHomePage(void)
{
   nav_buttons_setup(0);
}

void nav_buttons_setup(unsigned char active_panel)
{
    lv_obj_clean(lv_scr_act());
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

    const int button_width = 40;
    const int button_height = 60;
    const int icon_text_spacing = 10;

    // Home button
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, button_width, button_height);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_style(btn, &nav_button_style, 0);
    lv_obj_add_event_cb(btn, btn_click_home, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_HOME);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -1 * icon_text_spacing);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Home");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, icon_text_spacing);
    lv_obj_add_style(label, &nav_button_text_style, 0);
    //lv_obj_add_event_cb(label, btn_click_home, LV_EVENT_MSG_RECEIVED, NULL);
    //lv_msg_subsribe_obj(DATA_PRINTER_DATA, label, NULL);

    // Setting
    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, button_width, button_height);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, button_height);
    lv_obj_add_style(btn, &nav_button_style, 0);
    lv_obj_add_event_cb(btn, btn_click_settings, LV_EVENT_CLICKED, NULL);

    label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_SETTINGS);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -1 * icon_text_spacing);
    
    label = lv_label_create(btn);
    lv_label_set_text(label, "Setting");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, icon_text_spacing);
    //lv_obj_add_event_cb(label, update_setting, LV_EVENT_MSG_RECEIVED, NULL);
    //lv_msg_subsribe_obj(DATA_PRINTER_DATA, label, NULL);
    lv_obj_add_style(label, &nav_button_text_style, 0);

    // Information
    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, button_width, button_height);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, button_height * 2);
    lv_obj_add_style(btn, &nav_button_style, 0);
    lv_obj_add_event_cb(btn, btn_click_information, LV_EVENT_CLICKED, NULL);

    label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_EYE_OPEN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -1 * icon_text_spacing);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Info");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, icon_text_spacing);
    //lv_obj_add_event_cb(label, btn_click_information, LV_EVENT_MSG_RECEIVED, NULL);
    //lv_msg_subsribe_obj(DATA_PRINTER_DATA, label, NULL);
    lv_obj_add_style(label, &nav_button_text_style, 0);

    // Power
    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, button_width, button_height);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, button_height * 3);
    lv_obj_add_style(btn, &nav_button_style, 0);
    lv_obj_add_event_cb(btn, btn_click_power, LV_EVENT_CLICKED, NULL);

    label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_POWER);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -1 * icon_text_spacing);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Power");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, icon_text_spacing);
    lv_obj_add_style(label, &nav_button_text_style, 0);

    // The commplete menubar
    lv_obj_t * panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, TFT_HEIGHT - button_width, TFT_WIDTH);
    lv_obj_align(panel, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);

    switch (active_panel)
    {
        case 0:
            home_panel_init(panel);
            break;
        case 1:
            settings_panel_init(panel);
            break;
        case 2:
            info_panel_init(panel);
            break;
        case 3:
            //power_panel_init(panel);
            break;
        case 4:
            device_panel_init(panel);
    }

}

void nav_style_setup(){
    lv_style_init(&nav_button_style);
    lv_style_set_radius(&nav_button_style, 0);

    lv_style_init(&nav_button_text_style);
    lv_style_set_text_font(&nav_button_text_style, &lv_font_montserrat_10);
}