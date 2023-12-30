#include <lvgl.h>
#include "panel.h"
#include "../../core/data_setup.h"
#include "../../conf/global_config.h"

#include <HardwareSerial.h>
#include <HTTPClient.h>

LV_FONT_DECLARE(Montserrat_Bold_14)

extern Device myDevices[];

void Widget_Simple_button(void);
void Widget_icon(char* data, char* desc);
void Widget_button(char* desc, int x, int y, int w, int h, int *d, const lv_img_dsc_t *i);
void Widget_sensor(char* desc, char* value, int x, int y, int w, int h, const lv_img_dsc_t *i);

// My Resolution is 240x320
#define TOTAL_WIDTH 320
#define TOTAL_HEIGHT 240
#define TOTAL_ICONX 3
#define TOTAL_ICONY 3
#define TOTAL_OFFSET_X 10
#define TOTAL_OFFSET_Y 10
#define MENU_WIDTH 40

//Calculate widgets size, 3 * 3
int Size_w = int((TOTAL_WIDTH - MENU_WIDTH) / TOTAL_ICONX) -  TOTAL_OFFSET_X;
int Size_h = int(TOTAL_HEIGHT / TOTAL_ICONY) - TOTAL_OFFSET_Y;
//Icon size
int Size_icon = Size_h / 2;

#if 0
void Widget_icon(char* data, char* desc)
{

    static lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow,  10);
    lv_style_set_shadow_spread(&style_shadow,  5);
    lv_style_set_shadow_color(&style_shadow,  LV_COLOR_MAKE(0x00, 0x00, 0xFF));

    /*Create a container with ROW flex direction*/
    lv_obj_t * Button_icon = lv_obj_create(lv_scr_act());
    lv_obj_set_size(Button_icon, 200, 100);
    lv_obj_align(Button_icon, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(Button_icon, &style_shadow, LV_PART_MAIN);
    //lv_obj_set_drag(obj2, false);
    //lv_obj_set_flex_flow(Button_icon, LV_FLEX_FLOW_ROW);

    //lv_obj_t * btn = lv_btn_create(Button_icon);               /*Add a button the current screen*/
    //lv_obj_set_pos(btn, 10, 10);                               /*Set its position*/
    //lv_obj_set_size(btn, 120, 50);                              /*Set its size*/
    //lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/
    //lv_obj_align(btn, LV_ALIGN_IN_TOP_MID, 0, 0);

    lv_obj_t *img = lv_img_create(Button_icon);
    lv_img_set_src(img, LV_SYMBOL_OK "Accept");
    //lv_obj_align(img, LV_ALIGN_TOP_RIGHT , 0, 0);
    lv_obj_set_size(img, 30, 30);

    lv_obj_t * label = lv_label_create(Button_icon);              /*Add a label to the button*/
    lv_label_set_text(label, data);                           /*Set the labels text*/
    //lv_obj_center(label);
    lv_obj_align_to(label, img,  LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    lv_obj_t * label2 = lv_label_create(Button_icon);               /*Add a label to the button*/
    lv_label_set_text(label2, desc);                        /*Set the labels text*/
    //lv_obj_center(label2);
    lv_obj_align_to(label2, Button_icon,  LV_ALIGN_CENTER, 0, 0);

}
#endif

#if 0
//https://forum.lvgl.io/t/how-to-add-images-on-a-button-at-the-same-time-you-can-add-text/1022/3
lv_obj_t * lv_icon_create(lv_obj_t*par, const void* img_src, const char* txt)
{
    
    lv_obj_t* icon = lv_btn_create( par);
    lv_cont_set_layout(icon, LV_LAYOUT_COL_M);
    lv_cont_set_fit2(icon, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0,0);
    lv_obj_set_auto_realign(icon, true);

    static lv_style_t style_rel;  lv_style_copy(&style_rel, &lv_style_transp_tight);
      lv_btn_set_style(icon, LV_BTN_STYLE_REL, &style_rel);
      style_rel.body.padding.top    = 10;
      style_rel.body.padding.left   = 10;
      style_rel.body.padding.right  = 10;
      
    static lv_style_t style_pr;   lv_style_copy(&style_pr, &style_rel);
      lv_btn_set_style(icon, LV_BTN_STYLE_PR, &style_pr);
      style_pr.body.opa           = 128;
      style_pr.body.radius        = 0;
      style_pr.body.border.width  = 3;
      style_pr.body.border.color  = LV_COLOR_WHITE;
      style_pr.body.border.opa    = 128;
      style_pr.text.color= LV_COLOR_WHITE;
    
    lv_obj_t *img = lv_img_create(icon, NULL);
      lv_img_set_src(img, img_src);
    lv_obj_t *label = lv_label_create(icon,NULL);
      lv_label_set_text(label, txt);
      lv_obj_set_style(label, &style_pr);

  return icon;
}
#endif


static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    //int *d = (int*)lv_event_get_param(e);

    int * btn_no; // create a user data type pointer
    btn_no = (int*)lv_event_get_user_data(e); // cast the return pointer to data type pointer


    if(code == LV_EVENT_CLICKED) {
        //Serial.printf("Clic sur boutton: %d", * btn_no);
        Select_device(*btn_no);
    }
}

