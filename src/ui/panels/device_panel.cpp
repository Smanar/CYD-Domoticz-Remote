#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "lvgl.h"
#include "panel.h"
#include "../navigation.h"
#include "../../core/data_setup.h"
#include "../../conf/global_config.h"
#include "../main_ui.h"


static lv_obj_t * slider_label; // Label for slider
static lv_obj_t * slider_TH; // Slider for thermostat

//static Device SelectedDevice; // The selected one
static Device SpecialDevice; // structure for an actual one that is not on HP
static Device *SelectedDevice; // The selected one

extern lv_style_t style_shadow;

static void return_btn_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        ReturnPreviouspage();
    }
}

static void colorwheel_event_cb(lv_event_t * e)
{
    lv_obj_t * cw = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_color_t c = lv_colorwheel_get_rgb(cw);

        char buff[256] = {};
        snprintf(buff, 256, "/json.htm?type=command&param=setcolbrightnessvalue&idx=%d&color={\"m\":3,\"t\":0,\"r\":%d,\"g\":%d,\"b\":%d,\"cw\":0,\"ww\":0}&brightness=%d",
                        SelectedDevice->idx, c.ch.red, c.ch.green, c.ch.blue, SelectedDevice->level);
        HTTPGETRequest(buff);
    }
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (slider_label)
    {
        lv_label_set_text_fmt(slider_label, "%d%%",(int)lv_slider_get_value(slider));
    }

    if (code == LV_EVENT_VALUE_CHANGED) {
        char buff[256] = {};
        lv_snprintf(buff, 256, "/json.htm?type=command&param=switchlight&idx=%d&switchcmd=Set%%20Level&level=%d", SelectedDevice->idx, (int)lv_slider_get_value(slider));
        HTTPGETRequest(buff);
    }
}

static void TH_btn_event_handler(lv_event_t * e)
{

    int b = (int)lv_event_get_user_data(e);
    int v = lv_slider_get_value(slider_TH);

    switch (b)
    {
        case 1:
            v+=1;
            lv_slider_set_value(slider_TH, v, LV_ANIM_ON);
        break;
        case 2:
            v-=1;
            lv_slider_set_value(slider_TH, v, LV_ANIM_ON);
        break;
        case 3:
            char buff[256] = {};
            lv_snprintf(buff, 256, "/json.htm?type=command&param=setsetpoint&idx=%d&setpoint=%.1f", SelectedDevice->idx, v/10.f);
            HTTPGETRequest(buff);
        break;
    }

    if (slider_label)
    {
        lv_label_set_text_fmt(slider_label, "%.1f",(float)(v/10.f));
    }
}

static void switch_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        char buff[256] = {};
        lv_snprintf(buff, 256, "/json.htm?type=command&param=switchlight&idx=%d&switchcmd=%s", SelectedDevice->idx, lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
        HTTPGETRequest(buff);
    }
}

static void btn_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    char * s = (char *)lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED) {
        char buff[256] = {};
        lv_snprintf(buff, 256, "/json.htm?type=command&param=switchlight&idx=%d&switchcmd=%s", SelectedDevice->idx, s);
        HTTPGETRequest(buff);
    }
}

static void dd_event_handler(lv_event_t * e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    const lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        //char buf[32];
        //lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        //LV_LOG_USER("Option: %s", buf);
        int index = lv_dropdown_get_selected(obj);

        char buff[256] = {};
        lv_snprintf(buff, 256, "/json.htm?type=command&param=switchlight&idx=%d&switchcmd=Set%%20Level&level=%d", SelectedDevice->idx, index * 10);
        HTTPGETRequest(buff);
    }
}

static void hist_chart_event_cb(lv_event_t * e)
{
    //Get the event descriptor
    lv_obj_draw_part_dsc_t* dsc = lv_event_get_draw_part_dsc(e);

    //Check what part we are updating. Only proceed if we are updating one of the tick label(s)
    if (!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL))
        return;

    //Check this is a callback for a major tick (minor ticks have 0 here)
    if (dsc->text == NULL)
        return;

    //Get the multiplier from the user data
    int coef = (int)lv_event_get_user_data(e);

    //If we are editing the Y axis
    if (dsc->id == LV_CHART_AXIS_PRIMARY_Y)
    {
        //update the label with the modified multiplier
        lv_snprintf(dsc->text, dsc->text_length, "%.1f", (float)dsc->value / coef);
    }
}
void Select_deviceMemorised(void *device)
{
    //This one is already memorised so just pick it
    SelectedDevice = (Device *)device;
    navigation_screen(DEVICE_PANEL);
}

