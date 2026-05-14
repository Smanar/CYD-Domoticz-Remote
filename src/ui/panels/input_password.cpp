#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "panel.h"
#include "../navigation.h"
#include "../../conf/global_config.h"

static lv_obj_t * pwd_main_cont = NULL;                             // Password Input Interface Container
static lv_obj_t * hint_label = NULL;                                // Password prompt box
static lv_obj_t * pwd_text_area = NULL;                             // Password Input Box
static checkAdminCallback_t *checkAdminAllowed;                     // Routine callback address for checkAdmin if success
static checkAdminCallback_t *checkAdminRefused;                     // Routine callback address for checkAdmin if failure
static int checkAdminPagePtr= 0;                                    // pagePtr for checkAdmin callback
static unsigned long lastAdminRightGiven = 0;                       // Last time we set admin rights
static unsigned long lastAdminRightRefused = 0;                     // Last time we refused admin rights
#define ADMIN_RIGHTS_TIMEOUT 30000                                  // Admin wirghts are valid for this duration (ms)

static void runCallback(checkAdminCallback_t callback);

static void validateAccess(void);
static void refuseAccess(void);

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

// Confirm button
static void confirm_btn_event_callback(lv_event_t* event);

// Cancel the button's event callback
static void cancel_btn_event_callback(lv_event_t* event);

