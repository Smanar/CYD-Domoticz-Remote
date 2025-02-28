#include <lvgl.h>

#include "panel.h"
#include "../main_ui.h"
#include "../../core/data_setup.h"
#include "../../conf/global_config.h"
#include "../../core/ip_engine.h"
#include "../navigation.h"



extern lv_style_t style_shadow;

//Calculate values to use to display the homepage (even number)
#define TOTAL_OFFSET_X 10
#define TOTAL_OFFSET_Y 10
//Calculate widgets size
int Size_w = int(TFT_WIDTH/TOTAL_ICONX) -  TOTAL_OFFSET_X;
int Size_h = int(TFT_HEIGHT/TOTAL_ICONY) - TOTAL_OFFSET_Y;
//Icon size
//int Size_icon = 35;

static void btn_event_cb_group(lv_event_t * e)
{
    int idx = (int)lv_event_get_user_data(e);
    lv_obj_t * btn = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    
    //Serial.printf("Clic sur boutton: %d with code %d\n", idx, code);
    
    char buff[256] = {};

    if(code == LV_EVENT_CLICKED) {
        lv_snprintf(buff, 256, "/json.htm?type=command&param=switchscene&idx=%d&switchcmd=%s", idx, "On");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        if (lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            lv_obj_set_style_opa(btn, LV_OPA_50, 0);
            lv_snprintf(buff, 256, "/json.htm?type=command&param=switchscene&idx=%d&switchcmd=%s", idx, "Off");
        }
        else
        {
            lv_obj_set_style_opa(btn, LV_OPA_100, 0);
            lv_snprintf(buff, 256, "/json.htm?type=command&param=switchscene&idx=%d&switchcmd=%s", idx, "On");
        }
    }

    HTTPGETRequest(buff);
}

static void btn_event_cb(lv_event_t * e)
{
    //lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t * btn = lv_event_get_target(e);
    //int *d = (int*)lv_event_get_param(e);
    void * d = (void *)lv_event_get_user_data(e); // cast the return pointer to data type pointer

#ifdef FASTCLIC
        Device *d2 = (Device *)d;
        if ((d2->type == TYPE_PUSH) || (d2->type == TYPE_LIGHT) || (d2->type == TYPE_SWITCH) || (d2->type == TYPE_PLUG))
        {
            char buff[256] = {};
            lv_snprintf(buff, 256, "/json.htm?type=command&param=switchlight&idx=%d&switchcmd=%s", d2->idx, "Toggle");
            HTTPGETRequest(buff);
            return;
        }
#endif
        //Serial.printf("Clic sur boutton: %d", * btn_no);
        Select_deviceMemorised(d);
}

