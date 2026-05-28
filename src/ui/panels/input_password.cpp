#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "panel.h"
#include "../navigation.h"
#include "../../conf/global_config.h"

static lv_obj_t * pwd_main_cont = NULL;                             // Password Input Interface Container
static lv_obj_t * hint_label = NULL;                                // Password prompt box
static lv_obj_t * pwd_text_area = NULL;                             // Password Input Box
static int checkAdminPagePtr = 0;                                   // pagePtr for checkAdmin callback
static int checkAdminPagePrevious = 0; 
static unsigned long lastAdminRightGiven = 0;                       // Last time we set admin rights
static unsigned long lastAdminRightRefused = 0;                     // Last time we refused admin rights
#define ADMIN_RIGHTS_TIMEOUT 30000                                  // Admin rights are valid for this duration (ms)

static void validateAccess(void);
static void refuseAccess(void);
void password_keyboard_display(void);

static const char * keyboard_map[] = {
    "1","2", "3","\n",
    "4", "5", "6", "\n",
    "7", "8", "9" ,"\n",
    ".","0",LV_SYMBOL_BACKSPACE,""
};

static const lv_btnmatrix_ctrl_t keyboard_ctrl[] = {
    LV_BTNMATRIX_CTRL_NO_REPEAT, LV_BTNMATRIX_CTRL_NO_REPEAT, LV_BTNMATRIX_CTRL_NO_REPEAT,
    LV_BTNMATRIX_CTRL_NO_REPEAT, LV_BTNMATRIX_CTRL_NO_REPEAT, LV_BTNMATRIX_CTRL_NO_REPEAT,
    LV_BTNMATRIX_CTRL_NO_REPEAT, LV_BTNMATRIX_CTRL_NO_REPEAT, LV_BTNMATRIX_CTRL_NO_REPEAT,
    LV_BTNMATRIX_CTRL_NO_REPEAT, LV_BTNMATRIX_CTRL_NO_REPEAT, LV_BTNMATRIX_CTRL_NO_REPEAT,
};

// Password change event
static void password_change_event_callback(lv_event_t* event);

// Confirm button
static void confirm_btn_event_callback(lv_event_t* event);

// Cancel the button's event callback
static void cancel_btn_event_callback(lv_event_t* event);