// Display the keyboard input interface
void password_keyboard_display(void){
    // Current page canvas container
    pwd_main_cont = lv_obj_create(lv_scr_act());                    // Based on the screen creates a container
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

    // Keyboard background container
    lv_obj_t * pwd_bg_cont = lv_obj_create(pwd_main_cont);
    // Keyboard background pattern
    static lv_style_t bg_style;
    lv_style_reset(&bg_style);
    lv_style_init(&bg_style);                                       // Initialization
    lv_style_set_radius(&bg_style, 10);                             // Set the rounded radians of the style
    lv_style_set_border_width(&bg_style, 0);                        // Set the border width
    lv_style_set_bg_opa(&bg_style, LV_OPA_COVER);                   // Setting the transparency of the style background, opaque
    lv_style_set_bg_color(&bg_style, lv_palette_main(LV_PALETTE_BLUE)); // Background Color Setting to Blue
    lv_style_set_pad_all(&bg_style, 0);                             // Set the internally fill
    // Style add gradient
    lv_style_set_bg_grad_color(&bg_style, lv_color_white());        // Gradient background color is white
    lv_style_set_bg_grad_dir(&bg_style, LV_GRAD_DIR_VER);           // Vertical direction gradient
    lv_style_set_bg_main_stop(&bg_style, 30);                       // Main Stop Color 30
    lv_style_set_bg_grad_stop(&bg_style, 30);                       // Gradient stop color 30
    lv_obj_add_style(pwd_bg_cont, &bg_style, 0);                    // Add a style to the object
    lv_obj_set_size(pwd_bg_cont, 353, 415);                         // Setting size
    lv_obj_align(pwd_bg_cont, LV_ALIGN_TOP_MID, 0, 33);             // Top Case

    // Keyboard Title Style
    static lv_style_t title_label_style;
    lv_style_reset(&title_label_style);
    lv_style_init(&title_label_style);                              // Initialization Sample
    lv_style_set_radius(&title_label_style, 0);                     // Setting the round curvature of the style
    lv_style_set_border_width(&title_label_style, 0);               // Set the border width
    lv_style_set_text_color(&title_label_style, lv_color_white());  // Font Color Settings to White
    lv_style_set_text_font(&title_label_style, &medium_font);       // Setting Font

    // Create a keyboard title object based on the keyboard background object
    lv_obj_t *title_label = lv_label_create(pwd_bg_cont);
    lv_label_set_long_mode(title_label, LV_LABEL_LONG_SCROLL_CIRCULAR); // Set long text loop scrolling mode
    lv_obj_add_style(title_label, &title_label_style, 0);           // Add a style to btn_label
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);             // Top Case
    lv_label_set_text(title_label, "Input password");               // Setting the text content

    // Create a password box based on the keyboard background object
    pwd_text_area = lv_textarea_create(pwd_bg_cont);
    // Password box pattern
    static lv_style_t pwd_text_style;
    lv_style_reset(&pwd_text_style);
    lv_style_init(&pwd_text_style);                                 // Initialization Sample
    lv_style_set_radius(&pwd_text_style, 0);                        // Setting the rounded curvature of the style
    lv_style_set_border_width(&pwd_text_style, 1);                  // Set the border width
    lv_style_set_pad_all(&pwd_text_style, 0);                       // Set the internal fill of the style
    lv_style_set_text_font(&pwd_text_style, &medium_font);          // Setting the font
    lv_textarea_set_text(pwd_text_area, "");                        // Text box
    lv_textarea_set_password_mode(pwd_text_area, true);             // Password Mode
    lv_textarea_set_max_length(pwd_text_area, sizeof(global_config.protectionPassword)-1); // Set the maximum length of the entered text
    lv_obj_add_style(pwd_text_area, &pwd_text_style, 0);            // Add a style to btn_label
    lv_obj_set_size(pwd_text_area, 295, 41);                        // Settings Object Size
    lv_obj_align_to(pwd_text_area, title_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 30); // Top Case

    // Create a password check tab based on the keyboard background object
    hint_label = lv_label_create(pwd_bg_cont);
    // Password prompt text style
    static lv_style_t hint_label_style;
    lv_style_reset(&hint_label_style);
    lv_style_init(&hint_label_style);                               // Initialization Sample
    lv_style_set_radius(&hint_label_style, 0);                      // Set the rounded radians of the style
    lv_style_set_border_width(&hint_label_style, 0);                // Set the border width
    lv_style_set_text_color(&hint_label_style, lv_palette_main(LV_PALETTE_RED)); // Font Color Set to Red
    lv_style_set_text_font(&hint_label_style, &medium_font);         // Setting Font
    lv_label_set_long_mode(hint_label, LV_LABEL_LONG_SCROLL_CIRCULAR); // Long text loop scrolling
    lv_obj_add_style(hint_label, &hint_label_style, 0);             // Add a style to btn_label
    lv_obj_align(hint_label, LV_ALIGN_TOP_MID, 0, 110);             // Top Case
    lv_label_set_text(hint_label, "");                              // Setting the text content

    // Create a keyboard object based on the keyboard background object
    lv_obj_t * pwd_keyboard = lv_keyboard_create(pwd_bg_cont);
    // Keyboard style
    static lv_style_t pwd_kb_style;
    lv_style_reset(&pwd_kb_style);
    lv_style_init(&pwd_kb_style);                                   // Initialization Sample
    lv_style_set_radius(&pwd_kb_style, 5);                          // Setting the rounded curvature of the style
    lv_style_set_border_width(&pwd_kb_style, 0);                    // Set the border width
    lv_style_set_bg_color(&pwd_kb_style, lv_color_hex(0xF98E2D));
    lv_style_set_text_opa(&pwd_kb_style, LV_OPA_COVER);
    lv_style_set_text_font(&pwd_kb_style,& big_font_bold);          // Setting Font
    lv_obj_set_size(pwd_keyboard, 300, 220);                        // Set the keyboard size
    lv_keyboard_set_mode(pwd_keyboard, LV_KEYBOARD_MODE_NUMBER);    // Set the keyboard mode as numeric keypad
    lv_keyboard_set_map(pwd_keyboard, LV_KEYBOARD_MODE_NUMBER, keyboard_map, keyboard_ctrl); // Set keyboard mapping
    lv_keyboard_set_textarea(pwd_keyboard, pwd_text_area);          // Keyboard Object and Text Box Bind
    lv_obj_add_style(pwd_keyboard, &pwd_kb_style, 0);               // Add Style
    lv_obj_align(pwd_keyboard, LV_ALIGN_TOP_MID, 0, 130);

    static lv_style_t btn_style;
    lv_style_reset(&btn_style);
    lv_style_init(&btn_style);                                      // Initialization
    lv_style_set_radius(&btn_style, 5);                             // Set the rounded radians of the style
    lv_style_set_border_width(&btn_style, 0);                       // Set the border width
    lv_style_set_bg_color(&btn_style, lv_color_hex(0xF98E2D));
    lv_style_set_text_color(&btn_style, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&btn_style, &big_font_bold);             // Setting Font

    // Create a confirmation button based on the keyboard background object
    lv_obj_t * confirm_btn = lv_btn_create(pwd_bg_cont);
    lv_obj_add_event_cb(confirm_btn, confirm_btn_event_callback, LV_EVENT_CLICKED, NULL); // Add Click events and event processing callback functions to the object
    lv_obj_add_style(confirm_btn, &btn_style, 0);                   // Add button style
    lv_obj_set_size(confirm_btn, 112, 37);                          // Set button object size
    lv_obj_align(confirm_btn, LV_ALIGN_TOP_MID, -65, 365);          // Set button object location

    // Create a label based on the Confirm_BTN object
    lv_obj_t *confirm_btn_label = lv_label_create(confirm_btn);
    lv_label_set_text(confirm_btn_label, "Ok");                     // Setting the text content
    lv_obj_center(confirm_btn_label);                               // Center button

    lv_obj_t * cancel_btn = lv_btn_create(pwd_bg_cont);
    lv_obj_add_event_cb(cancel_btn, cancel_btn_event_callback, LV_EVENT_CLICKED, NULL); // Add Click events and event processing callback functions to objects to objects
    lv_obj_add_style(cancel_btn, &btn_style, 0);
    lv_obj_set_size(cancel_btn, 112, 37);
    lv_obj_align(cancel_btn, LV_ALIGN_TOP_MID, 65, 365);

    lv_obj_t *cancel_btn_label = lv_label_create(cancel_btn);
    lv_label_set_text(cancel_btn_label, "Cancel");                  // Setting the text content
    lv_obj_center(cancel_btn_label);
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
        if(pwd_text_area != NULL){
            lv_textarea_set_text(pwd_text_area, "");                // Text box
        }
        if(hint_label != NULL){
            lv_label_set_text(hint_label, "");
        }
        refuseAccess();                                             // Refuse callback
        lv_obj_del(pwd_main_cont);                                  // Delete me
    }
}

