#ifdef DEBUG_LVGL
    #include "lvgl_debug.h"

    // Used to ident object dumps (up to 16 times)
    const char identChar[] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"; 

    // lv_align_t text
    const char* align_t_text[] = {
        (char*) "LV_ALIGN_DEFAULT",
        (char*) "LV_ALIGN_TOP_LEFT",
        (char*) "LV_ALIGN_TOP_MID",
        (char*) "LV_ALIGN_TOP_RIGHT",
        (char*) "LV_ALIGN_BOTTOM_LEFT",
        (char*) "LV_ALIGN_BOTTOM_MID",
        (char*) "LV_ALIGN_BOTTOM_RIGHT",
        (char*) "LV_ALIGN_LEFT_MID",
        (char*) "LV_ALIGN_RIGHT_MID",
        (char*) "LV_ALIGN_CENTER",
        (char*) "LV_ALIGN_OUT_TOP_LEFT",
        (char*) "LV_ALIGN_OUT_TOP_MID",
        (char*) "LV_ALIGN_OUT_TOP_RIGHT",
        (char*) "LV_ALIGN_OUT_BOTTOM_LEFT",
        (char*) "LV_ALIGN_OUT_BOTTOM_MID",
        (char*) "LV_ALIGN_OUT_BOTTOM_RIGHT",
        (char*) "LV_ALIGN_OUT_LEFT_TOP",
        (char*) "LV_ALIGN_OUT_LEFT_MID",
        (char*) "LV_ALIGN_OUT_LEFT_BOTTOM",
        (char*) "LV_ALIGN_OUT_RIGHT_TOP",
        (char*) "LV_ALIGN_OUT_RIGHT_MID",
        (char*) "LV_ALIGN_OUT_RIGHT_BOTTOM"
    };

    // lv_grad_dir_t text
    const char* grad_dir_t_text[] = {
        "LV_GRAD_DIR_NONE",
        "LV_GRAD_DIR_VER",
        "LV_GRAD_DIR_HOR"
    };
    
    // lv_blend_mode_t text
    const char* blend_mode_t_text[] = {
        "LV_BLEND_MODE_NORMAL",
        "LV_BLEND_MODE_ADDITIVE",
        "LV_BLEND_MODE_SUBTRACTIVE",
        "LV_BLEND_MODE_MULTIPLY",
        "LV_BLEND_MODE_REPLACE"
    };

    // lv_border_side_t text and bits
    const char* border_side_t_text[] = {
        "LV_BORDER_SIDE_NONE",
        "LV_BORDER_SIDE_BOTTOM",
        "LV_BORDER_SIDE_TOP",
        "LV_BORDER_SIDE_LEFT",
        "LV_BORDER_SIDE_RIGHT",
        "LV_BORDER_SIDE_FULL",
        "LV_BORDER_SIDE_INTERNAL"
    };

    const uint8_t border_side_t_bits[] = {
        0x00,
        0x01,
        0x02,
        0x04,
        0x08,
        0x0F,
        0x10
    };

    // lv_text_decor_t text and bits
    const char* text_decor_t_text[] = {
        "LV_TEXT_DECOR_NONE",
        "LV_TEXT_DECOR_UNDERLINE",
        "LV_TEXT_DECOR_STRIKETHROUGH"
    };

    const uint8_t text_decor_t_bits[] = {
        0x00,
        0x01,
        0x02
    };

    // lv_base_dir_t text and bits
    const char* base_dir_t_text[] = {
        "LV_BASE_DIR_LTR",
        "LV_BASE_DIR_RTL",
        "LV_BASE_DIR_AUTO"
        "LV_BASE_DIR_NEUTRAL",
        "LV_BASE_DIR_WEAK"
    };

    const uint8_t base_dir_t_bits[] = {
        0x00,
        0x01,
        0x02,
        0x20,
        0x21
    };

    // lv_obj_flag_t text and bits
    const char* obj_flag_t_text[] = {
        "LV_OBJ_FLAG_HIDDEN",
        "LV_OBJ_FLAG_CLICKABLE",
        "LV_OBJ_FLAG_CLICK_FOCUSABLE",
        "LV_OBJ_FLAG_CHECKABLE",
        "LV_OBJ_FLAG_SCROLLABLE",
        "LV_OBJ_FLAG_SCROLL_ELASTIC",
        "LV_OBJ_FLAG_SCROLL_MOMENTUM",
        "LV_OBJ_FLAG_SCROLL_ONE",
        "LV_OBJ_FLAG_SCROLL_CHAIN_HOR",
        "LV_OBJ_FLAG_SCROLL_CHAIN_VER",
        "LV_OBJ_FLAG_SCROLL_ON_FOCUS",
        "LV_OBJ_FLAG_SCROLL_WITH_ARROW",
        "LV_OBJ_FLAG_SNAPPABLE",
        "LV_OBJ_FLAG_PRESS_LOCK",
        "LV_OBJ_FLAG_EVENT_BUBBLE",
        "LV_OBJ_FLAG_GESTURE_BUBBLE",
        "LV_OBJ_FLAG_ADV_HITTEST",
        "LV_OBJ_FLAG_IGNORE_LAYOUT",
        "LV_OBJ_FLAG_FLOATING",
        "LV_OBJ_FLAG_OVERFLOW_VISIBLE",
        "LV_OBJ_FLAG_LAYOUT_1",
        "LV_OBJ_FLAG_LAYOUT_2",
        "LV_OBJ_FLAG_WIDGET_1",
        "LV_OBJ_FLAG_WIDGET_2",
        "LV_OBJ_FLAG_USER_1",
        "LV_OBJ_FLAG_USER_2",
        "LV_OBJ_FLAG_USER_3",
        "LV_OBJ_FLAG_USER_4"
    };

    const uint32_t obj_flag_t_bits[] = {
        (1L<<0),
        (1L<<1),
        (1L<<2),
        (1L<<3),
        (1L<<4),
        (1L<<5),
        (1L<<6),
        (1L<<7),
        (1L<<8),
        (1L<<9),
        (1L<<10),
        (1L<<11),
        (1L<<12),
        (1L<<13),
        (1L<<14),
        (1L<<15),
        (1L<<16),
        (1L<<17),
        (1L<<18),
        (1L<<19),
        (1L<<23),
        (1L<<24),
        (1L<<25),
        (1L<<26),
        (1L<<27),
        (1L<<28),
        (1L<<29),
        (1L<<30)
    };

    // lv_obj_state_t text and bits
    const char* obj_state_t_text[] = {
        "LV_STATE_DEFAULT",
        "LV_STATE_CHECKED",
        "LV_STATE_FOCUSED",
        "LV_STATE_FOCUS_KEY",
        "LV_STATE_EDITED",
        "LV_STATE_HOVERED",
        "LV_STATE_PRESSED",
        "LV_STATE_SCROLLED",
        "LV_STATE_DISABLED",
        "LV_STATE_USER_1",
        "LV_STATE_USER_2",
        "LV_STATE_USER_3",
        "LV_STATE_USER_4",
        "LV_STATE_ANY"
    };

    const uint32_t obj_state_t_bits[] = {
        0x0000,
        0x0001,
        0x0002,
        0x0004,
        0x0008,
        0x0010,
        0x0020,
        0x0040,
        0x0080,
        0x1000,
        0x2000,
        0x4000,
        0x8000,
        0xFFFF
    };

    //  Dumps information about a lvgl object (debug)
    void dumpObject(lv_obj_t* objectToDump, const char* name, uint8_t ident){
        lv_area_t objCoords;
        lv_obj_get_coords(objectToDump, &objCoords);

        Serial.printf("%.*sLV_OBJ_T: %s, x=%d, y=%d, w=%d, h=%d\n",
            ident, identChar, 
            name,
            objCoords.x1,
            objCoords.y1,
            1 + objCoords.x2 - objCoords.x1,
            1 + objCoords.y2 - objCoords.y1
        );
        
        decodeObjType(objectToDump, ident+1);
        Serial.printf("%.*sLV_OBJ_FLAG=0x%x (", ident+1, identChar, objectToDump->flags);
        decodeNotBitValue(objectToDump->flags, obj_flag_t_text, obj_flag_t_bits, sizeof(obj_flag_t_text) / sizeof(char*));

        Serial.printf("%.*sLV_OBJ_STATE=0x%x (", ident+1, identChar, objectToDump->state);
        decodeBitValue(objectToDump->state, obj_state_t_text, obj_state_t_bits, sizeof(obj_state_t_text) / sizeof(char*));

        dumpStyle(objectToDump, ident+1);
    }

    // Dump an object and all associated children, possibly recursively
    void dumpTreeObject(lv_obj_t* objectToDump, const char* name, uint8_t ident, bool recursive) {
        dumpObject(objectToDump, name, ident);
        lv_obj_t * child;
        char childName[strlen(name)+5]; // Allow adding :999 plus ending zero to current name
        for (uint32_t i=0; i < lv_obj_get_child_cnt(objectToDump); i++) {
            child = lv_obj_get_child(objectToDump, i);
            snprintf(childName, strlen(name)+5, "%s:%d", name, i);
            if (recursive) {
                dumpTreeObject(child, childName, ident+1, recursive);
            } else {
                dumpObject(child, childName, ident+1);
            }
        }
    }

    // Extract lvgl object type
    void decodeObjType(lv_obj_t* objectToDump, uint8_t ident) {
        const lv_obj_class_t *objClass = lv_obj_get_class(objectToDump);
        Serial.printf("%.*sLV_OBJ_TYPE=", ident, identChar);
        if (objClass == &lv_arc_class) {
            Serial.print("ARC");
        } else if (objClass == &lv_bar_class) {
            Serial.print("BAR");
        } else if (objClass == &lv_btn_class) {
            Serial.print("BTN");
        } else if (objClass == &lv_btnmatrix_class) {
            Serial.print("BTNMATRIX");
        } else if (objClass == &lv_canvas_class) {
            Serial.print("CANVAS");
        } else if (objClass == &lv_checkbox_class) {
            Serial.printf("CHECKBOX (%s)", lv_checkbox_get_text(objectToDump));
        } else if (objClass == &lv_dropdown_class) {
            Serial.printf("DROPDOWN (%s)", lv_dropdown_get_text(objectToDump));
        } else if (objClass == &lv_img_class) {
            Serial.print("IMG");
        } else if (objClass == &lv_label_class) {
            Serial.printf("LABEL (%s)", lv_label_get_text(objectToDump));
        } else if (objClass == &lv_line_class) {
            Serial.print("LINE");
        } else if (objClass == &lv_roller_class) {
            char buffer[128];
            lv_roller_get_selected_str(objectToDump, buffer, sizeof(buffer));
            Serial.printf("ROLLER (%s)", buffer);
        } else if (objClass == &lv_slider_class) {
            Serial.printf("SLIDER (%l)", lv_slider_get_value(objectToDump));
        } else if (objClass == &lv_switch_class) {
            Serial.print("SWITCH");
        } else if (objClass == &lv_table_class) {
            Serial.print("TABLE");
        } else if (objClass == &lv_textarea_class) {
            Serial.printf("TEXTAREA (%s)", lv_textarea_get_text(objectToDump));
        } else {
            Serial.print("??? unknown ???");
        }
        Serial.println();
    }

    // Print text of a value if in range, else raw value
    void decodeValue(uint16_t value, const char* text[], const size_t textSize) {
        if (value < textSize) {
            Serial.print(text[value]);
        } else {
            Serial.printf("??? 0x%x ???", value);
        }
        Serial.println(")");
    } 

    // Print text of a bit it value (first value opf table should be text of zero value)
    void decodeBitValue(uint16_t value, const char* text[], const uint8_t bits[], const size_t textSize) {
        bool somethingPrinted = false;
        if (value) {
            for (uint8_t i = 1; i < textSize; i++) {
            if (value && (bits[i] == bits[i])) {
                if (somethingPrinted) Serial.print(" | ");
                somethingPrinted = true;
                Serial.print(text[i]);
            }
        }
        if (!somethingPrinted) {
            Serial.printf("??? %d ???", value);
            }
        } else {
            Serial.print(text[0]);
        }
        Serial.println(")");
    }

    // Print text of a bit it value (first value opf table should be text of zero value)
    void decodeBitValue(uint32_t value, const char* text[], const uint32_t bits[], const size_t textSize) {
        bool somethingPrinted = false;
        if (value) {
            for (uint8_t i = 1; i < textSize; i++) {
                if (value && (bits[i] == bits[i])) {
                    if (somethingPrinted) Serial.print(" | ");
                    somethingPrinted = true;
                    Serial.print(text[i]);
                }
            }
            if (!somethingPrinted) {
                Serial.printf("??? %d ???", value);
            }
        } else {
            Serial.print(text[0]);
        }
        Serial.println(")");
    }

    // Print text of not set bit value
    void decodeNotBitValue(uint32_t value, const char* text[], const uint32_t bits[], const size_t textSize) {
        bool somethingPrinted = false;
        for (uint8_t i = 0; i < textSize; i++) {
            if (!value && (bits[i])) {
                if (somethingPrinted) Serial.print(" | ");
                somethingPrinted = true;
                Serial.print("-");
                Serial.print(text[i]);
            }
        }
        if (!somethingPrinted) {
            Serial.printf("[Default]", value);
        }
        Serial.println(")");
    }

    void dumpStyle(const lv_obj_t* objToDump, uint8_t ident) {
        {
            lv_coord_t value = lv_obj_get_style_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_WIDTH=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_min_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_MIN_WIDTH=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_max_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_MAX_WIDTH=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_height(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_HEIGHT=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_min_height(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_MIN_HEIGHT=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_max_height(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_MAX_HEIGHT=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_x(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_X=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_y(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_Y=%d\n", ident, identChar, value);
        }
        {
            lv_align_t value = lv_obj_get_style_align(objToDump, LV_PART_MAIN);
            if (value) {
                Serial.printf("%.*sLV_STYLE_ALIGN=0x%x (", ident, identChar, value);
                decodeValue(value, align_t_text, sizeof(align_t_text) / sizeof(char*));
            }
        }
        {
            lv_coord_t value = lv_obj_get_style_transform_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TRANSFORM_WIDTH=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_transform_height(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TRANSFORM_HEIGHT=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_translate_x(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TRANSLATE_X=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_translate_y(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TRANSLATE_Y=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_transform_zoom(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TRANSFORM_ZOOM=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_transform_angle(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TRANSFORM_ANGLE=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_transform_pivot_x(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TRANSFORM_PIVOT_X=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_transform_pivot_y(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TRANSFORM_PIVOT_Y=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_pad_top(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_PAD_TOP=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_pad_bottom(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_PAD_BOTTOM=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_pad_left(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_PAD_LEFT=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_pad_right(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_PAD_RIGHT=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_pad_row(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_PAD_ROW=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_pad_column(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_PAD_COLUMN=%d\n", ident, identChar, value);
        }
        {
            lv_color_t value = lv_obj_get_style_bg_color(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_BG_COLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_opa_t value = lv_obj_get_style_bg_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_BG_OPA=%d\n", ident, identChar, value);
        }
        {
            lv_color_t value = lv_obj_get_style_bg_grad_color(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_BG_GRAD_COLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_grad_dir_t value = lv_obj_get_style_bg_grad_dir(objToDump, LV_PART_MAIN);
            if (value) {
                Serial.printf("%.*sLV_STYLE_BG_GRAD_DIR=0x%x (", ident, identChar, value);
                decodeValue(value, grad_dir_t_text, sizeof(grad_dir_t_text) / sizeof(char*));
            }
        }
        {
            lv_coord_t value = lv_obj_get_style_bg_main_stop(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_BG_MAIN_STOP=%d\n", ident, identChar, value);
        }
        {
            lv_opa_t value = lv_obj_get_style_bg_img_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_BG_IMG_OPA=%d\n", ident, identChar, value);
        }
        {
            lv_color_t value = lv_obj_get_style_bg_img_recolor(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_BG_IMG_RECOLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_opa_t value = lv_obj_get_style_bg_img_recolor_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_BG_IMG_RECOLOR_OPA=%d\n", ident, identChar, value);
        }
        {
            bool value = lv_obj_get_style_bg_img_tiled(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_BG_IMG_TILED=%s\n", ident, identChar, value ? "true": "false");
        }
        {
            lv_color_t value = lv_obj_get_style_border_color(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_BORDER_COLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_opa_t value = lv_obj_get_style_border_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_BORDER_OPA=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_border_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_BORDER_WIDTH=%d\n", ident, identChar, value);
        }
        {
            lv_border_side_t value = lv_obj_get_style_border_side(objToDump, LV_PART_MAIN);
            if (value) {
                Serial.printf("%.*sLV_STYLE_BORDER_SIDE=0x%x (", ident, identChar, value);
                decodeBitValue(value, border_side_t_text, border_side_t_bits, sizeof(border_side_t_text) / sizeof(char*));
            }
        }
        {
            bool value = lv_obj_get_style_border_post(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_BORDER_POST=%s\n", ident, identChar, value ? "true": "false");
        }
        {
            lv_coord_t value = lv_obj_get_style_outline_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_OUTLINE_WIDTH=%d\n", ident, identChar, value);
        }
        {
            lv_color_t value = lv_obj_get_style_outline_color(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_OUTLINE_COLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_opa_t value = lv_obj_get_style_outline_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_OUTLINE_OPA=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_outline_pad(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_OUTLINE_PAD=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_shadow_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_SHADOW_WIDTH=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_shadow_ofs_x(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_SHADOW_OFS_X=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_shadow_ofs_y(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_SHADOW_OFS_Y=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_shadow_spread(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_SHADOW_SPREAD=%d\n", ident, identChar, value);
        }
        {
            lv_color_t value = lv_obj_get_style_shadow_color(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_SHADOW_COLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_opa_t value = lv_obj_get_style_shadow_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_SHADOW_OPA=%d\n", ident, identChar, value);
        }
        {
            lv_opa_t value = lv_obj_get_style_img_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_IMG_OPA=%d\n", ident, identChar, value);
        }
        {
            lv_color_t value = lv_obj_get_style_img_recolor(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_IMG_RECOLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_opa_t value = lv_obj_get_style_img_recolor_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_IMG_RECOLOR_OPA=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_line_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_LINE_WIDTH=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_line_dash_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_LINE_DASH_WIDTH=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_line_dash_gap(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_LINE_DASH_GAP=%d\n", ident, identChar, value);
        }
        {
            bool value = lv_obj_get_style_line_rounded(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_LINE_ROUNDED=%s\n", ident, identChar, value ? "true": "false");
        }
        {
            lv_color_t value = lv_obj_get_style_line_color(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_LINE_COLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_opa_t value = lv_obj_get_style_line_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_LINE_OPA=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_arc_width(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_ARC_WIDTH=%d\n", ident, identChar, value);
        }
        {
            bool value = lv_obj_get_style_arc_rounded(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_ARC_ROUNDED=%s\n", ident, identChar, value ? "true": "false");
        }
        {
            lv_color_t value = lv_obj_get_style_arc_color(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_ARC_COLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_color_t value = lv_obj_get_style_text_color(objToDump, LV_PART_MAIN);
            if (value.full) Serial.printf("%.*sLV_STYLE_TEXT_COLOR=0x%06x\n", ident, identChar, value.full);
        }
        {
            lv_coord_t value = lv_obj_get_style_text_letter_space(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TEXT_LETTER_SPACE=%d\n", ident, identChar, value);
        }
        {
            lv_coord_t value = lv_obj_get_style_text_line_space(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_TEXT_LINE_SPACE=%d\n", ident, identChar, value);
        }
        {
            lv_text_decor_t value = lv_obj_get_style_text_decor(objToDump, LV_PART_MAIN);
            if (value) {
                Serial.printf("%.*sLV_STYLE_TEXT_DECOR=0x%x (", ident, identChar, value);
                decodeBitValue(value, text_decor_t_text, text_decor_t_bits, sizeof(text_decor_t_text) / sizeof(char*));
            }
        }
        {
            lv_text_align_t value = lv_obj_get_style_text_align(objToDump, LV_PART_MAIN);
            if (value) {
                Serial.printf("%.*sLV_STYLE_TEXT_ALIGN=0x%x (", ident, identChar, value);
                decodeValue(value, align_t_text, sizeof(align_t_text) / sizeof(char*));
            }
        }
        {
            lv_coord_t value = lv_obj_get_style_radius(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_RADIUS=%d\n", ident, identChar, value);
        }
        {
            bool value = lv_obj_get_style_clip_corner(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_CLIP_CORNER=%s\n", ident, identChar, value ? "true": "false");
        }
        {
            lv_opa_t value = lv_obj_get_style_opa(objToDump, LV_PART_MAIN);
            if (value != 255) Serial.printf("%.*sLV_STYLE_OPA=%d\n", ident, identChar, value);
        }
        {
            uint32_t value = lv_obj_get_style_anim_time(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_ANIM_TIME=%ld\n", ident, identChar, value);
        }
        {
            lv_blend_mode_t value = lv_obj_get_style_blend_mode(objToDump, LV_PART_MAIN);
            if (value) {
                Serial.printf("%.*sLV_STYLE_BLEND_MODE=0x%x (", ident, identChar, value);
                decodeValue(value, blend_mode_t_text, sizeof(blend_mode_t_text) / sizeof(char*));
            }
        }
        {
            uint16_t value = lv_obj_get_style_layout(objToDump, LV_PART_MAIN);
            if (value) Serial.printf("%.*sLV_STYLE_LAYOUT=%d\n", ident, identChar, value);
        }
        {
            lv_base_dir_t value = lv_obj_get_style_base_dir(objToDump, LV_PART_MAIN);
            if (value) {
                Serial.printf("%.*sLV_STYLE_BASE_DIR=0x%x (", ident, identChar, value);
                decodeBitValue(value, base_dir_t_text, base_dir_t_bits, sizeof(base_dir_t_text) / sizeof(char*));
            }
        }
    }
    
#endif