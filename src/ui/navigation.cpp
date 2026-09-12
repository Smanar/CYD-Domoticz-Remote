#include "lvgl.h"
#include "navigation.h"
#include "panels/panel.h"
#include "../core/data_setup.h"
#include "../core/ip_engine.h"
#include "../conf/global_config.h"
#include "../core/helper.h"

#ifdef INTERACTION
    #define INTERACTION_MAX_BUTTONS 5
    static char* buttonsTable[INTERACTION_MAX_BUTTONS] = {NULL};    // Static pointers table for buttons
    static lv_obj_t* interaction_msgbox = nullptr;                  // Message box object
    static lv_obj_t* overlay = nullptr;                             // Overlay object
    static  lv_timer_t* interaction_timeout;                        // Interaction timeout
    static JsonDocument jsonResponse;                               // Response for INTERACTION
    static bool responseSent = false;                               // Is response already sent?
    static bool interactionActive = false;                          // Is an interaction active?
#endif

static lv_obj_t * tv;
static int actived_panel = 0;
static int master_panel = 0;

extern Device myDevices[];

int GetActiveWidgetPage(void)
{
    return actived_panel - HOMEPAGE_PANEL;
}

int GetActivePanel(void)
{
    return actived_panel;
}

void SetActivePanel(int p)
{
    actived_panel = p;
}

void ReturnPreviouspage(void)
{
    navigation_screen(master_panel);
}

void RefreshWidgetsPanel(bool dontLoadData)
{
    if (actived_panel >= HOMEPAGE_PANEL && actived_panel <= LAST_PAGE_PANEL)
    {                           
        navigation_screen(actived_panel, dontLoadData);
    }
}

void RefreshDevicePanel(void)
{
    if (actived_panel == DEVICE_PANEL)
    {                           
        navigation_screen(DEVICE_PANEL);
    }
}

#ifndef NO_GROUP_PAGE
void RefreshScenePanel(void)
{
    if (actived_panel == GROUP_PANEL)
    {                           
        navigation_screen(GROUP_PANEL);
    }
}
#endif

