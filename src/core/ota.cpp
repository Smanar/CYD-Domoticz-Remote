#include "lvgl.h"
#include "ota.h"
#include <HTTPClient.h>
#include <Update.h>

// USE PULL OTA


const char url[] = "http://192.168.1.81:8080/firmware.bin";
extern lv_obj_t * label_tool;

static int last_callback_time = 0;
void do_update_callback(int offset, int totallength, lv_obj_t * bar)
{
    int now = millis();
    if (now - last_callback_time < 1000)
    {
        return;
    }

    last_callback_time = now;

    float percentage = (float)offset / (float)totallength; // 0 -> 1
    lv_bar_set_value(bar, percentage * 100, LV_ANIM_OFF);

    lv_refr_now(NULL);
    lv_timer_handler();
    lv_task_handler();
}

void OTAUpdate()
{

    HTTPClient http;
    http.begin(url);

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200)
    {
        int totalLength = http.getSize();

        // this is required to start firmware update process
        //if (!Update.begin(UPDATE_SIZE_UNKNOWN))
        if (!Update.begin(totalLength, U_FLASH))
        {
            lv_label_set_text(label_tool, "Hardware can't Update");
            //lv_obj_invalidate(label_tool);

            return;
        }

        lv_label_set_text(label_tool, "Starting OTA Update");

        // create buffer for read
        uint8_t buff[1280] = { 0 };

        // get tcp stream
        WiFiClient* stream = http.getStreamPtr();

        // Make a scrollbar
        lv_obj_t * cont = lv_obj_get_parent(label_tool);
        lv_obj_t * bar1 = lv_bar_create(cont);
        lv_obj_set_size(bar1, 200, 20);
        lv_obj_center(bar1);
        lv_bar_set_value(bar1, 0, LV_ANIM_ON);

        // read all data from server
        int offset = 0;
        while (http.connected() && offset < totalLength)
        {
            size_t sizeAvail = stream->available();
            if (sizeAvail > 0)
            {
                size_t bytes_to_read = min(sizeAvail, sizeof(buff));
                size_t bytes_read = stream->readBytes(buff, bytes_to_read);
                size_t bytes_written = Update.write(buff, bytes_read);
                if (bytes_read != bytes_written)
                {
                    Serial.printf("Unexpected error in OTA: %d %d %d\n", bytes_to_read, bytes_read, bytes_written);
                    break;
                }
                offset += bytes_written;

                lv_timer_handler();
                lv_task_handler();

                lv_bar_set_value(bar1, offset * 100 / totalLength , LV_ANIM_ON);
                Serial.printf("OTA %d %\n", offset * 100 / totalLength );

                do_update_callback(offset, totalLength, bar1);
            }

        }

        if (offset == totalLength)
        {
            Update.end(true);
            lv_label_set_text(label_tool, "OTA Update finished");
            delay(1000);

            ESP.restart();
        }
        lv_label_set_text(label_tool, "OTA Update failed");
    }
Serial.printf("OTA ok3");
    http.end();
    lv_label_set_text(label_tool, "OTA Update failed, no file");

}