static void Widget_button(lv_obj_t* panel, char* desc, int x, int y, int w, int h, lv_color_t color, Device *d, const lv_img_dsc_t* icon)
{

    /*Create a container with ROW flex direction*/
    lv_obj_t * Button_icon = lv_obj_create(panel);
    lv_obj_set_size(Button_icon, w, h);
    //lv_obj_align(Button_icon, LV_ALIGN_CENTER, x, y);
    lv_obj_set_pos(Button_icon, x, y);
    lv_obj_add_style(Button_icon, &style_shadow, LV_PART_MAIN);
    //lv_obj_set_drag(Button_icon, false);
    //lv_obj_set_flex_flow(Button_icon, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag( Button_icon, LV_OBJ_FLAG_SCROLLABLE );                                // Remove scrollbar
    //lv_obj_set_style_pad_all(Button_icon, 0, 0);                                           // Remove padding
    lv_obj_add_event_cb(Button_icon, btn_event_cb, LV_EVENT_CLICKED, (void *)d);             // Assign a callback to the button

    //special part to display small text direclty on widget
    if (d->type == TYPE_TEXT && strlen(d->data) < 6)
    {
        lv_obj_t * label2 = lv_label_create(Button_icon);               /*Add a label to the button*/
        //lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);             /*Break the long lines*/
        lv_obj_set_style_text_font(label2, &font1, 0);
        lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(label2, color, 0);

        lv_label_set_text(label2, d->data);                                /*Set the labels text*/

        lv_obj_set_width(label2, Size_w);
        lv_obj_align_to(label2, Button_icon,  LV_ALIGN_CENTER, 0, 0); 
        return;
    }

    lv_obj_t *img = lv_img_create(Button_icon);
    //lv_img_set_src(img, LV_SYMBOL_OK "Accept");
    lv_img_set_src(img, icon);
    //lv_img_set_zoom(img,512); // Work only for 256 color image or any non True color
    lv_obj_align(img, LV_ALIGN_TOP_MID , 0, -10);
    //lv_obj_set_size(img, Size_icon, Size_icon);
    lv_obj_set_style_img_recolor_opa(img, 50, 0);
    lv_obj_set_style_img_recolor(img, color, 0);

    // Display a "on" icon
    if (d->type < TYPE_SWITCH && strcmp(d->data, "On") == 0)
    {
        lv_obj_t * label = lv_label_create(Button_icon);
        lv_obj_set_style_text_color(label, color, 0);
        lv_obj_align_to(label, img,  LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
        //lv_obj_set_style_border_width(label, 5, 0); // To make it visible
        lv_label_set_text(label, " On");
    }
#if 0
    // Display a open/clode state for covering
    if (d->type == TYPE_BLINDS)
    {
        lv_obj_t * label = lv_label_create(Button_icon);
        lv_obj_set_style_text_color(label, color, 0);
        lv_obj_align_to(label, img,  LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
        if (strcmp(d->data, "Closed") == 0)
        {
            lv_label_set_text(label, " Closed");
        }
        else if (strcmp(d->data, "Open") == 0)
        {
            lv_label_set_text(label, " Open");
        }
        else
        {
            lv_label_set_text_fmt(label, " %d %%",d->level );
        }
    }
#endif

    /*Create description*/
    lv_obj_t * label2 = lv_label_create(Button_icon);               /*Add a label to the button*/
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);             /*Break the long lines*/
    lv_obj_set_style_text_font(label2, &font2, 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    //lv_label_set_recolor(label2, true);                           /*Activate coloring*/
    lv_label_set_text(label2, desc);                                /*Set the labels text*/
    //lv_obj_center(label2);
    //lv_obj_set_size(label2, Size_w-10, 30);
    lv_obj_set_width(label2, Size_w);
    lv_obj_align_to(label2, Button_icon,  LV_ALIGN_BOTTOM_MID, 0, 10); 

}

static void Widget_sensor(lv_obj_t* panel, char* desc, char* value, int x, int y, int w, int h, lv_color_t color, Device *d, const lv_img_dsc_t *icon)
{

    /*Create a container*/
    lv_obj_t * Button_icon = lv_obj_create(panel);
    lv_obj_set_size(Button_icon, w, h);
    //lv_obj_align(Button_icon, LV_ALIGN_CENTER, x, y);
    lv_obj_set_pos(Button_icon, x, y);
    lv_obj_add_style(Button_icon, &style_shadow, LV_PART_MAIN);
    lv_obj_clear_flag( Button_icon, LV_OBJ_FLAG_SCROLLABLE );
    //lv_obj_set_style_pad_all(Button_icon, 0, 0);
    lv_obj_add_event_cb(Button_icon, btn_event_cb, LV_EVENT_CLICKED, (void *)d);              /*Assign a callback to the button*/
    //lv_obj_add_event_cb(Button_icon,button_draw_event_cb,LV_EVENT_DRAW_PART_END,NULL); // To bypass drawing

    /*Create icon*/
    lv_obj_t *img = lv_img_create(Button_icon);
    //lv_img_set_src(img, LV_SYMBOL_OK "Accept");
    lv_img_set_src(img, icon);
    //lv_img_set_zoom(img,200);                         // Not working for this image type
    lv_obj_align(img, LV_ALIGN_TOP_LEFT , -10, -10);
    //lv_obj_set_size(img, Size_icon, Size_icon);
    lv_obj_set_style_img_recolor_opa(img, 50, 0);
    lv_obj_set_style_img_recolor(img, color, 0);

     /*Create Value*/
     // Does _local_ is working here ?
    lv_obj_t * label = lv_label_create(Button_icon);
    lv_obj_set_style_text_font(label, &font1, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
#if DEVICE_SIZE == 1
    lv_obj_set_height(label, 30);
    lv_obj_set_width(label, Size_w /2);
#else
    lv_obj_set_height(label, 55);
    lv_obj_set_width(label, 2* Size_w /3);
#endif
    lv_obj_align_to(label, img,  LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    //lv_obj_set_style_border_width(label, 5, 0); // To make it visible
    lv_label_set_text(label, value);

    /*Create description*/
    lv_obj_t * label2 = lv_label_create(Button_icon);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(label2, &font2, 0);
    lv_label_set_text(label2, desc);
    //lv_obj_set_size(label2, Size_w-10, 30);
    lv_obj_set_width(label2, Size_w);
    lv_obj_align_to(label2, Button_icon,  LV_ALIGN_BOTTOM_MID, 0, 10); 
}


static void Widget_button_group(lv_obj_t* panel, char* desc, int x, int y, int w, int h, lv_color_t color, int idx, const lv_img_dsc_t* icon, bool state, bool group)
{

    /*Create a container with ROW flex direction*/
    lv_obj_t * Button_icon = lv_obj_create(panel);
    lv_obj_set_size(Button_icon, w, h);
    lv_obj_set_pos(Button_icon, x, y);
    lv_obj_add_style(Button_icon, &style_shadow, LV_PART_MAIN);
    lv_obj_clear_flag( Button_icon, LV_OBJ_FLAG_SCROLLABLE );         // Remove scrollbar
    //lv_obj_add_flag( Button_icon, LV_OBJ_FLAG_CLICKABLE );
    //lv_obj_add_style(Button_icon, &style_pr, LV_STATE_PRESSED);     // Set a style when pressed

    if (group)
    {
        lv_obj_add_event_cb(Button_icon, btn_event_cb_group, LV_EVENT_VALUE_CHANGED, (void *)idx);
        lv_obj_add_flag(Button_icon, LV_OBJ_FLAG_CHECKABLE);
        if (!state)
        {
            lv_obj_set_style_opa(Button_icon, LV_OPA_50, 0);
            lv_obj_add_state(Button_icon, LV_STATE_CHECKED);
        }
    }
    else
    {
        lv_obj_add_event_cb(Button_icon, btn_event_cb_group, LV_EVENT_CLICKED, (void *)idx);
    }

    lv_obj_t *img = lv_img_create(Button_icon);
    lv_img_set_src(img, icon);
    lv_obj_align(img, LV_ALIGN_TOP_MID , 0, -10);
    lv_obj_set_style_img_recolor_opa(img, 50, 0);
    lv_obj_set_style_img_recolor(img, color, 0);

    /*Create description*/
    lv_obj_t * label2 = lv_label_create(Button_icon);               /*Add a label to the button*/
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);             /*Break the long lines*/
    lv_obj_set_style_text_font(label2, &font2, 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label2, desc);                                /*Set the labels text*/
    lv_obj_set_width(label2, Size_w);
    lv_obj_align_to(label2, Button_icon,  LV_ALIGN_BOTTOM_MID, 0, 10); 

}

void home_panel_init(lv_obj_t* panel, Device d[], short page)
{
    short x,y;
    short cx,cy;
    short i = (page * TOTAL_ICONX * TOTAL_ICONY);

    for (y=0; y<TOTAL_ICONY; y=y+1)
    {
        for (x=0; x<TOTAL_ICONX; x=x+1)
        {
            const lv_color_t device_color = Getcolor(d[i].type);
            const lv_img_dsc_t *icon = Geticon(d[i].type);

            cx = TOTAL_OFFSET_X / 2 + (Size_w + TOTAL_OFFSET_X) * x;
            cy = TOTAL_OFFSET_Y / 2 + (Size_h + TOTAL_OFFSET_Y) * y;

            switch (d[i].type)
            {
                case TYPE_UNUSED:  // Not used device
                break;
                case TYPE_TEMPERATURE:
                case TYPE_HUMIDITY:
                case TYPE_CONSUMPTION:
                case TYPE_POWER:
                case TYPE_SWITCH_SENSOR:
                case TYPE_LUX:
                case TYPE_METEO:
                case TYPE_VALUE_SENSOR:
                case TYPE_SETPOINT:
                case TYPE_AIR_QUALITY:
                {
                    Widget_sensor(panel, d[i].name, d[i].data, cx , cy , Size_w , Size_h, device_color, &d[i],icon);
                }
                break;
                case TYPE_UNKNOWN: // Unknown type
                case TYPE_SWITCH:
                case TYPE_SPEAKER:
                case TYPE_SELECTOR:
                case TYPE_DIMMER:
                case TYPE_PLUG:
                case TYPE_COLOR:
                case TYPE_LIGHT:
                case TYPE_BLINDS:
                case TYPE_PUSH:
                case TYPE_WARNING: // This one is a sensor, but too much text to be displayed on homepage
                case TYPE_TEXT: // This one is a sensor, but too much text to be displayed on homepage
                {
                    Widget_button(panel, d[i].name, cx, cy, Size_w , Size_h, device_color, &d[i], icon); 
                }
                break;
                default:
                    Serial.printf("Undefined widget for HomePage: %d\n", d[i].type);
                break;
            }

            i = i + 1;
        }
    }

}

struct _Group
{
    bool scene;
    bool on;
};

void group_panel_init(lv_obj_t* panel)
{
    //Hide scrollbar
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_OFF);

    JsonDocument doc;

#ifdef OLD_DOMOTICZ
    if (!HTTPGETRequestWithReturn("/json.htm?type=scenes", &doc)) return;
#else
    if (!HTTPGETRequestWithReturn("/json.htm?type=command&param=getscenes", &doc)) return;
#endif

    JsonArray JS;
    JS = doc["result"];

    if (JS.isNull())
    {
        Serial.println(F("Json not available\n"));
        return;
    }
    
    short x,y = 0;
    short cx,cy;
    int idx = 0;
    const char *name;
    const char *type;
    const char *status = nullptr;
    lv_color_t device_color;
    
    for (auto i : JS)
    {
        cx = TOTAL_OFFSET_X / 2 + (Size_w + TOTAL_OFFSET_X) * x;
        cy = TOTAL_OFFSET_Y / 2 + (Size_h + TOTAL_OFFSET_Y) * y;

        name = i["Name"];
        //if (i.containsKey("idx")) idx = atoi(i["idx"]);
        if (i["idx"].is<const char*>()) idx = atoi(i["idx"]);

        device_color = lv_color_make(0xFF, 0x2F, 0x2F);
        if (strcmp(i["Type"], "Group") == 0) device_color = lv_color_make(0x2F, 0x2F, 0xFF);

        Widget_button_group(panel, (char*)name, cx, cy, Size_w , Size_h, device_color, idx, Geticon(TYPE_LIGHT), strcmp(i["Status"], "On") == 0,  strcmp(i["Type"], "Group") == 0);

        x += 1;
        if (x >= TOTAL_ICONX)
        {
            x = 0;
            y += 1;
        }
        if (y >= TOTAL_ICONY) break;
    }

}

void Update_scene_data(void)
{
#ifndef NO_GROUP_PAGE
    RefreshScenePage();
#endif
}