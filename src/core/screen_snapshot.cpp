#include "lvgl.h"

#if LV_USE_SNAPSHOT != 0
    #include "screen_snapshot.h"
    #include "LittleFS.h"
    #include "conf/json_config.h"

    typedef struct {
        uint32_t initial_delay;
        uint32_t task_delay;
    } screenshot_task_params_t;

    typedef struct {
        uint16_t file_type;      // File type identifier (BM)
        uint32_t file_size;      // Size of the BMP file in bytes
        uint16_t reserved1;     // Reserved; must be zero
        uint16_t reserved2;     // Reserved; must be zero
        uint32_t offset_data;    // Offset from beginning of file to the image data
    } __attribute__((packed)) bitmap_fileheader;

    typedef struct {
        uint32_t size;           // Size of this header (40 bytes)
        int32_t  width;          // Image width in pixels
        int32_t  height;         // Image height in pixels
        uint16_t planes;         // Number of color planes 
        uint16_t bit_count;      // Bits per pixel (24 for 24-bit RGB)
        uint32_t compression;    // Compression method (0 for none)
        uint32_t size_image;     // Size of the image data in bytes
        int32_t  x_pels_per_meter; // Horizontal resolution in pixels per meter
        int32_t  y_pels_per_meter; // Vertical resolution in pixels per meter
        uint32_t clr_used;       // Number of colors in the color palette
        uint32_t clr_important;  // Number of important colors used
    } __attribute__((packed)) bitmap_infoheader;

    // Take a snapshot of current screen
    void takeSnapshot(void) {
        lv_img_dsc_t *snap=lv_snapshot_take(lv_scr_act(), LV_IMG_CF_TRUE_COLOR);

        if (snap) {
            // BMP header size calculation
            uint8_t bpp = 24; // Assuming 24-bit BMP
            uint32_t header_size = sizeof(bitmap_fileheader) + sizeof(bitmap_infoheader);
            uint32_t image_size = snap->header.w * snap->header.h * (bpp / 8); 
            uint32_t file_size = header_size + image_size;
        
            Serial.printf("Snapshot %dx%d, file size %d\n", snap->header.w, snap->header.h, file_size);
            Serial.flush();

            // Allocate memory for the BMP data
            uint8_t *bmp_data = (uint8_t *)heap_caps_malloc(file_size, MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
            if (!bmp_data) {
                Serial.println("Failed to allocate memory for BMP data");
                lv_snapshot_free(snap);
                return;
            }
        
            bitmap_fileheader file_header;
            bitmap_infoheader info_header;
        
            file_header.file_type = 0x4D42; // 'BM'
            file_header.file_size = file_size;
            file_header.reserved1 = 0;
            file_header.reserved2 = 0;
            file_header.offset_data = sizeof(bitmap_fileheader) + sizeof(bitmap_infoheader);
        
            info_header.size = sizeof(bitmap_infoheader);
            info_header.width = snap->header.w;
            info_header.height = snap->header.h;
            info_header.planes = 1;
            info_header.bit_count = 24;
            info_header.compression = 0;
            info_header.size_image = image_size;
            info_header.x_pels_per_meter = 0; // You can set these if needed
            info_header.y_pels_per_meter = 0;
            info_header.clr_used = 0;
            info_header.clr_important = 0;
        
            // Write header structures
            memcpy(bmp_data, &file_header, sizeof(bitmap_fileheader));
            memcpy(bmp_data + sizeof(bitmap_fileheader), &info_header, sizeof(bitmap_infoheader));
        
            // Convert LVGL image data to BMP format
            uint8_t *pixel_data = bmp_data + header_size;
            for (int y = snap->header.h - 1; y >= 0; y--) {
        
                for (int x = 0; x < snap->header.w; x++) {
                    lv_color_t color = lv_img_buf_get_px_color(snap, x, y, lv_color_white());
        
                    // 16 Bit to 24 Bit conversion
                    *pixel_data++ = color.ch.red << 3;
                    *pixel_data++ = color.ch.green << 2;
                    *pixel_data++ = color.ch.blue << 3;
                }
            }
            File snapStream = LittleFS.open(SNAPSHOT_FILE, "w");
            size_t writtenBytes = snapStream.write(bmp_data, file_size);
            if (writtenBytes != file_size) {
                Serial.printf("Only %d bytes out of %d written\n", writtenBytes, file_size);
            }
            snapStream.close();
            heap_caps_free(bmp_data);
            lv_snapshot_free(snap);
        } else {
            Serial.println("Can't take snapshoot!");
            return;
        }
    }
#endif