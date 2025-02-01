#include "lvgl.h"
#include "ota.h"
#include "ui/wifi_setup.h"

//https://randomnerdtutorials.com/esp32-web-server-arduino-ide/
//https://docs.arduino.cc/tutorials/uno-wifi-rev2/uno-wifi-r2-hosting-a-webserver/
//https://gist.github.com/elktros/a39d167e55625396ad6df57b89b00ca7

#ifdef PULLOTA
// USE PULL OTA

#include <HTTPClient.h>
#include <Update.h>

const char url[] = "http://192.168.1.81:8080/firmware.bin";
extern lv_obj_t * label_tool;

static int last_callback_time = 0;
void do_update_callback(int offset, int totallength, lv_obj_t * bar)
{

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

                //Display loading bar
                int now = millis();
                if (now - last_callback_time > 1000)
                {
                    last_callback_time = now;

                    float percentage = (float)offset / (float)totalLength; // 0 -> 1
                    lv_bar_set_value(bar1, percentage * 100, LV_ANIM_OFF);

                    lv_refr_now(NULL);
                    lv_timer_handler();
                    lv_task_handler();
                }
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

    http.end();
    lv_label_set_text(label_tool, "OTA Update failed, no file");

}
#endif

#ifdef PUSHOTA
// USE PUSH OTA
// Use URL http://192.168.1.29/update
// Code from https://github.com/pangodream/ESP2SOTA

#include <WebServer.h>
//#include "stdlib_noniso.h"
#include "Update.h"

WebServer server(80);

const char* indexHtml =
    "<body style='font-family: Verdana,sans-serif; font-size: 14px;'>"
    "<div style='width:400px;padding:20px;border-radius:10px;border:solid 2px #e0e0e0;margin:auto;margin-top:20px;'>"
    "<div style='width:100%;text-align:center;font-size:18px;font-weight:bold;margin-bottom:12px;'>ESP 2S OTA</div>"
       "<form method='POST' action='#' enctype='multipart/form-data' id='upload-form' style='width:100%;margin-bottom:8px;'>"
         "<input type='file' name='update'>"
         "<input type='submit' value='Update' style='float:right;'>"
       "</form>"
    "<div style='width:100%;background-color:#e0e0e0;border-radius:8px;'>"
    "<div id='prg' style='width:0%;background-color:#2196F3;padding:2px;border-radius:8px;color:white;text-align:center;'>0%</div>"
    "</div>"
    "</div>"
    "</body>"
    "<script>"
    "var prg = document.getElementById('prg');"
    "var form = document.getElementById('upload-form');"
    "form.addEventListener('submit', e=>{"
         "e.preventDefault();"
         "var data = new FormData(form);"
         "var req = new XMLHttpRequest();"
         "req.open('POST', '/update');"  
         "req.upload.addEventListener('progress', p=>{"
              "let w = Math.round((p.loaded / p.total)*100) + '%';"
              "if(p.lengthComputable){"
                   "prg.innerHTML = w;"
                   "prg.style.width = w;"
              "}"
              "if(w == '100%') prg.style.backgroundColor = '#04AA6D';" 
         "});"
         "req.send(data);"
     "});"
    "</script>";


void OTA_init(void)
{
    /* SETUP YOR WEB OWN ENTRY POINTS */
    server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", "Use /update for OTA!");
    });

  //Returns index.html page
  server.on("/update", HTTP_GET, [&]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", indexHtml);
  });

  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, [&]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, [&]()
  {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN))
      {
        //start with max available size
        Update.printError(Serial);
      }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
      {
        Update.printError(Serial);
      }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        wifi_stop(); // To test.
        delay(1000); // To test.

        if (Update.end(true)) //true to set the size to the current progress
        {
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        }
        else
        {
            Update.printError(Serial);
        }
    }
  });

    server.begin();
}

void OTA_loop(void)
{
  /* HANDLE UPDATE REQUESTS */
  server.handleClient();
}

#endif