// Routine to display password keyboard and get admin rights
void checkAdminRights(const int pagePtr, checkAdminCallback_t allowedCallback, checkAdminCallback_t refusedCallback){
    checkAdminPagePtr = pagePtr;                                    // Page pointer to send to routine
    checkAdminAllowed = allowedCallback;                            // Routine to be called if admin rights are given
    checkAdminRefused = refusedCallback;                            // Routine to be called if admin rights are refused
    #if (0+1)
    Serial.printf("checkAdminRights: page %d, protected %d, given %lu, refused %lu\n", 
        pagePtr, isPageProtected(pagePtr), 
        (millis() - lastAdminRightGiven) * (lastAdminRightGiven? 1 : 0) / 1000,
        (millis() - lastAdminRightRefused) * (lastAdminRightRefused? 1 : 0) / 1000);
    #endif
    if (global_config.protectionPassword[0]                         // Is admin password defined?
            && isPageProtected(pagePtr)) {                          // ... and page protected?
        if (lastAdminRightGiven && ((millis() - lastAdminRightGiven) < ADMIN_RIGHTS_TIMEOUT)) { // If right given recently
            validateAccess();                                       // Allow access
            return;
        } else if (!lastAdminRightRefused || ((millis() - lastAdminRightRefused) >= ADMIN_RIGHTS_TIMEOUT)) { // Unless rights refused recently
            password_keyboard_display();                            // Display password keyboard
            return;
        }
        runCallback(checkAdminRefused);                             // Reject access
    } else {                                                        // No protection password given
        runCallback(checkAdminAllowed);                             // Run access callback directly
    }
}

// Routine called when access is validated
void validateAccess(void) {
    lastAdminRightGiven = millis();                                 // Last time we gave admin rights
    lastAdminRightRefused = 0;                                      // Last time we refused admin rights
    Serial.printf("Access to page %d granted\n",checkAdminPagePtr);
    runCallback(checkAdminAllowed);                                 // Execute allow access code
}

// Routine called when access is refused
void refuseAccess(void) {
    lastAdminRightGiven = 0;                                        // Last time we gave admin rights
    lastAdminRightRefused = millis();                               // Last time we refused admin rights
    Serial.printf("Access to page %d refused\n",checkAdminPagePtr);
    runCallback(checkAdminRefused);                                 // Execute refuse access code
}

// Routine called when admin rights have been given
void runCallback(checkAdminCallback_t callback){
    if (callback) {                                                 // Is callback defined?
        (callback)(checkAdminPagePtr);                              // Activate callback with parameter
    }
}