// Display the keyboard input interface
void password_keyboard_display(lv_obj_t* panel){

    lv_obj_clean(panel); // Clear the page

#if 1
    // Current page canvas container
    pwd_main_cont = lv_obj_create(panel);                    // Based on the screen creates a container
    // Canvas style
    static lv_style_t main_cont_style;
    lv_style_reset(&main_cont_style);
    lv_style_init(&main_cont_style);                                // Initialization
    lv_style_set_radius(&main_cont_style, 0);                       // Setting the round curvature of the style
    lv_style_set_border_width(&main_cont_style, 0);                 // Set the border width
    lv_style_set_bg_opa(&main_cont_style, LV_OPA_50);               // Setting the transparency of the style background, opaque
    lv_style_set_bg_color(&main_cont_style, lv_color_make(0xea, 0xea, 0xea));
    lv_style_set_pad_all(&main_cont_style, 0);                      // Set the internally filled
    lv_obj_add_style(pwd_main_cont, &main_cont_style, 0);           // Add a style to the object
    lv_obj_set_size(pwd_main_cont, LV_PCT(100), LV_PCT(100));       // Setting size to max
    lv_obj_center(pwd_main_cont);                                   // Object in the middle of the screen display
#endif

    // Keyboard background container
    lv_obj_t * pwd_bg_cont = lv_obj_create(pwd_main_cont);
    // Keyboard background pattern
    lv_obj_set_size(pwd_bg_cont, LV_PCT(100), LV_PCT(100));         // Setting size
    lv_obj_clear_flag( pwd_bg_cont, LV_OBJ_FLAG_SCROLLABLE );       // Remove scrollbar
    lv_obj_align(pwd_bg_cont, LV_ALIGN_TOP_MID, 0, 0);              // Top Case

    // Create a keyboard title object based on the keyboard background object
    lv_obj_t *title_label = lv_label_create(pwd_bg_cont);
    lv_label_set_long_mode(title_label, LV_LABEL_LONG_SCROLL_CIRCULAR); // Set long text loop scrolling mode
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 0);                // Top Case
    lv_label_set_text(title_label, "Input password");                 // Setting the text content

    // Create a password box based on the keyboard background object
    pwd_text_area = lv_textarea_create(pwd_bg_cont);
    // Password box pattern
    #ifdef HIDE_PASSWORD
        lv_textarea_set_password_mode(pwd_text_area, true);         // Password Mode
    #endif
    lv_textarea_set_max_length(pwd_text_area, sizeof(global_config.protectionPassword)-1); // Set the maximum length of the entered text
    lv_obj_set_size(pwd_text_area, lv_pct(80), lv_pct(12));          // Settings Object Size
    lv_textarea_set_one_line(pwd_text_area, true);
    lv_obj_align_to(pwd_text_area, title_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_clear_flag(pwd_text_area, LV_OBJ_FLAG_SCROLLABLE );      // Remove scrollbar
    lv_textarea_set_text(pwd_text_area, "");                        // Text box

    // Create a password check tab based on the keyboard background object
    hint_label = lv_label_create(pwd_bg_cont);
    // Password prompt text style
    lv_obj_set_size(hint_label, lv_pct(80), lv_pct(8));             // Settings Object Size
    lv_obj_align_to(hint_label, pwd_text_area, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_style_text_align(hint_label , LV_TEXT_ALIGN_CENTER, 0); // Center text
    lv_label_set_text(hint_label, "");                              // Setting the text content

    // Create a keyboard object based on the keyboard background object
    lv_obj_t * pwd_keyboard = lv_keyboard_create(pwd_bg_cont);
    // Keyboard style
#if 1
    static lv_style_t pwd_kb_style;
    lv_style_reset(&pwd_kb_style);
    lv_style_init(&pwd_kb_style);                                   // Initialization Sample
    lv_style_set_radius(&pwd_kb_style, 5);                          // Setting the rounded curvature of the style
    lv_style_set_border_width(&pwd_kb_style, 0);                    // Set the border width
    lv_style_set_bg_color(&pwd_kb_style, lv_color_hex(0xF98E2D));
    lv_style_set_text_opa(&pwd_kb_style, LV_OPA_COVER);
    lv_style_set_text_font(&pwd_kb_style,& big_font_bold);          // Setting Font
    lv_obj_add_style(pwd_keyboard, &pwd_kb_style, 0);               // Add Style
#endif
    lv_obj_set_size(pwd_keyboard, lv_pct(90), lv_pct(50));          // Set the keyboard size
    lv_keyboard_set_mode(pwd_keyboard, LV_KEYBOARD_MODE_NUMBER);    // Set the keyboard mode as numeric keypad
    lv_keyboard_set_map(pwd_keyboard, LV_KEYBOARD_MODE_NUMBER, keyboard_map, keyboard_ctrl); // Set keyboard mapping
    lv_keyboard_set_textarea(pwd_keyboard, pwd_text_area);          // Keyboard Object and Text Box Bind
    lv_obj_add_event_cb(pwd_keyboard, password_change_event_callback, LV_EVENT_VALUE_CHANGED, NULL); // Add password change event
    //lv_obj_align(pwd_keyboard, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(pwd_keyboard, hint_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    // Create a confirmation button based on the keyboard background object
    lv_obj_t * confirm_btn = lv_btn_create(pwd_bg_cont);
    lv_obj_add_event_cb(confirm_btn, confirm_btn_event_callback, LV_EVENT_CLICKED, NULL); // Add Click events and event processing callback functions to the object
    //lv_obj_add_style(confirm_btn, &btn_style, 0);                 // Add button style
    lv_obj_set_size(confirm_btn, lv_pct(35), lv_pct(12));           // Set button object size
    lv_obj_align(confirm_btn, LV_ALIGN_TOP_MID, lv_pct(-20), lv_pct(92));   // Set button object location
    // Create a label based on the Confirm_BTN object
    lv_obj_t *confirm_btn_label = lv_label_create(confirm_btn);
    lv_label_set_text(confirm_btn_label, "Ok");                     // Setting the text content
    lv_obj_center(confirm_btn_label);                               // Center button

    // Create a cancel button based on the keyboard background object
    lv_obj_t * cancel_btn = lv_btn_create(pwd_bg_cont);
    lv_obj_add_event_cb(cancel_btn, cancel_btn_event_callback, LV_EVENT_CLICKED, NULL); // Add Click events and event processing callback functions to objects to objects
    //lv_obj_add_style(cancel_btn, &btn_style, 0);
    lv_obj_set_size(cancel_btn, lv_pct(35), lv_pct(12));            // Set button object size
    lv_obj_align(cancel_btn, LV_ALIGN_TOP_MID, lv_pct(20), lv_pct(92));
    // Create a label based on the Cancel object
    lv_obj_t *cancel_btn_label = lv_label_create(cancel_btn);
    lv_label_set_text(cancel_btn_label, "Cancel");                  // Setting the text content
    lv_obj_center(cancel_btn_label);
}

// Password change venet callback
static void password_change_event_callback(lv_event_t* event){
    if (strlen(lv_textarea_get_text(pwd_text_area)) == 1) {         // On first input char
        lv_label_set_text(hint_label, "");                          // Clear hint text
    }
}

// Confirm event callback
static void confirm_btn_event_callback(lv_event_t* event){
    lv_event_code_t code = lv_event_get_code(event);
    if(code == LV_EVENT_CLICKED){
        Serial.print("Ok button clicked\n");
        if(pwd_text_area != NULL){
            const char *pwd_txt = lv_textarea_get_text(pwd_text_area);
            if((pwd_txt != NULL)){
                if(strcmp(pwd_txt, global_config.protectionPassword) == 0){
                    if(hint_label != NULL){
                        lv_label_set_text(hint_label, "Password correct!");
                    }
                    lv_obj_del(pwd_main_cont);                      // Delete me

                    validateAccess();                               // Activate callback
                    navigation_screen(checkAdminPagePtr);

                } else if(pwd_txt[0] == '\0') {
                    if(hint_label != NULL){
                        lv_label_set_text(hint_label, "Password can not be empty!");
                    }
                } else {
                    if(hint_label != NULL){
                        lv_label_set_text(hint_label, "Password error!");
                    }
                    if(pwd_text_area != NULL){
                        lv_textarea_set_text(pwd_text_area, "");    // Clear text box
                    }
                }
            }
        }
    }
}

// Cancel event callback
static void cancel_btn_event_callback(lv_event_t* event){
    lv_event_code_t code = lv_event_get_code(event);
    if(code == LV_EVENT_CLICKED){
        Serial.print("Cancel button clicked\n");
        lv_obj_del(pwd_main_cont);                                  // Delete me

        refuseAccess();                                             // Refuse callback
        navigation_screen(checkAdminPagePrevious);
    }
}

// Routine to display password keyboard and get admin rights
int checkAdminRights(const int pagePtr, const int DefaultPage)
{
    checkAdminPagePtr = pagePtr;                                    // Page pointer to send to routine
    checkAdminPagePrevious = DefaultPage;

    #if (1)
    Serial.printf("checkAdminRights: page %d, protected %d, given %lu, refused %lu\n", 
        pagePtr, isPageProtected(pagePtr), 
        (millis() - lastAdminRightGiven) * (lastAdminRightGiven? 1 : 0) / 1000,
        (millis() - lastAdminRightRefused) * (lastAdminRightRefused? 1 : 0) / 1000);
    #endif

    if (global_config.protectionPassword[0]                         // Is admin password defined?
            && isPageProtected(pagePtr))                            // ... and page protected?
    {
        if (lastAdminRightGiven && ((millis() - lastAdminRightGiven) < ADMIN_RIGHTS_TIMEOUT)) { // If right given recently
            return pagePtr;
        } else if (!lastAdminRightRefused || ((millis() - lastAdminRightRefused) >= ADMIN_RIGHTS_TIMEOUT)) { // Unless rights refused recently
            return PASS_PANEL;
        }
        return DefaultPage;
    }
    else
    {                                                        // No protection password given
        return pagePtr;
    }
}

// Routine called when access is validated
void validateAccess(void) {
    lastAdminRightGiven = millis();                                 // Last time we gave admin rights
    lastAdminRightRefused = 0;                                      // Last time we refused admin rights
    Serial.printf("Access to page %d granted\n",checkAdminPagePtr);
}

// Routine called when access is refused
void refuseAccess(void) {
    lastAdminRightGiven = 0;                                        // Last time we gave admin rights
    lastAdminRightRefused = millis();                               // Last time we refused admin rights
    Serial.printf("Access to page %d refused\n",checkAdminPagePtr);
}