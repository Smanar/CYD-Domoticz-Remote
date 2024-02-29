#include <lvgl.h>
#include "panel.h"
#include "../../core/data_setup.h"
#include "../../conf/global_config.h"
#include "../main_ui.h"

#include <HardwareSerial.h>
#include <HTTPClient.h>

extern Device myDevices[];
extern lv_style_t style_shadow;

//Calculate values to use to display the homepage (even number)
#define TOTAL_OFFSET_X 10
#define TOTAL_OFFSET_Y 10
//Calculate widgets size
int Size_w = int(TFT_HEIGHT/TOTAL_ICONX) -  TOTAL_OFFSET_X;
int Size_h = int(TFT_WIDTH/TOTAL_ICONY) - TOTAL_OFFSET_Y;
//Icon size
int Size_icon = 35;// Size_h/2; // Fixed for the moment


static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t * btn = lv_event_get_target(e);
    //int *d = (int*)lv_event_get_param(e);

    int btn_no = (int)lv_event_get_user_data(e); // cast the return pointer to data type pointer

    if(code == LV_EVENT_CLICKED) {
        //Serial.printf("Clic sur boutton: %d", * btn_no);
        Select_deviceHP(btn_no);
    }
}

static void Widget_button(lv_obj_t* panel, char* desc, int x, int y, int w, int h, lv_color_t color, int d, const lv_img_dsc_t* icon)
{

    /*Create a container with ROW flex direction*/
    lv_obj_t * Button_icon = lv_obj_create(panel);
    lv_obj_set_size(Button_icon, w, h);
    //lv_obj_align(Button_icon, LV_ALIGN_CENTER, x, y);
    lv_obj_set_pos(Button_icon, x, y);
    lv_obj_add_style(Button_icon, &style_shadow, LV_PART_MAIN);
    //lv_obj_set_drag(Button_icon, false);
    //lv_obj_set_flex_flow(Button_icon, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag( Button_icon, LV_OBJ_FLAG_SCROLLABLE );                     // Remove scrollbar
    lv_obj_add_event_cb(Button_icon, btn_event_cb, LV_EVENT_ALL, (void *)d);              /*Assign a callback to the button*/

    lv_obj_t *img = lv_img_create(Button_icon);
    //lv_img_set_src(img, LV_SYMBOL_OK "Accept");
    lv_img_set_src(img, icon);
    //lv_img_set_zoom(img,512); // Work only for 256 color image or any non True color
    lv_obj_align(img, LV_ALIGN_TOP_MID , 0, -10);
    lv_obj_set_size(img, Size_icon, Size_icon);
    lv_obj_set_style_img_recolor_opa(img, 50, 0);
    lv_obj_set_style_img_recolor(img, color, 0);

    /*Create description*/
    lv_obj_t * label2 = lv_label_create(Button_icon);               /*Add a label to the button*/
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);             /*Break the long lines*/
    lv_obj_set_style_text_font(label2, &font2, 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    //lv_label_set_recolor(label2, true);                           /*Activate coloring*/
    lv_label_set_text(label2, desc);                                /*Set the labels text*/
    //lv_obj_center(label2);
    lv_obj_set_size(label2, Size_w-10, 30); 
#if TOTAL_ICONX == 3
    lv_obj_align_to(label2, img,  LV_ALIGN_OUT_BOTTOM_MID, 0, 5);                        // Need to use absolute position
#else
    lv_obj_align_to(label2, img,  LV_ALIGN_OUT_BOTTOM_MID, 0, 20);                         // Need to use absolute position
#endif

}

static void Widget_sensor(lv_obj_t* panel, char* desc, char* value, int x, int y, int w, int h, lv_color_t color, int d, const lv_img_dsc_t *icon)
{

    /*Create a container*/
    lv_obj_t * Button_icon = lv_obj_create(panel);
    lv_obj_set_size(Button_icon, w, h);
    //lv_obj_align(Button_icon, LV_ALIGN_CENTER, x, y);
    lv_obj_set_pos(Button_icon, x, y);
    lv_obj_add_style(Button_icon, &style_shadow, LV_PART_MAIN);
    lv_obj_clear_flag( Button_icon, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_add_event_cb(Button_icon, btn_event_cb, LV_EVENT_ALL, (void *)d);              /*Assign a callback to the button*/
    //lv_obj_add_event_cb(Button_icon,button_draw_event_cb,LV_EVENT_DRAW_PART_END,NULL); // To bypass drawing

    /*Create icon*/
    lv_obj_t *img = lv_img_create(Button_icon);
    //lv_img_set_src(img, LV_SYMBOL_OK "Accept");
    lv_img_set_src(img, icon);
    //lv_img_set_zoom(img,200);                         // Not working for this image type
    lv_obj_align(img, LV_ALIGN_TOP_LEFT , -10, -10);
    lv_obj_set_size(img, Size_icon, Size_icon);
    lv_obj_set_style_img_recolor_opa(img, 50, 0);
    lv_obj_set_style_img_recolor(img, color, 0);

     /*Create Value*/
     // Does _local_ is working here ?
    lv_obj_t * label = lv_label_create(Button_icon);
    lv_obj_set_style_text_font(label, &font1, 0);
    lv_label_set_text(label, value);
#if TOTAL_ICONX == 3
    lv_obj_align_to(label, img,  LV_ALIGN_OUT_RIGHT_MID, 0, 0); //Need to use absolute method
#else
    lv_obj_align_to(label, img,  LV_ALIGN_OUT_RIGHT_MID, 20, 0); //Need to use absolute method
#endif
    lv_obj_set_style_text_color(label, color, 0);

    /*Create description*/
    lv_obj_t * label2 = lv_label_create(Button_icon);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(label2, &font2, 0);
    lv_label_set_text(label2, desc);
    lv_obj_set_size(label2, Size_w-10, 30);
    //lv_obj_align_to(label2, img,  LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_align(label2, LV_ALIGN_BOTTOM_MID, 0, 20);
}

void home_panel_init(lv_obj_t* panel)
{
    int x,y;
    int i = 0;

    for (x=0; x<TOTAL_ICONX; x=x+1)
    {
        for (y=0; y<TOTAL_ICONY; y=y+1)
        {
            const lv_color_t device_color = Getcolor(myDevices[i].type);
            const lv_img_dsc_t *icon = Geticon(myDevices[i].type);

            int cx = TOTAL_OFFSET_X / 2 + (Size_w + TOTAL_OFFSET_X) * x;
            int cy = TOTAL_OFFSET_Y / 2 + (Size_h + TOTAL_OFFSET_Y) * y;

            switch (myDevices[i].type)
            {
                case TYPE_UNKNOWN: // Unknown type
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
                {
                    Widget_sensor(panel, myDevices[i].name, myDevices[i].data, cx , cy , Size_w , Size_h, device_color, i,icon);
                }
                break;
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
                    Widget_button(panel, myDevices[i].name, cx, cy, Size_w , Size_h, device_color, i, icon); 
                }
                break;
                default:
                    Serial.printf("Undefinied widget for HomePage: %d\n", myDevices[i].type);
                break;
            }

            i = i + 1;
        }
    }

}