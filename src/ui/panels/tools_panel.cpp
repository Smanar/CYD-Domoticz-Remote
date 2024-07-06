#include <HardwareSerial.h>
#include <ArduinoJson.h>
//#include "esp32/himem.h"

#include "lvgl.h"
#include "panel.h"

#include "../../core/data_setup.h"
#include "../../conf/global_config.h"
#include "../../core/ip_engine.h"
#include "../../core/ota.h"
#include "../src/ui/navigation.h"

static lv_style_t style_container;
extern lv_style_t style_shadow;

lv_obj_t * label_tool;

static void tools_btn_event_handler(lv_event_t * e)
{
    //const lv_obj_t * ta = lv_event_get_target(e);
    //const lv_obj_t *label = lv_obj_get_child(ta, 0);
    int b = (int)lv_event_get_user_data(e);

    if (b == 1) navigation_screen(SETTING_PANEL);
    if (b == 2) ESP.restart();
    if (b == 3) OTAUpdate();

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

    // Setting button
    obj= lv_btn_create(cont1);
    lv_obj_set_size(obj, LV_PCT(30), 40);
    lv_obj_align(obj, LV_ALIGN_RIGHT_MID, 0, 0);
    label = lv_label_create(obj);
    lv_label_set_text_static(label, "Settings");
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, tools_btn_event_handler, LV_EVENT_CLICKED, (void *)1);
    // Reboot button
    obj= lv_btn_create(cont1);
    lv_obj_set_size(obj, LV_PCT(30), 40);
    lv_obj_align(obj, LV_ALIGN_LEFT_MID, 0, 0);
    label = lv_label_create(obj);
    lv_label_set_text_static(label, "Reboot");
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, tools_btn_event_handler, LV_EVENT_CLICKED, (void *)2);
    // OTA button
    obj= lv_btn_create(cont1);
    lv_obj_set_size(obj, LV_PCT(30), 40);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    label = lv_label_create(obj);
    lv_label_set_text_static(label, "OTA Updt");
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, tools_btn_event_handler, LV_EVENT_CLICKED, (void *)3);

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
    // esp_get_free_heap_size() ???

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    uint32_t used_size = mon.total_size - mon.free_size;

    char Text[181];
    lv_snprintf(Text, 180, "+ HEAP Memory Usable (Kb) %d, Max %d, Total %d\n", ESP.getMaxAllocHeap()/1024, ESP.getFreeHeap()/1024, ESP.getHeapSize()/1024);
    //lv_snprintf(Text + strlen(Text),180, "+ PSRAM Memory Free (Kb) %d, Total %d\n", ESP.getFreePsram()/1024, ESP.getPsramSize()/1024); // Not used, CRASH
    lv_snprintf(Text + strlen(Text), 180, "+ LV Heap %d kB used (%d %%) %d%% frag.\n", used_size / 1024, mon.used_pct, mon.frag_pct);
    //lv_snprintf(Text + strlen(Text), 180, "Spiram size (Kb) %d , himem free %d\n", esp_spiram_get_size()/1000, esp_himem_get_free_size()/1000); // Not used, CRASH
    lv_snprintf(Text + strlen(Text), 180, "+ Application Version : %d\n", APPLICATION_VERSION);
    lv_snprintf(Text + strlen(Text), 180, "+ Running time : %d:%d:%d:%d\n", runningTime()/(3600*24), (runningTime()/3600)%24 , (runningTime()/60)%60, runningTime()%60);
    lv_snprintf(Text + strlen(Text), 180, "+ Total data by WS : %d ko", total_WS_lenght());

    label_tool = lv_label_create(cont2);
    lv_obj_set_style_text_font(label_tool, &font3, 0);
    lv_label_set_long_mode(label_tool, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label_tool, Text);
    lv_obj_set_size(label_tool, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_text_align(label_tool, LV_TEXT_ALIGN_LEFT, 0);

}