#if 0
static void button_draw_event_cb(lv_event_t* e)
{

    lv_obj_draw_part_dsc_t * part_dsc = lv_event_get_draw_part_dsc(e);
    if(part_dsc->class_p == &lv_obj_class &&
            (part_dsc->part == LV_OBJ_DRAW_PART_RECTANGLE || part_dsc->part == LV_OBJ_DRAW_PART_BORDER_POST))
    {
        lv_obj_t * obj = lv_event_get_target(e);
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_opa = 0;
        draw_dsc.border_width = 3;//lv_obj_get_style_border_width(obj, 0);
        draw_dsc.border_color = LV_COLOR_MAKE(lv_rand(0,0xff), lv_rand(0,0xff), lv_rand(0,0xff));
        //draw_dsc.border_side = LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT;
        lv_draw_rect(part_dsc->draw_ctx, &draw_dsc, part_dsc->draw_area);
    }
}
#endif
/**
 * Create a button with a label and react on click event.
 */
void Widget_Simple_button(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act());                         /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                                          /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                                        /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);                              /*Add a label to the button*/      
    lv_label_set_text(label, "Button");                                   /*Set the labels text*/
    lv_obj_center(label);
}

void Widget_button(char* desc, int x, int y, int w, int h, lv_color_t color, int *d, const lv_img_dsc_t* icon)
{

    static lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow,  10);
    lv_style_set_shadow_spread(&style_shadow,  5);
    lv_style_set_shadow_color(&style_shadow,  color);

    /*Create a container with ROW flex direction*/
    lv_obj_t * Button_icon = lv_obj_create(lv_scr_act());
    lv_obj_set_size(Button_icon, w, h);
    //lv_obj_align(Button_icon, LV_ALIGN_CENTER, x, y);
    lv_obj_set_pos(Button_icon, x, y);
    lv_obj_add_style(Button_icon, &style_shadow, LV_PART_MAIN);
    //lv_obj_set_drag(Button_icon, false);
    //lv_obj_set_flex_flow(Button_icon, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag( Button_icon, LV_OBJ_FLAG_SCROLLABLE );                     // Remove scrollbar
    lv_obj_add_event_cb(Button_icon, btn_event_cb, LV_EVENT_ALL, d);              /*Assign a callback to the button*/

    lv_obj_t *img = lv_img_create(Button_icon);
    //lv_img_set_src(img, LV_SYMBOL_OK "Accept");
    lv_img_set_src(img, icon);
    lv_obj_align(img, LV_ALIGN_TOP_MID , 0, -10);
    lv_obj_set_size(img, Size_icon, Size_icon);
    lv_obj_set_style_img_recolor_opa(img, 50, 0);
    lv_obj_set_style_img_recolor(img, color, 0);

    /*Create description*/
    lv_obj_t * label2 = lv_label_create(Button_icon);               /*Add a label to the button*/
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_10, 0);
    //lv_label_set_recolor(label2, true);                           /*Activate coloring*/
    lv_label_set_text(label2, desc);                                /*Set the labels text*/
    //lv_obj_center(label2);
    lv_obj_set_size(label2, Size_w-10, 30);
    lv_obj_align_to(label2, img,  LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

}

