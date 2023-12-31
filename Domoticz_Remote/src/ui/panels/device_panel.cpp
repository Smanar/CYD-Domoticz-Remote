#include "lvgl.h"
#include "panel.h"
#include "../navigation.h"
#include "../../core/data_setup.h"
#include "../../conf/global_config.h"
#include "../main_ui.h"

#include <HardwareSerial.h>
#include <HTTPClient.h>

LV_FONT_DECLARE(Montserrat_Bold_14)

int Selected_device = 0;
static lv_obj_t * slider_label;

extern Device myDevices[];
extern lv_style_t style_shadow;

static void return_btn_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        navigation_screen(0);
    }
}

static void colorwheel_event_cb(lv_event_t * e)
{
    lv_obj_t * cw = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_color_t c = lv_colorwheel_get_rgb(cw);

        char buff[256] = {};
        sprintf(buff, "/json.htm?type=command&param=setcolbrightnessvalue&idx=%d&color={\"m\":3,\"t\":0,\"r\":%d,\"g\":%d,\"b\":%d,\"cw\":0,\"ww\":0}&brightness=%d",
                        myDevices[Selected_device].idx, c.ch.red, c.ch.green, c.ch.blue, 100);
        HTTPGETRequest(buff);
    }
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (slider_label)
    {
        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
        lv_label_set_text(slider_label, buf);
        lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    }

    if (code == LV_EVENT_VALUE_CHANGED) {
        char buff[256] = {};
        sprintf(buff, "/json.htm?type=command&param=switchlight&idx=%d&switchcmd=Set%%20Level&level=%d", myDevices[Selected_device].idx, (int)lv_slider_get_value(slider));
        HTTPGETRequest(buff);
    }
}

static void  switch_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        char buff[256] = {};
        sprintf(buff, "/json.htm?type=command&param=switchlight&idx=%d&switchcmd=%s", myDevices[Selected_device].idx, lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
        HTTPGETRequest(buff);
    }
}


static void dd_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        //char buf[32];
        //lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        //LV_LOG_USER("Option: %s", buf);
        int index = lv_dropdown_get_selected(obj);

        char buff[256] = {};
        sprintf(buff, "/json.htm?type=command&param=switchlight&idx=%d&switchcmd=Set%%20Level&level=%d", myDevices[Selected_device].idx, index * 10);
        HTTPGETRequest(buff);
    }
}

void Select_device(int device)
{
    Selected_device = device;
    navigation_screen(1);
}

lv_color_t Getcolor(int type)
{
    switch (type)
    {
        case TYPE_TEMPERATURE:
        case TYPE_HUMIDITY:
            return LV_COLOR_MAKE(0x00, 0x7F, 0xFF);
        break;
        case TYPE_WARNING:
            return LV_COLOR_MAKE(0x00, 0xFF, 0x00);
        break;
        case TYPE_SPEAKER:
            return LV_COLOR_MAKE(0xFF, 0xFF, 0x00);
        case TYPE_SWITCH:
        case TYPE_SELECTOR:
        case TYPE_DIMMER:
        case TYPE_PLUG:
        case TYPE_COLOR:
            return LV_COLOR_MAKE(0xFF, 0x2F, 0x2F); 
        break;
        default:
        break;
    }

    return LV_COLOR_MAKE(0x00, 0xFF, 0xFF);
}

LV_IMG_DECLARE(temperature35x35)
LV_IMG_DECLARE(lampe35x35)
LV_IMG_DECLARE(plug35x35)
LV_IMG_DECLARE(warning35x35)
LV_IMG_DECLARE(speaker35x35)
LV_IMG_DECLARE(humidity35x35)
LV_IMG_DECLARE(power35x35)

// To convert image https://lvgl.io/tools/imageconverter
const lv_img_dsc_t *Geticon(int type)
{
    switch (type)
    {
        case TYPE_TEMPERATURE:
            return &temperature35x35;
        break;
        case TYPE_HUMIDITY:
            return &humidity35x35;
        break;
        case TYPE_CONSUMPTION:
            return &power35x35;
        break;
        case TYPE_WARNING:
            return &warning35x35;
        break;
        case TYPE_PLUG:
            return &plug35x35;
        break;
        case TYPE_SPEAKER:
            return &speaker35x35;
        case TYPE_SELECTOR:
        case TYPE_DIMMER:
        case TYPE_SWITCH:
        case TYPE_COLOR:
            return &lampe35x35; 
        break;
        default:
        break;
    }

    return &temperature35x35;
}

