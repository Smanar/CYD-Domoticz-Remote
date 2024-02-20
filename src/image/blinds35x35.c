#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_BLINDS35X35
#define LV_ATTRIBUTE_IMG_BLINDS35X35
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_BLINDS35X35 uint8_t blinds35x35_map[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x1c, 0x00, 0x00, 0x07, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x19, 0x80, 0x00, 0x33, 0x00, 
  0x19, 0x80, 0x00, 0x33, 0x00, 
  0x19, 0x80, 0x00, 0x33, 0x00, 
  0x19, 0x80, 0x00, 0x33, 0x00, 
  0x19, 0x80, 0x00, 0x33, 0x00, 
  0x19, 0x80, 0x00, 0x33, 0x00, 
  0x19, 0xff, 0xff, 0xf3, 0x00, 
  0x19, 0xff, 0xff, 0xf3, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x18, 0x00, 0x00, 0x03, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0x00, 
  0x0f, 0xff, 0xff, 0xfe, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t blinds35x35 = {
  .header.cf = LV_IMG_CF_ALPHA_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 35,
  .header.h = 35,
  .data_size = 175,
  .data = blinds35x35_map,
};