void Select_deviceIDX(int idx)
{
    //This one is empty so get data
    FillDeviceData(&SpecialDevice, idx);
    SelectedDevice = &SpecialDevice;
    navigation_screen(DEVICE_PANEL);
}

lv_color_t Getcolor(int type)
{
    switch (type)
    {
        case TYPE_SWITCH_SENSOR:
        case TYPE_LUX:
        case TYPE_PERCENT_SENSOR:
        case TYPE_VALUE_SENSOR:
        case TYPE_AIR_QUALITY:
            return LV_COLOR_MAKE(0x00, 0x7F, 0xFF);
        case TYPE_METEO:
        case TYPE_TEMPERATURE:
        case TYPE_HUMIDITY:
            return LV_COLOR_MAKE(0xFF, 0x33, 0xFF);
        case TYPE_WARNING:
        case TYPE_TEXT:
            return LV_COLOR_MAKE(0x00, 0xFF, 0x00);
        case TYPE_SPEAKER:
        case TYPE_SETPOINT:
            return LV_COLOR_MAKE(0xFF, 0xFF, 0x00);
        case TYPE_SWITCH:
        case TYPE_SELECTOR:
        case TYPE_DIMMER:
        case TYPE_PLUG:
        case TYPE_COLOR:
        case TYPE_LIGHT:
        case TYPE_BLINDS:
        case TYPE_PUSH:
            return LV_COLOR_MAKE(0xFF, 0x2F, 0x2F); 
        default:
        break;
    }

    return LV_COLOR_MAKE(0x00, 0x7F, 0xFF);
}

LV_IMG_DECLARE(temperature35x35)
LV_IMG_DECLARE(lampe35x35)
LV_IMG_DECLARE(plug35x35)
LV_IMG_DECLARE(warning35x35)
LV_IMG_DECLARE(speaker35x35)
LV_IMG_DECLARE(humidity35x35)
LV_IMG_DECLARE(power35x35)
LV_IMG_DECLARE(sensor35x35)
LV_IMG_DECLARE(blinds35x35)
LV_IMG_DECLARE(unknown35x35)
LV_IMG_DECLARE(meteo35x35)
LV_IMG_DECLARE(percent35x35)

// To convert image https://lvgl.io/tools/imageconverter
const lv_img_dsc_t *Geticon(int type)
{
    switch (type)
    {
        case TYPE_TEMPERATURE:
        case TYPE_SETPOINT:
            return &temperature35x35;
        case TYPE_HUMIDITY:
            return &humidity35x35;
        case TYPE_CONSUMPTION:
        case TYPE_POWER:
            return &power35x35;
        case TYPE_WARNING:
            return &warning35x35;
        case TYPE_PLUG:
            return &plug35x35;
        case TYPE_SPEAKER:
            return &speaker35x35;
        case TYPE_SELECTOR:
        case TYPE_DIMMER:
        case TYPE_SWITCH:
        case TYPE_COLOR:
        case TYPE_LIGHT:
        case TYPE_PUSH:
            return &lampe35x35; 
        case TYPE_BLINDS:
            return &blinds35x35;
        case TYPE_PERCENT_SENSOR:
            return &percent35x35;
        case TYPE_SWITCH_SENSOR:
        case TYPE_LUX:
        case TYPE_VALUE_SENSOR:
        case TYPE_AIR_QUALITY:
            return &sensor35x35;
        case TYPE_METEO:
            return &meteo35x35;
        default:
        break;
    }

    return &unknown35x35;
}

