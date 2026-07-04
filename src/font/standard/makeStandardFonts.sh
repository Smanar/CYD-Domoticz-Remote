#!/bin/bash
#	Used to generate standard font files with only few symbols
#   Don't change it !!!
#   Be aware that some icons from FontAwesome5-Solid+Brands+Regular.woff are not into these fonts. They can't be used as default font.
#   lv_font_conv tool is needed, and can be installed by "npm i lv_font_conv -g"
lv_font_conv --bpp 4 --size 12 --no-compress --font ../Montserrat-Regular.ttf --symbols €°µ¥£²³ --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_12.c
lv_font_conv --bpp 4 --size 16 --no-compress --font ../Montserrat-Regular.ttf --symbols €°µ¥£²³ --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_16.c
lv_font_conv --bpp 4 --size 14 --no-compress --font ../Montserrat-Bold.ttf --symbols €°µ¥£²³ --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_Bold_14.c
lv_font_conv --bpp 4 --size 18 --no-compress --font ../Montserrat-Bold.ttf --symbols €°µ¥£²³ --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_Bold_18.c