void navigation_screen(unsigned char active_panel, bool dontLoadData)
{
    actived_panel = active_panel;

    //Clear the display
    lv_obj_clean(lv_scr_act());

    lv_obj_t * panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_align(panel, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    //lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    char pageName[30];
    getPanelName(active_panel, pageName, sizeof(pageName));
    Serial.printf("Showing page %d (%s)\n", active_panel, pageName);

    switch (active_panel)
    {
        case TOOL_PANEL: // Tools
            master_panel = active_panel;
            tools_panel_init(panel);
            break;
        case HOMEPAGE_PANEL ... (LAST_PAGE_PANEL): // Widget pages
            master_panel = active_panel;
            widget_panel_init(panel, dontLoadData);
            break;
#ifndef NO_GROUP_PAGE
        case GROUP_PANEL: //Group/Scene panel
            master_panel = active_panel;
            group_panel_init(panel);
            break;
#endif
#ifndef NO_INFO_PAGE
        case INFO_PANEL: //Info panel
            master_panel = active_panel;
            info_panel_init(panel);
            break;
#endif
        case DEVICE_PANEL: // Device panel
            device_panel_init(panel);
            break;
        case SETTING_PANEL:// Settings
            settings_panel_init(panel);
            break;
        case PASS_PANEL://Password entry
            password_keyboard_display(panel);
            break;

        default:
            tools_panel_init(panel);
            break;
    }
}

void nav_style_setup()
{

}

// Check if a given page is protected
bool isPageProtected(int page) {
    if (page == TOOL_PANEL) {                                       // Tools
        return global_config.protectTool;
    } else if (page >= HOMEPAGE_PANEL && page <= LAST_PAGE_PANEL) { // Homepage
        return global_pages[page - HOMEPAGE_PANEL].isProtected;
    #ifndef NO_GROUP_PAGE
    } else if (page == GROUP_PANEL) {                               //Group/Scene panel
        return global_config.protectGroup;
    #endif
    #ifndef NO_INFO_PAGE
    } else if (page == INFO_PANEL) {                                //Info panel
        return global_config.protectInfo;
    #endif
    } else if (page == SETTING_PANEL) {                             // Settings
        return global_config.protectSetting;
    }
    return false;                                                   // By default (including DEVICE_PANEL)
}

#ifdef INTERACTION
    // Process part of interaction

    // Indicates if interaction is currently active
    bool isInteractionActive(void) {
        return interactionActive;
    }

    // Message box timeout callback
    static void interaction_timeout_cb(lv_timer_t * timer) {
        if (!responseSent) {
            jsonResponse["errorText"] = "timeout";
            sendInteractionResponse();
            lv_msgbox_close(interaction_msgbox);
        }
    }

    // Message box delete callback
    static void interaction_msgbox_delete_cb(lv_event_t * e)
    {
        if (interactionActive) {
            interactionActive = false;
            interaction_msgbox = nullptr;
            if (!responseSent) {
                Serial.printf("Message box deleted\n");
                jsonResponse["errorText"] = "canceled";
                sendInteractionResponse();
            }
            lv_timer_del(interaction_timeout);
            lv_obj_del_async(overlay);
            for (unsigned int i=0; i < INTERACTION_MAX_BUTTONS; i++){
                if (buttonsTable[i]) {
                    free(buttonsTable[i]);
                    buttonsTable[i] = NULL;
                }
            }
        }
    }

    // Message box button click callback
    static void interaction_msgbox_button_click_cb(lv_event_t * e)
    {
        lv_obj_t * obj = lv_event_get_current_target(e);
        jsonResponse["button"] = lv_msgbox_get_active_btn_text(obj);
        sendInteractionResponse();
        lv_msgbox_close(interaction_msgbox);
    }

    // Dropdown list change callback
    static void interaction_dropdown_change_cb(lv_event_t * e) {
        lv_obj_t * ta = lv_event_get_target(e);
    
        char buf[64];
        lv_dropdown_get_selected_str(ta, buf, sizeof(buf));
        jsonResponse["value"] = buf; 
    }

    // Text area change callback
    static void interaction_textArea_cb(lv_event_t * e) {
        lv_obj_t * target = lv_event_get_target(e);
        jsonResponse["value"] = lv_textarea_get_text(target);
    }

    static void keyboard_event_cb(lv_event_t * e) {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t * ta = lv_event_get_target(e);
        lv_obj_t * kb = (lv_obj_t *)lv_event_get_user_data(e);
    
        if(code == LV_EVENT_FOCUSED) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
        else if(code == LV_EVENT_DEFOCUSED) {
            lv_keyboard_set_textarea(kb, NULL);
            lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    //  Here, we're called with request JSON from "Command IDX" Domoticz text device
    void processInteraction(const char* interactionMessage) {
        // Process interaction only if not empty
        if (*interactionMessage) {
            Serial.printf("Processing interaction '%s'\n", interactionMessage);
            char buffer[70];
            // Clear interaction request (to avoid looping on same request)
            lv_snprintf(buffer, sizeof(buffer), "/json.htm?type=command&param=udevice&idx=%d&nvalue=0&svalue=", global_config.commandIdx);
            HTTPGETRequest(buffer);

            // Prepare an empty answer
            jsonResponse["type"] = "";
            jsonResponse["errorText"] = "";
            jsonResponse["button"] = "";
            jsonResponse["value"] = "";
            jsonResponse["id"] = "";
            responseSent = false;

            JsonDocument jsonCommand;        // Command for INTERACTION

            auto error = deserializeJson(jsonCommand, interactionMessage);  // Read message
            if (error) {                                                    // Error reading JSON?
                jsonResponse["errorText"] = "Failed to parse interaction";
                sendInteractionResponse();
                return;
            }

            // Copy id from request to response
            jsonResponse["id"] = jsonCommand["id"];

            // Check for "type" in message as string
            if (jsonCommand["type"].is<const char*>()) {
                // Check for "ask" type
                jsonResponse["type"] = jsonCommand["type"];
                if (jsonCommand["type"] == "ask") {
                    if (interaction_msgbox) {
                        lv_msgbox_close(interaction_msgbox);
                    }
                    // Here, response is preloaded with type and id, analyze request loading defaults
                    const char* title = jsonCommand["title"] | "No title given";
                    const char* message = jsonCommand["message"] | "No message given";
                    const int timeOut = jsonCommand["timeOut"] | 10;
                    Serial.printf("Title: %s\nMessage: %s\nTimeOut: %d\n", title, message, timeOut);

                    // Extract list of buttons, use default one if not specified
                    JsonDocument defaultArray;
                    char defaultButton[] = "[\"Ok\"]";
                    deserializeJson(defaultArray, defaultButton);
                    JsonArray buttonArray = jsonCommand["buttons"].as<JsonArray>();
                    if (!buttonArray) {
                        buttonArray = defaultArray.as<JsonArray>();
                    }

                    // Extract all button items
                    for (size_t i=0; i < buttonArray.size() && i < INTERACTION_MAX_BUTTONS; i++) {
                        Serial.printf("Button: %s\n", buttonArray[i].as<const char*>());
                        char* ptr = (char*) malloc(strlen(buttonArray[i].as<const char*>())+1);
                        strcpy(ptr, buttonArray[i].as<const char*>());
                        buttonsTable[i] = ptr;
                    }

                    // Create overlay
                    overlay = lv_obj_create(lv_scr_act());
                    lv_obj_set_size(overlay, LV_HOR_RES, LV_VER_RES);
                    lv_obj_set_style_bg_color(overlay, lv_color_black(), LV_PART_MAIN);
                    lv_obj_set_style_bg_opa(overlay, LV_OPA_50, LV_PART_MAIN);
                    lv_obj_set_style_border_width(overlay, 0, LV_PART_MAIN);
                    lv_obj_align(overlay, LV_ALIGN_CENTER, 0, 0);

                    // Create timeout timer
                    interaction_timeout = lv_timer_create(interaction_timeout_cb, timeOut * 1000, 0);

                    // Create message box
                    interactionActive = true;
                    interaction_msgbox = lv_msgbox_create(overlay, title, message, (const char**) buttonsTable, true);
                    lv_obj_add_event_cb(interaction_msgbox, interaction_msgbox_button_click_cb, LV_EVENT_VALUE_CHANGED, NULL);
                    lv_obj_add_event_cb(interaction_msgbox, interaction_msgbox_delete_cb, LV_EVENT_DELETE, NULL);
                    lv_obj_align(interaction_msgbox, LV_ALIGN_CENTER, 0, 0);
                    lv_point_t titleSize, messageSize;
                    lv_txt_get_size(&titleSize, title, LV_FONT_DEFAULT, 0, 0, LCD_WIDTH * 0.9, LV_TEXT_FLAG_NONE); // Get text size
                    lv_txt_get_size(&messageSize, message, LV_FONT_DEFAULT, 0, 0, LCD_WIDTH * 0.9, LV_TEXT_FLAG_NONE); // Get text size
                    lv_obj_set_width(interaction_msgbox, titleSize.x > messageSize.x ? titleSize.x : messageSize.x);

                    // Get msgbox content object and height
                    lv_obj_t* msgbox_content = lv_msgbox_get_content(interaction_msgbox);
                    lv_obj_update_layout(interaction_msgbox);
                    lv_coord_t msgbox_height = lv_obj_get_height(msgbox_content);

                    // Look for responseType = list
                    JsonArray listArray = jsonCommand["responseType"]["list"].as<JsonArray>();
                    if (listArray) {
                        size_t listSize = 0;
                        // Extract list size
                        for (size_t i=0; i<listArray.size(); i++) {
                            listSize += strlen(listArray[i].as<const char*>())+1;
                        }
                        char listContent[listSize] = {0};
                        // Extract all items, terminate them by \n
                        for (size_t i=0; i<listArray.size(); i++) {
                            strcat(listContent, listArray[i].as<const char*>());
                            if (i+1 < listArray.size()) {
                                strcat(listContent, "\n");
                            }
                        }
                        // Set first item as selected value
                        jsonResponse["value"] = listArray[0].as<const char*>();
                        Serial.printf("Full list:\n%s\n", listContent);
                        //  Create a normal drop down list
                        lv_obj_t* dropdown1 = lv_dropdown_create(msgbox_content);
                        lv_dropdown_set_options(dropdown1, listContent);
                        lv_obj_set_width(dropdown1, LV_PCT(100));
                        lv_obj_set_pos(dropdown1, 0, msgbox_height + 10);
                        lv_obj_add_event_cb(dropdown1, interaction_dropdown_change_cb, LV_EVENT_VALUE_CHANGED, NULL);
                    } else {
                        // Is this a responseType = string?
                        const char* responseType = jsonCommand["responseType"] | "";
                        if (responseType && !strcmp(responseType, "string")) {
                            Serial.print("Get a string\n");
                            lv_obj_t* keyboard = lv_keyboard_create(overlay);
                            lv_coord_t height = lv_obj_get_height(msgbox_content);
                            lv_obj_t* text = lv_textarea_create(msgbox_content);
                            lv_obj_add_event_cb(text, interaction_textArea_cb, LV_EVENT_VALUE_CHANGED, NULL);
                            lv_obj_add_event_cb(text, keyboard_event_cb, LV_EVENT_ALL, keyboard);
                            lv_textarea_set_one_line(text, true);
                            lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
                            lv_keyboard_set_textarea(keyboard, text);
                            lv_obj_set_width(text, LV_PCT(100));
                            lv_obj_set_pos(text, 0, msgbox_height + 10);
                            lv_obj_align(interaction_msgbox, LV_ALIGN_TOP_MID, 0, 0);
                            // Is this a responseType = number?
                        } else if (responseType && !strcmp(responseType, "number")) {
                            Serial.print("Get a number\n");
                            lv_obj_t* keyboard = lv_keyboard_create(overlay);
                            lv_obj_t* text = lv_textarea_create(msgbox_content);
                            lv_obj_add_event_cb(text, interaction_textArea_cb, LV_EVENT_VALUE_CHANGED, NULL);
                            lv_obj_add_event_cb(text, keyboard_event_cb, LV_EVENT_ALL, keyboard);
                            lv_textarea_set_accepted_chars(text, "0123456789-");
                            lv_textarea_set_one_line(text, true);
                            lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER);
                            lv_keyboard_set_textarea(keyboard, text);
                            lv_obj_set_width(text, LV_PCT(100));
                            lv_obj_set_pos(text, 0, msgbox_height + 10);
                            lv_obj_align(interaction_msgbox, LV_ALIGN_TOP_MID, 0, 0);
                        }
                    }
                } else {
                    jsonResponse["errorText"] = "'Type' should be 'ask'";
                    sendInteractionResponse();
                    return;
                }
            } else {
                jsonResponse["errorText"] = "Illegal 'type'";
                sendInteractionResponse();
                return;
            }
        }
    }

    // Send an interaction response
    void sendInteractionResponse(void) {
        // Create a buffer to get response JSON text
        char buffer[measureJson(jsonResponse) + 1];
        // Try to serialize answer
        if (serializeJson(jsonResponse, buffer, sizeof(buffer))) {
            Serial.printf("Interaction response '%s'\n", buffer);
            // Send message back if response IDX defined
            if (global_config.responseIdx) {
                // Make a buffer with size of encoded command + size of URL
                char buffer2[urlEncode(NULL, 0, buffer) + 70];
                lv_snprintf(buffer2, sizeof(buffer2), "/json.htm?type=command&param=udevice&idx=%d&nvalue=0&svalue=", global_config.responseIdx);
                // Add buffer at end (used part) of buffer2
                urlEncode(buffer2 + strlen(buffer2), sizeof(buffer2) - (strlen(buffer2) + 1), buffer);
                HTTPGETRequest(buffer2);
            }
        } else {
            Serial.printf("Can't serialize response, initial error %s\n", jsonResponse["errorText"].as<const char*>());
        }
        responseSent = true;
    }
#endif
