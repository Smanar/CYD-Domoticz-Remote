#include <HardwareSerial.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
//#include "esp32/himem.h"

#include "lvgl.h"
#include "panel.h"
#include "../../core/data_setup.h"
#include "../../conf/global_config.h"
#include "../../core/ip_engine.h"
#include "../src/ui/navigation.h"

static lv_style_t style_container;
extern lv_style_t style_shadow;

static void tools_btn_event_handler(lv_event_t * e)
{
    //const lv_obj_t * ta = lv_event_get_target(e);
    //const lv_obj_t *label = lv_obj_get_child(ta, 0);
    int b = (int)lv_event_get_user_data(e);

    if (b == 1)
    {
        navigation_screen(SETTING_PANEL);
    }
    if (b == 2) ESP.restart();

}

void tools_panel_init(lv_obj_t* panel)
{

    //First part
    lv_obj_t * obj;
    lv_obj_t * label;

    lv_obj_t * cont1 = lv_obj_create(panel);
    //lv_obj_set_size(cont, lv_pct(90), lv_pct(90));
    lv_obj_set_size(cont1, LV_PCT(90), LV_PCT(25));
    //lv_obj_set_pos(cont1, LV_PCT(5), LV_PCT(5));
    lv_obj_align(cont1, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_add_style(cont1, &style_shadow, LV_PART_MAIN);
    lv_obj_clear_flag( cont1, LV_OBJ_FLAG_SCROLLABLE );

    obj= lv_btn_create(cont1);
    lv_obj_set_size(obj, LV_PCT(30), 40);
    lv_obj_align(obj, LV_ALIGN_RIGHT_MID, 0, 0);
    label = lv_label_create(obj);
    lv_label_set_text_static(label, "Settings");
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, tools_btn_event_handler, LV_EVENT_CLICKED, (void *)1);
    obj= lv_btn_create(cont1);
    lv_obj_set_size(obj, LV_PCT(30), 40);
    lv_obj_align(obj, LV_ALIGN_LEFT_MID, 0, 0);
    label = lv_label_create(obj);
    lv_label_set_text_static(label, "Reboot");
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, tools_btn_event_handler, LV_EVENT_CLICKED, (void *)2);


    //Second part
    lv_obj_t * cont2 = lv_obj_create(panel);
    //lv_obj_set_size(cont, lv_pct(90), lv_pct(90));
    lv_obj_set_size(cont2, LV_PCT(90), LV_PCT(60));
    //lv_obj_set_pos(cont1, LV_PCT(5), LV_PCT(35));
    lv_obj_align(cont2, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_style(cont2, &style_shadow, LV_PART_MAIN);
    lv_obj_clear_flag( cont2, LV_OBJ_FLAG_SCROLLABLE );

    // The "4MB" in NodeMCU refers to the size of flash, the size of RAM on ESP32 is fixed at 512KB
    // roughly 200KB of which is used by IRAM cache/code sections, leaving around 320KB for program memory, half of which is available for dynamic allocation.
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/mem_alloc.html#:%7E:text=Due%20to%20a%20technical%20limitation,allocated%20at%20runtime%20as%20heap.

    label = lv_label_create(cont2);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text_fmt(label, "Heap Memory Usable (Kb) %d , Max %d, Total %d\n", ESP.getMaxAllocHeap()/1024, ESP.getFreeHeap()/1024, ESP.getHeapSize()/1024);
    lv_obj_set_size(label, LV_PCT(100), 40);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    //lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label2 = lv_label_create(cont2);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_label_set_text_fmt(label2, "RAM Memory Free (Kb) %d , Total %d\n", ESP.getFreePsram()/1024, ESP.getPsramSize()/1024);
    lv_obj_set_size(label2, LV_PCT(100), 40);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align_to(label2, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

/*
    // Not used, carsh the esp
    lv_obj_t *label3 = lv_label_create(cont2);
    lv_label_set_long_mode(label3, LV_LABEL_LONG_WRAP);
    lv_label_set_text_fmt(label3, "Spiram size (Kb) %d , himem free %d\n", esp_spiram_get_size()/1000, esp_himem_get_free_size()/1000);
    lv_obj_set_size(label3, LV_PCT(100), 40);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align_to(label3, label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
*/
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    uint32_t used_size = mon.total_size - mon.free_size;;
    uint32_t used_kb = used_size / 1024;

    lv_obj_t *label3 = lv_label_create(cont2);
    lv_label_set_long_mode(label3, LV_LABEL_LONG_WRAP);
    lv_label_set_text_fmt(label3,"LV Heap %d kB used (%d %%) %d%% frag.", used_kb, mon.used_pct, mon.frag_pct);
    lv_obj_set_size(label3, LV_PCT(100), 40);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align_to(label3, label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

}