void Widget_sensor(char* desc, char* value, int x, int y, int w, int h, lv_color_t color, const lv_img_dsc_t *icon)
{

    static lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow,  10);
    lv_style_set_shadow_spread(&style_shadow,  5);
    lv_style_set_shadow_color( &style_shadow,  color);


    /*Create a container*/
    lv_obj_t * Button_icon = lv_obj_create(lv_scr_act());
    lv_obj_set_size(Button_icon, w, h);
    //lv_obj_align(Button_icon, LV_ALIGN_CENTER, x, y);
    lv_obj_set_pos(Button_icon, x, y);
    lv_obj_add_style(Button_icon, &style_shadow, LV_PART_MAIN);
    lv_obj_clear_flag( Button_icon, LV_OBJ_FLAG_SCROLLABLE );

    //lv_obj_add_event_cb(Button_icon,button_draw_event_cb,LV_EVENT_DRAW_PART_END,NULL); // To bypass drawing
    lv_obj_set_style_shadow_color(Button_icon, color, LV_PART_MAIN); // Because styles are global

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

    // Set a new style
    //lv_style_set_text_font(&nav_button_text_style, &lv_font_montserrat_12);
    lv_obj_set_style_text_font(label, &Montserrat_Bold_14, 0);
    lv_label_set_text(label, value);
    lv_obj_align_to(label, img,  LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_color(label, color, 0);

    /*Create description*/
    lv_obj_t * label2 = lv_label_create(Button_icon);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_10, 0);
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
            //Serial.printf("set widget x: %i , y : %i\n", MENU_WIDTH + (Size_w + TOTAL_OFFSET_X) * x, (Size_h + TOTAL_OFFSET_Y) * y);
            lv_color_t device_color = Getcolor(myDevices[i].type);
            const lv_img_dsc_t *icon = Geticon(myDevices[i].type);


            switch (myDevices[i].type)
            {
                case TYPE_TEMPERATURE:
                {
                    Widget_sensor(myDevices[i].name, myDevices[i].data, MENU_WIDTH + TOTAL_OFFSET_X + (Size_w + TOTAL_OFFSET_X) * x -4, TOTAL_OFFSET_Y + (Size_h + TOTAL_OFFSET_Y) * y -4, Size_w , Size_h, device_color, icon);
                }
                break;
                case TYPE_WARNING:
                {
                    Widget_sensor(myDevices[i].name, myDevices[i].data, MENU_WIDTH + TOTAL_OFFSET_X + (Size_w + TOTAL_OFFSET_X) * x -4, TOTAL_OFFSET_Y + (Size_h + TOTAL_OFFSET_Y) * y -4, Size_w , Size_h, device_color, icon);
                }
                break;
                case TYPE_SWITCH:
                case TYPE_SPEAKER:
                case TYPE_SELECTOR:
                case TYPE_DIMMER:
                case TYPE_PLUG:
                {
                    myDevices[i].pointer = i;
                    Widget_button(myDevices[i].name, MENU_WIDTH + TOTAL_OFFSET_X + (Size_w + TOTAL_OFFSET_X) * x -4, TOTAL_OFFSET_Y + (Size_h + TOTAL_OFFSET_Y) * y -4, Size_w , Size_h, device_color, &myDevices[i].pointer, icon); 
                }
                break;
                default:
                break;
            }

            i = i + 1;
        }
    }

}