void device_panel_init(lv_obj_t* panel)
{

    lv_color_t color = Getcolor(SelectedDevice->type);
    const lv_img_dsc_t *icon = Geticon(SelectedDevice->type);
    lv_obj_t * label;

    static lv_coord_t col_dsc[] = {LV_GRID_FR(34), LV_GRID_FR(33), LV_GRID_FR(33), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(30), LV_GRID_FR(54), LV_GRID_FR(16), LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    lv_obj_t * cont = lv_obj_create(panel);
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
#if DEVICE_SIZE == 1
    lv_obj_set_size(cont, lv_pct(90), lv_pct(90));
#else
    lv_obj_set_size(cont, lv_pct(80), lv_pct(80));
#endif
    //lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(cont);

    lv_obj_add_style(cont, &style_shadow, LV_PART_MAIN);
    lv_obj_clear_flag( cont, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_set_style_pad_row(cont, 0, LV_PART_MAIN);  //Remove space beetween grid part
    lv_obj_set_style_pad_column(cont, 0, LV_PART_MAIN); //Remove space beetween grid part
    lv_obj_set_style_pad_all(cont, 0, LV_PART_MAIN); //Remove parent padding

    lv_obj_t * GridTop = lv_obj_create(cont);
    lv_obj_set_size(GridTop, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(GridTop, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_STRETCH, 0, 1);
    //lv_obj_set_local_style_prop_meta(GridTop, LV_STYLE_TEXT_COLOR, LV_STYLE_PROP_META_INHERIT, LV_PART_MAIN);
    lv_obj_set_style_bg_color(GridTop, lv_palette_darken(LV_PALETTE_GREY, 4),0);
    lv_obj_set_style_border_width(GridTop, 4, 0);
    //lv_obj_set_style_border_color(cw, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    //lv_obj_set_style_border_opa(cw, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_clear_flag( GridTop, LV_OBJ_FLAG_SCROLLABLE );

    lv_obj_t * GridSmall = lv_obj_create(cont);
    lv_obj_set_size(GridSmall, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(GridSmall, LV_GRID_ALIGN_STRETCH, 2, 1,LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_style_bg_color(GridSmall, lv_palette_darken(LV_PALETTE_GREY, 4),0);
    lv_obj_set_style_border_width(GridSmall, 0, 0);


    lv_obj_t * GridBig = lv_obj_create(cont);
    lv_obj_set_size(GridBig, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(GridBig, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 1, 2);
    lv_obj_set_style_bg_color(GridBig, lv_palette_darken(LV_PALETTE_GREY, 4),0);
    lv_obj_set_style_border_width(GridBig, 0, 0);


    lv_obj_t * GridExit = lv_obj_create(cont);
    lv_obj_set_size(GridExit, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_clear_flag( GridExit, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_set_grid_cell(GridExit, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_set_style_bg_color(GridExit, lv_palette_darken(LV_PALETTE_GREY, 4),0);
    lv_obj_set_style_border_width(GridExit, 0, 0);
    lv_obj_set_style_pad_all(GridExit, 0, 0);

    // Icon
    lv_obj_t *img = lv_img_create(GridTop);
    lv_img_set_src(img, icon);
    //lv_obj_align(img, LV_ALIGN_LEFT_MID , 0, 0);
    //lv_obj_set_size(img, Size_icon, Size_icon);
    lv_obj_set_style_img_recolor_opa(img, 50, 0);
    lv_obj_set_style_img_recolor(img, color, 0);
    //Label
    label = lv_label_create(GridTop);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); 
    lv_label_set_text(label, SelectedDevice->name);
    //lv_obj_align(label, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(label, TFT_WIDTH - 50, 30);
    lv_obj_align_to(label, img,  LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    //Options
    //
    //Create switch
    if ((SelectedDevice->type == TYPE_SWITCH) || (SelectedDevice->type == TYPE_DIMMER)
     || (SelectedDevice->type == TYPE_PLUG) || (SelectedDevice->type == TYPE_COLOR) || (SelectedDevice->type == TYPE_LIGHT))
    {
        lv_obj_t * sw = lv_switch_create(GridSmall);
        if (strcmp(SelectedDevice->data, "On") == 0)
        {
            lv_obj_add_state(sw, LV_STATE_CHECKED);
        }
        lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_ALL, NULL);

    }

    //Create a push switch
    if (SelectedDevice->type == TYPE_PUSH)
    {
        lv_obj_t * sw = lv_btn_create(GridBig);
        lv_obj_set_size(sw, LV_PCT(60), LV_PCT(50));
        lv_obj_align(sw, LV_ALIGN_CENTER, 0, 0);
        label = lv_label_create(sw);
        if (strcmp(SelectedDevice->data, "On") == 0)
        {
            lv_label_set_text_static(label, "ON");
            lv_obj_add_event_cb(sw, btn_event_handler, LV_EVENT_ALL,(void *)"On");
        }
        else
        {
            lv_label_set_text_static(label, "OFF");
            lv_obj_add_event_cb(sw, btn_event_handler, LV_EVENT_ALL,(void *)"Off");
        }
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        //lv_obj_set_pos(obj, 0, 0);
    }

    //Create blinds buttons
    if (SelectedDevice->type == TYPE_BLINDS)
    {
        lv_obj_t * obj= lv_btn_create(GridSmall);
        lv_obj_set_size(obj, LV_PCT(100), LV_PCT(30));
        label = lv_label_create(obj);
        lv_label_set_text_static(label, LV_SYMBOL_UP);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_pos(obj, 0, 0);
#ifdef OLD_DOMOTICZ
        lv_obj_add_event_cb(obj, btn_event_handler, LV_EVENT_CLICKED, (char *)"Off");
#else
        lv_obj_add_event_cb(obj, btn_event_handler, LV_EVENT_CLICKED, (char *)"Open");
#endif
        obj= lv_btn_create(GridSmall);
        lv_obj_set_size(obj, LV_PCT(100), LV_PCT(30));
        label = lv_label_create(obj);
        lv_label_set_text_static(label, LV_SYMBOL_STOP);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_pos(obj, 0, LV_PCT(33));
        lv_obj_add_event_cb(obj, btn_event_handler, LV_EVENT_CLICKED, (char *)"Stop");
        obj= lv_btn_create(GridSmall);
        lv_obj_set_size(obj, LV_PCT(100), LV_PCT(30));
        label = lv_label_create(obj);
        lv_label_set_text_static(label, LV_SYMBOL_DOWN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_pos(obj, 0, LV_PCT(66));
#ifdef OLD_DOMOTICZ
        lv_obj_add_event_cb(obj, btn_event_handler, LV_EVENT_CLICKED, (char *)"On");
#else
        lv_obj_add_event_cb(obj, btn_event_handler, LV_EVENT_CLICKED, (char *)"Close");
#endif
    }

    //Create a LED
    if (SelectedDevice->type == TYPE_SWITCH_SENSOR) 
    {
        lv_obj_t * led  = lv_led_create(GridBig);
        lv_obj_align(led, LV_ALIGN_CENTER, 0, 0);
        if (strcmp(SelectedDevice->data, "On") == 0)
        {
            lv_led_on(led);
        }
        else
        {
            lv_led_off(led);
        }
    }

    //Create a slider
    if ((SelectedDevice->type == TYPE_DIMMER) || (SelectedDevice->type == TYPE_BLINDS))
    {

        lv_obj_t * slider = lv_slider_create(GridBig);
        lv_slider_set_value(slider, SelectedDevice->level, LV_ANIM_ON);
        lv_obj_set_width(slider, lv_pct(80));
        lv_slider_set_range(slider, 0, 100);
        lv_obj_center(slider);
        lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

        /*Create a label below the slider*/
        slider_label = lv_label_create(lv_scr_act());
        lv_label_set_text_fmt(slider_label, "%d%",SelectedDevice->level);
        lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
    }

    //Create a color wheel + slider
    if (SelectedDevice->type == TYPE_COLOR)
    {
        /*Create a slider*/
        lv_obj_t * slider = lv_slider_create(GridBig);
        lv_slider_set_value(slider, SelectedDevice->level, LV_ANIM_ON);
        lv_obj_set_size(slider, 10, lv_pct(80));
        lv_slider_set_range(slider, 0, 100);
        lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        //lv_obj_align_to(slider, GridBig, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        lv_obj_align(slider, LV_ALIGN_LEFT_MID, 0, 0);
        slider_label = NULL; // Do not use the value display, not enought place

        /*Create a color picker*/
        lv_obj_t * cw = lv_colorwheel_create(GridBig, true);
        lv_obj_set_style_arc_width(cw, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(cw, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(cw, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(cw, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(cw, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
        lv_obj_align(cw, LV_ALIGN_RIGHT_MID, -10, 0);

        lv_obj_set_size(cw, lv_pct(60), lv_pct(80));
        //lv_colorwheel_set_color(cw, rgb);
        lv_obj_add_event_cb(cw, colorwheel_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        //lv_obj_center(cw);
    }

    //Selector switch
    if (SelectedDevice->type == TYPE_SELECTOR || SelectedDevice->type == TYPE_SPEAKER)
    {
        /*Create a normal drop down list*/
        lv_obj_t * dd = lv_dropdown_create(GridBig);
        lv_dropdown_set_options(dd, SelectedDevice->levelname);

        //lv_obj_align(dd, LV_ALIGN_TOP_MID, 0, 20);
        lv_obj_add_event_cb(dd, dd_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
        lv_dropdown_set_selected(dd, SelectedDevice->level / 10);
    }

    // Info and Text device
    if ((SelectedDevice->type == TYPE_WARNING) ||(SelectedDevice->type == TYPE_TEXT))
    {
        lv_obj_add_flag(cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);       //Force new line
        lv_obj_add_flag(cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);       //Force new line

        //Display Text
        label = lv_label_create(GridBig);
        lv_obj_set_style_text_font(label, &font1, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_align(label,  LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_text_color(label, color, 0);
        lv_obj_set_size(label, lv_pct(100), lv_pct(80)); 
        lv_label_set_text(label, SelectedDevice->data);
    }

    // Info device 
    if (SelectedDevice->type == TYPE_WARNING) 
    {
        lv_obj_t * led  = lv_led_create(GridSmall);
        lv_obj_set_size(led, lv_pct(80), lv_pct(60));
        lv_obj_align(led, LV_ALIGN_CENTER, 0, 0);
        lv_led_on(led);

        //Color the led according to alert Level
        if (SelectedDevice->level == 1)
        {
            lv_led_set_color(led, LV_COLOR_MAKE(0x00, 0xFF, 0x00));
        }
        else if (SelectedDevice->level == 2)
        {
            lv_led_set_color(led, LV_COLOR_MAKE(0xFF, 0xFF, 0x00));
        }
        else if (SelectedDevice->level == 3)
        {
            lv_led_set_color(led, LV_COLOR_MAKE(0xFF, 0x80, 0x00));
        }
        else if (SelectedDevice->level == 4)
        {
            lv_led_set_color(led, LV_COLOR_MAKE(0xFF, 0x00, 0x00));
        }
        else
        {
            lv_led_set_color(led, lv_palette_main(LV_PALETTE_GREY));
        }

    }

    //Thermostat setpoint
    if (SelectedDevice->type == TYPE_SETPOINT)
    {
        slider_TH = lv_slider_create(GridBig);
        lv_obj_set_width(slider_TH, lv_pct(80));
        lv_slider_set_range(slider_TH, 0, 400);
        lv_slider_set_value(slider_TH, atof(SelectedDevice->data) *10, LV_ANIM_ON);
        lv_obj_center(slider_TH);
        
        //lv_obj_set_style_bg_color(slider_TH, lv_color_hex(0x569af8), LV_PART_INDICATOR);
        //lv_obj_set_style_bg_color(slider_TH, lv_color_hex(0xff0000), LV_PART_INDICATOR | LV_STATE_PRESSED);
        lv_obj_remove_style(slider_TH, NULL, LV_PART_INDICATOR);

        /*Create a label below the slider*/
        slider_label = lv_label_create(lv_scr_act());
        lv_label_set_text_fmt(slider_label, "%.1f",atof(SelectedDevice->data)/1.f);
        lv_obj_align_to(slider_label, slider_TH, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);

        // And somes buttons
        lv_obj_t * obj= lv_btn_create(GridSmall);
        lv_obj_set_size(obj, LV_PCT(100), LV_PCT(30));
        label = lv_label_create(obj);
        lv_label_set_text_static(label, "+");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_pos(obj, 0, 0);
        lv_obj_add_event_cb(obj, TH_btn_event_handler, LV_EVENT_CLICKED, (void *)1);
        obj= lv_btn_create(GridSmall);
        lv_obj_set_size(obj, LV_PCT(100), LV_PCT(30));
        label = lv_label_create(obj);
        lv_label_set_text_static(label, "-");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_pos(obj, 0, LV_PCT(33));
        lv_obj_add_event_cb(obj, TH_btn_event_handler, LV_EVENT_CLICKED, (void *)2);
        obj= lv_btn_create(GridSmall);
        lv_obj_set_size(obj, LV_PCT(100), LV_PCT(30));
        label = lv_label_create(obj);
        lv_label_set_text_static(label, "Set");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_pos(obj, 0, LV_PCT(66));
        lv_obj_add_event_cb(obj, TH_btn_event_handler, LV_EVENT_CLICKED, (char *)3);

    }

    // Other sensors
    if ((SelectedDevice->type == TYPE_TEMPERATURE) || (SelectedDevice->type == TYPE_VALUE_SENSOR)
    || (SelectedDevice->type == TYPE_HUMIDITY) || (SelectedDevice->type == TYPE_CONSUMPTION) || (SelectedDevice->type == TYPE_POWER)
    || (SelectedDevice->type == TYPE_LUX) || (SelectedDevice->type == TYPE_PERCENT_SENSOR) || (SelectedDevice->type == TYPE_METEO))
    {

        label = lv_label_create(GridSmall);
        lv_obj_set_style_text_font(label, &font1, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_set_size(label, lv_pct(100), lv_pct(100));
        lv_label_set_text(label, SelectedDevice->data);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_text_color(label, color, 0);

#if LV_USE_CHART
        //Making a graph
        int min = 0;
        int max = 0;
        int* pTab = nullptr; // Pointer to graph value

        pTab = GetGraphValue(SelectedDevice->type, SelectedDevice->idx, &min, &max);

        if (pTab)
        {
            lv_obj_t * chart;
            chart = lv_chart_create(GridBig);
            lv_obj_set_size(chart, lv_pct(100)-15, lv_pct(100));
            //lv_obj_center(chart);
            lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);
            lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, min , max );
            //lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_X, 0 , 24 );
            lv_chart_set_point_count(chart, 24);

            lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);

            // Make a scale
            lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 3, 2, 3, 1, true, 25);
            lv_obj_set_style_text_font(chart, &font2, 0);

            Serial.printf("Making chart with Range %d > %d\n",min , max);

            uint16_t i;
            for (i = 0; i < 24; i++)
            {
                lv_chart_set_next_value(chart, ser1, pTab[i]);
                //Serial.printf("Using values %d\n", pTab[i] );
            }

            //For float value
            if (SelectedDevice->type == TYPE_TEMPERATURE)
            {
                lv_obj_add_event_cb(chart, hist_chart_event_cb, LV_EVENT_DRAW_PART_BEGIN, (void *)10);
            }

            lv_chart_refresh(chart); /*Required after direct set*/
        }
        else
        {
            Serial.printf("Can't retreive graph value\n");
        }
#endif

    }

    // Back button
    lv_obj_t * returnBtn = lv_btn_create(GridExit);
    lv_obj_set_size(returnBtn, LV_PCT(80)  , LV_PCT(80) );
    lv_obj_set_style_bg_color(returnBtn, lv_palette_darken(LV_PALETTE_RED, 4),0);
    lv_obj_set_style_border_width(returnBtn, 4, 4);
    lv_obj_add_event_cb(returnBtn, return_btn_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align_to(returnBtn, GridExit, LV_ALIGN_BOTTOM_RIGHT, -4, -4);
    //lv_obj_center(returnBtn);

    label = lv_label_create(returnBtn);
    lv_label_set_text_static(label, "Back");
    lv_obj_center(label);

}