void device_panel_init(lv_obj_t* panel)
{

    lv_color_t color = Getcolor(myDevices[Selected_device].type);
    const lv_img_dsc_t *icon = Geticon(myDevices[Selected_device].type);
    lv_obj_t * label;

    //Container
    static lv_style_t style_container;
    lv_style_init(&style_container);
    lv_style_set_flex_flow(&style_container, LV_FLEX_FLOW_ROW_WRAP); // Place the children in a row with wrapping
    lv_style_set_flex_main_place(&style_container, LV_FLEX_ALIGN_SPACE_EVENLY); // items are distributed so that the spacing between any two items (and the space to the edges) is equal
    lv_style_set_layout(&style_container, LV_LAYOUT_FLEX); // Make Flex layout

    lv_obj_t * cont = lv_obj_create(panel);
    lv_obj_set_size(cont, lv_pct(80), lv_pct(90));
    //lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(cont);
    //lv_obj_add_style(cont, &style_shadow, 0);
    lv_obj_add_style(cont, &style_shadow, LV_PART_MAIN);
    //lv_obj_add_style(cont, &style_container, 0);
    lv_obj_add_style(cont, &style_container, LV_PART_MAIN);
    lv_obj_clear_flag( cont, LV_OBJ_FLAG_SCROLLABLE );

    // Icon
    lv_obj_t *img = lv_img_create(cont);
    lv_img_set_src(img, icon);
    //lv_obj_align(img, LV_ALIGN_TOP_MID , 0, -10);
    //lv_obj_set_size(img, Size_icon, Size_icon);
    lv_obj_set_style_img_recolor_opa(img, 50, 0);
    lv_obj_set_style_img_recolor(img, color, 0);

    //Label
    label = lv_label_create(cont);
    lv_label_set_text(label, myDevices[Selected_device].name);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_flag(cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);       //Force new line

    //Options
    if ((myDevices[Selected_device].type == TYPE_SWITCH) || (myDevices[Selected_device].type == TYPE_DIMMER)
     || (myDevices[Selected_device].type == TYPE_PLUG) || (myDevices[Selected_device].type == TYPE_COLOR))
    {
        //Create switch
        lv_obj_t * sw = lv_switch_create(cont);
        if (strcmp(myDevices[Selected_device].data, "On") == 0)
        {
            lv_obj_add_state(sw, LV_STATE_CHECKED);
        }
        lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_ALL, NULL);
    }

    if (myDevices[Selected_device].type == TYPE_DIMMER)
    {
        /*Create a slider in the center of the display*/
        lv_obj_t * slider = lv_slider_create(cont);
        lv_slider_set_value(slider, myDevices[Selected_device].level, LV_ANIM_ON);
        lv_obj_set_width(slider, lv_pct(80));
        lv_obj_center(slider);
        lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

        /*Create a label below the slider*/
        slider_label = lv_label_create(lv_scr_act());
        lv_label_set_text(slider_label, "0%");
        lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    }

    if (myDevices[Selected_device].type == TYPE_COLOR)
    {
        
        /*Create a slider in the center of the display*/
        lv_obj_t * slider = lv_slider_create(panel);
        lv_slider_set_value(slider, myDevices[Selected_device].level, LV_ANIM_ON);
        lv_obj_set_size(slider, 10, lv_pct(40));
        lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        //lv_obj_align_to(slider, cont, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        lv_obj_align_to(slider, cont, LV_ALIGN_BOTTOM_MID, -90, 0);
        slider_label = NULL; // Do not use the value display, not enought place

        /*Create a color picker*/
        lv_obj_t * cw = lv_colorwheel_create(cont, true);
        lv_obj_set_style_arc_width(cw, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(cw, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(cw, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(cw, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(cw, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

        lv_obj_set_size(cw, lv_pct(44), lv_pct(50));
        //lv_colorwheel_set_color(cw, rgb);
        lv_obj_add_event_cb(cw, colorwheel_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        //lv_obj_center(cw);
    }

    if (myDevices[Selected_device].type == TYPE_SELECTOR || myDevices[Selected_device].type == TYPE_SPEAKER)
    {
        /*Create a normal drop down list*/
        lv_obj_t * dd = lv_dropdown_create(cont);
        lv_dropdown_set_options(dd, myDevices[Selected_device].levelname);

        //lv_obj_align(dd, LV_ALIGN_TOP_MID, 0, 20);
        lv_obj_add_event_cb(dd, dd_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
        lv_dropdown_set_selected(dd, myDevices[Selected_device].level / 10);
    }

    if (myDevices[Selected_device].type == TYPE_WARNING)
    {
        lv_obj_add_flag(cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);       //Force new line
        lv_obj_add_flag(cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);       //Force new line

        label = lv_label_create(panel);
        lv_obj_set_style_text_font(label, &Montserrat_Bold_14, 0);
        lv_label_set_text(label, myDevices[Selected_device].data);
        lv_obj_align_to(label, cont,  LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_text_color(label, color, 0);
    }

    lv_obj_add_flag(cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);       //Force new line

    // Back button
    lv_obj_t * returnBtn = lv_btn_create(panel);
    lv_obj_add_event_cb(returnBtn, return_btn_event_handler, LV_EVENT_ALL, NULL);
    //lv_obj_align(returnBtn, LV_ALIGN_CENTER, 0, 40);
    //lv_obj_center(returnBtn);
    //lv_obj_set_y(returnBtn, 200); // No effect
    lv_obj_align_to(returnBtn, cont, LV_ALIGN_BOTTOM_MID, -10, -10);

    label = lv_label_create(returnBtn);
    lv_label_set_text(label, "Back");
    lv_obj_center(label);

}