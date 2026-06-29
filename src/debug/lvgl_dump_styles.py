"""
    Read lvgl object style file and generate and generate C dump routine code
"""

import os
import pathlib

#   *****************
#   *** Main code ***
#   *****************

# Set current working directory to this python file folder
currentPath = pathlib.Path(__file__).parent.resolve()
os.chdir(currentPath)

# Get command file name (without suffix)
currentFileName = pathlib.Path(__file__).stem

# Read csv file to make table
tableDef = {}
# Read lvgl object style file and generate csv file and convert it to C table
with open(F"{currentPath}/../../.pio/libdeps/esp32-S3TOUCHLCD7/lvgl/src/core/lv_obj_style_gen.h"
    , 'rt', encoding='UTF-8') as inputStream:
    routine = ""
    for line in inputStream.readlines():
        fields = line.strip().split(" ")
        if len(fields) >=6 \
               and fields[0] == "static" \
                and fields[1] == "inline" \
                and fields[2] != "const":
            routine = fields[3].split("(")[0]
            tableDef[routine] = {}
            tableDef[routine]["type"] = fields[2]
        elif fields[0] == "return":
            tableDef[routine]["value"] = line[line.find("v.")+2:-1].split(";")[0]
        elif line.find("LV_STYLE_") >= 0:
            tableDef[routine]["const"] = line[line.find("LV_STYLE_"):].split(")")[0]

with open(currentFileName+'.code', 'wt', encoding='utf-8') as outStream:
    outStream.write('void dumpStyle(const lv_obj_t* objToDump, uint8_t ident) {\n')
    for key in tableDef.items():
        #print(F"{key[0]}={key[1]}")
        if key[0].find("color_filtered") == -1 and key[1]["value"] != "ptr":
            outStream.write('\t{\n')
            outStream.write(F'\t\t{key[1]["type"]} value = {key[0]}(objToDump, LV_PART_MAIN);\n')
            match key[1]["type"]:
                case "lv_coord_t":
                    outStream.write(F'\t\tif (value) Serial.printf("%.*s{key[1]["const"]}=%d\\n", ident, identChar, value);\n')
                case "lv_align_t" | "lv_text_align_t":
                    outStream.write('\t\tif (value) {\n')
                    outStream.write(F'\t\t\tSerial.printf("%.*s{key[1]["const"]}=0x%x (", ident, identChar, value);\n')
                    outStream.write('\t\t\tdecodeValue(value, align_t_text, sizeof(align_t_text) / sizeof(char*));\n')
                    outStream.write('\t\t}\n')
                case "lv_grad_dir_t":
                    outStream.write('\t\tif (value) {\n')
                    outStream.write(F'\t\t\tSerial.printf("%.*s{key[1]["const"]}=0x%x (", ident, identChar, value);\n')
                    outStream.write('\t\t\tdecodeValue(value, grad_dir_t_text, sizeof(grad_dir_t_text) / sizeof(char*));\n')
                    outStream.write('\t\t}\n')
                case "lv_blend_mode_t":
                    outStream.write('\t\tif (value) {\n')
                    outStream.write(F'\t\t\tSerial.printf("%.*s{key[1]["const"]}=0x%x (", ident, identChar, value);\n')
                    outStream.write('\t\t\tdecodeValue(value, blend_mode_t_text, sizeof(blend_mode_t_text) / sizeof(char*));\n')
                    outStream.write('\t\t}\n')
                case "lv_border_side_t":
                    outStream.write('\t\tif (value) {\n')
                    outStream.write(F'\t\t\tSerial.printf("%.*s{key[1]["const"]}=0x%x (", ident, identChar, value);\n')
                    outStream.write('\t\t\tdecodeBitValue(value, border_side_t_text, border_side_t_bits, sizeof(border_side_t_text) / sizeof(char*));\n')
                    outStream.write('\t\t}\n')
                case "lv_text_decor_t":
                    outStream.write('\t\tif (value) {\n')
                    outStream.write(F'\t\t\tSerial.printf("%.*s{key[1]["const"]}=0x%x (", ident, identChar, value);\n')
                    outStream.write('\t\t\tdecodeBitValue(value, text_decor_t_text, text_decor_t_bits, sizeof(text_decor_t_text) / sizeof(char*));\n')
                    outStream.write('\t\t}\n')
                case "lv_base_dir_t":
                    outStream.write('\t\tif (value) {\n')
                    outStream.write(F'\t\t\tSerial.printf("%.*s{key[1]["const"]}=0x%x (", ident, identChar, value);\n')
                    outStream.write('\t\t\tdecodeBitValue(value, base_dir_t_text, base_dir_t_bits, sizeof(base_dir_t_text) / sizeof(char*));\n')
                    outStream.write('\t\t}\n')
                case "uint16_t":
                    outStream.write(F'\t\tif (value) Serial.printf("%.*s{key[1]["const"]}=%d\\n", ident, identChar, value);\n')
                case "lv_opa_t":
                    outStream.write(F'\t\tif (value != 255) Serial.printf("%.*s{key[1]["const"]}=%d\\n", ident, identChar, value);\n')
                case "lv_color_t":
                    outStream.write(F'\t\tif (value.full) Serial.printf("%.*s{key[1]["const"]}=0x%06x\\n", ident, identChar, value.full);\n')
                case "uint32_t":
                    outStream.write(F'\t\tif (value) Serial.printf("%.*s{key[1]["const"]}=%ld\\n", ident, identChar, value);\n')
                case "bool":
                    outStream.write(F'\t\tif (value) Serial.printf("%.*s{key[1]["const"]}=%s\\n", ident, identChar, value ? "true": "false");\n')
                case _:
                    print(F"Can't decode {key[1]["type"]}")
            outStream.write('\t}\n')
    outStream.write('}\n')
