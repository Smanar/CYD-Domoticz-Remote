#include <HardwareSerial.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "lvgl.h"
#include "panel.h"
#include "../../core/data_setup.h"
#include "../../conf/global_config.h"
#include "../../core/ip_engine.h"
#include "../src/ui/navigation.h"

static lv_style_t style_container;

#define MAXDEVICE 500
lv_obj_t * table;
static uint16_t MemIDX[MAXDEVICE];

static void TV_btn_event_handler(lv_event_t * e) {
    const lv_obj_t * ta = lv_event_get_target(e);
    const lv_obj_t *label = lv_obj_get_child(ta, 0);

    //lv_table_clear_cell_ctrl(table,)
    //lv_obj_clean(table);
    //uint16_t r = lv_table_get_row_cnt(table);

    JsonArray JS;
    char buff[256] = {};

#ifdef OLD_DOMOTICZ
    snprintf(buff, 256, "/json.htm?type=devices&filter=%s&used=true&order=Name",lv_label_get_text(label));
#else
    //snprintf(buff, 256, "/json.htm?type=command&param=getdevices&used=true&displayhidden=1"); // TO TEST, need to be removed
    snprintf(buff, 256, "/json.htm?type=command&param=getdevices&filter=%s&used=true&order=Name",lv_label_get_text(label));
#endif

    if (!HTTPGETRequestWithReturn(buff, &JS, true)) return;
    
    lv_table_set_row_cnt(table, JS.size()); // To prevent multiple memory re allocation

    int j = 1;
    for (auto i : JS)
    {
        if (j < MAXDEVICE)
        {
            lv_table_set_cell_value(table, j, 0, i["Name"]); // Fisr column 
            lv_table_set_cell_value(table, j, 1, i["Data"]); // Second column
            //lv_table_set_cell_user_data() // Not used in this LVLG version so need to use a table
            MemIDX[j] = atoi(i["idx"]);
            j+=1;
        }
    }

    lv_table_set_row_cnt(table, j);

}

void Init_Info_Style(void)
{
    lv_style_init(&style_container);
    lv_style_set_flex_flow(&style_container, LV_FLEX_FLOW_ROW_WRAP); // Place the children in a row with wrapping
    lv_style_set_flex_main_place(&style_container, LV_FLEX_ALIGN_SPACE_EVENLY); // items are distributed so that the spacing between any two items (and the space to the edges) is equal
    lv_style_set_layout(&style_container, LV_LAYOUT_FLEX); // Make Flex layout

    lv_style_set_pad_ver(&style_container, 2);
    lv_style_set_pad_left(&style_container, 2);
}

static void change_event_cb(lv_event_t * e)
{

    lv_obj_t * obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        uint16_t col;
        uint16_t row;
        lv_table_get_selected_cell(obj, &row, &col);
        if (row > 0)
        {
            //Serial.printf("Info on idx: %d\n", MemIDX[row]);
            Select_deviceIDX(MemIDX[row]);
         }

    }
}

static void draw_part_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    /*If the cells are drawn...*/
    if(dsc->part == LV_PART_ITEMS) {
        uint32_t row = dsc->id /  lv_table_get_col_cnt(obj);
        uint32_t col = dsc->id - row * lv_table_get_col_cnt(obj);

        /*Make the texts in the first cell center aligned*/
        if(row == 0) {
            dsc->label_dsc->align = LV_TEXT_ALIGN_CENTER;
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), dsc->rect_dsc->bg_color, LV_OPA_20);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
        /*In the first column align the texts to the right*/
        else if(col == 0) {
            dsc->label_dsc->align = LV_TEXT_ALIGN_LEFT;
        }

        /*MAke every 2nd row grayish*/
        if((row != 0 && row % 2) == 0) {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), dsc->rect_dsc->bg_color, LV_OPA_10);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
    }
}

void info_panel_init(lv_obj_t* panel)
{

    //container
    lv_obj_t * cont = lv_obj_create(panel);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    //lv_obj_center(cont);
    lv_obj_add_style(cont, &style_container, LV_PART_MAIN);
    //lv_obj_clear_flag( cont, LV_OBJ_FLAG_SCROLLABLE );

    //add 5 buttons
    lv_obj_t * obj;
    lv_obj_t * label;

    obj= lv_btn_create(cont);
    lv_obj_set_size(obj, LV_PCT(20), 20);
    label = lv_label_create(obj);
    lv_label_set_text_static(label, "light");
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, TV_btn_event_handler, LV_EVENT_CLICKED, NULL);
    obj= lv_btn_create(cont);
    lv_obj_set_size(obj, LV_PCT(20), 20);
    label = lv_label_create(obj);
    lv_label_set_text_static(label, "temp");
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, TV_btn_event_handler, LV_EVENT_CLICKED, NULL);
    obj= lv_btn_create(cont);
    lv_obj_set_size(obj, LV_PCT(20), 20);
    label = lv_label_create(obj);
    lv_label_set_text_static(label, "utility");
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, TV_btn_event_handler, LV_EVENT_CLICKED, NULL);
    obj= lv_btn_create(cont);
    lv_obj_set_size(obj, LV_PCT(20), 20);
    label = lv_label_create(obj);
    lv_label_set_text_static(label, "baro");
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, TV_btn_event_handler, LV_EVENT_CLICKED, NULL);

    table = lv_table_create(cont);

    lv_obj_set_style_text_font(table, &font3, 0);
    lv_obj_set_style_pad_all(table, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_width(table, 1, LV_PART_ITEMS);
    lv_table_set_col_width(table, 1, TFT_WIDTH / 3+25);
    //lv_obj_set_size(table, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_table_set_col_width(table, 0, 2 * TFT_WIDTH / 3+25);

    lv_table_set_cell_value(table, 0, 0, "Name"); // Fisr column 
    lv_table_set_cell_value(table, 0, 1, "State"); // Second column

    /*Set a smaller height to the table. It'll make it scrollable*/
    //lv_obj_set_height(table, 200);
    //lv_obj_center(table);

    /*Add an event callback to to apply some custom drawing*/
    lv_obj_add_event_cb(table, draw_part_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

    lv_obj_add_event_cb(table, change_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    //erase table
    //lv_table_set_row_cnt(table, 1);

}
