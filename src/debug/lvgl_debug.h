#pragma once

#ifdef DEBUG_LVGL
    #include "Arduino.h"
    #include "lvgl.h"

    void dumpObject(lv_obj_t* objectToDump, const char* name, uint8_t ident = 0);
    void dumpTreeObject(lv_obj_t* objectToDump, const char* name, uint8_t ident = 0, bool recursive = false);
    void dumpStyle(const lv_obj_t* objToDump, uint8_t ident);
    void decodeObjType(lv_obj_t* objectToDump, uint8_t ident);
    void decodeValue(uint16_t value, const char* text[], const size_t textSize);
    void decodeBitValue(uint16_t value, const char* text[], const uint8_t bits[], const size_t textSize);
    void decodeBitValue(uint32_t value, const char* text[], const uint32_t bits[], const size_t textSize);
    void decodeNotBitValue(uint32_t value, const char* text[], const uint32_t bits[], const size_t textSize);
#endif