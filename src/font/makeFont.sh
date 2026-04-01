#!/bin/bash
#	Used to generate font files with extended accents
lv_font_conv --bpp 4 --size 10 --no-compress --font Montserrat-Regular.ttf --symbols €°µ¥£²³ÀÂÄÉÈÊËÎÏÔÖÙÛÜÇàâäéèêëîïôöùûüç --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_10.c
lv_font_conv --bpp 4 --size 12 --no-compress --font Montserrat-Regular.ttf --symbols €°µ¥£²³ÀÂÄÉÈÊËÎÏÔÖÙÛÜÇàâäéèêëîïôöùûüç --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_12.c
lv_font_conv --bpp 4 --size 14 --no-compress --font Montserrat-Regular.ttf --symbols €°µ¥£²³ÀÂÄÉÈÊËÎÏÔÖÙÛÜÇàâäéèêëîïôöùûüç --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_14.c
lv_font_conv --bpp 4 --size 16 --no-compress --font Montserrat-Regular.ttf --symbols €°µ¥£²³ÀÂÄÉÈÊËÎÏÔÖÙÛÜÇàâäéèêëîïôöùûüç --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_16.c
lv_font_conv --bpp 4 --size 14 --no-compress --font Montserrat-Bold.ttf --symbols €°µ¥£²³ÀÂÄÉÈÊËÎÏÔÖÙÛÜÇàâäéèêëîïôöùûüç --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_Bold_14.c
lv_font_conv --bpp 4 --size 18 --no-compress --font Montserrat-Bold.ttf --symbols €°µ¥£²³ÀÂÄÉÈÊËÎÏÔÖÙÛÜÇàâäéèêëîïôöùûüç --range 32-126 --format lvgl --lv-include ../lvgl.h -o Montserrat_Bold_18.c