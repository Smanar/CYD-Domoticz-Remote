#include "main_ui.h"
#include "../core/data_setup.h"
#include "../conf/global_config.h"
#include "../core/screen_driver.h"
#include "lvgl.h"
#include "nav_buttons.h"


void check_if_screen_needs_to_be_disabled(){

}

static void on_state_change(void * s, lv_msg_t * m)
{
    check_if_screen_needs_to_be_disabled();
}


void main_ui_setup(){
    //lv_msg_subscribe(DATA_PRINTER_STATE, on_state_change, NULL);
    on_state_change(NULL, NULL);
}