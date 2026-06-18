#include "lvgl.h"
#include "ota.h"
#include "ui/wifi_setup.h"
#include "helper.h"
#include "data_setup.h"
#include "../ui/navigation.h"

#if LV_USE_SNAPSHOT != 0
    #include "core/screen_snapshot.h"
#endif

//https://randomnerdtutorials.com/esp32-web-server-arduino-ide/
//https://docs.arduino.cc/tutorials/uno-wifi-rev2/uno-wifi-r2-hosting-a-webserver/
//https://gist.github.com/elktros/a39d167e55625396ad6df57b89b00ca7

extern Device myDevices[];

#ifdef PULLOTA
// USE PULL OTA

#include <HTTPClient.h>
#include <Update.h>

const char url[] = "http://192.168.1.81:8080/firmware.bin";
extern lv_obj_t * label_tool;

static unsigned long last_callback_time = 0;
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
                //lv_task_handler();

                lv_bar_set_value(bar1, offset * 100 / totalLength , LV_ANIM_ON);
                Serial.printf("OTA %d %\n", offset * 100 / totalLength );

                //Display loading bar
                unsigned long now = millis();
                if (now - last_callback_time > 1000)
                {
                    last_callback_time = now;

                    float percentage = (float)offset / (float)totalLength; // 0 -> 1
                    lv_bar_set_value(bar1, percentage * 100, LV_ANIM_OFF);

                    lv_refr_now(NULL);
                    lv_timer_handler();
                    //lv_task_handler();
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
#include "Update.h"
#include "LittleFS.h"
#include "../conf/json_config.h"

WebServer server(80);
File tmpFile;
bool downloadError = false;

const char* indexHtml =
"<!DOCTYPE html>\n"
"<html>\n"
        "<head>\n"
            "<meta charset='utf-8'>\n"
        "</head>\n"
        "<body style='font-family: Verdana,sans-serif; font-size: 14px;'>\n"
            "<div style='width:400px;padding:20px;border-radius:10px;border:solid 2px #e0e0e0;margin:auto;margin-top:20px;'>\n"
                "<div style='width:100%;text-align:center;font-size:18px;font-weight:bold;margin-bottom:12px;'>Write firmware</div>\n"
                "<form method='POST' action='#' enctype='multipart/form-data' id='firmwareForm' style='width:100%;margin-bottom:8px;'>\n"
                    "<input type='file' name='firmware'>\n"
                    "<input type='submit' value='Update' style='float:right;'>\n"
                "</form>\n"
                "<div style='width:100%;background-color:#e0e0e0;border-radius:8px;'>\n"
                    "<div id='firmwareProgress' style='width:0%;background-color:#2196F3;padding:2px;border-radius:8px;color:white;text-align:center;'>0%</div>\n"
                "</div>\n"
            "</div>\n"
            "<div style='width:400px;padding:20px;border-radius:10px;border:solid 2px #e0e0e0;margin:auto;margin-top:20px;'>\n"
                "<div style='width:100%;text-align:center;font-size:18px;font-weight:bold;margin-bottom:12px;'>Write configuration</div>\n"
                "<form method='POST' action='#' enctype='multipart/form-data' id='configForm' style='width:100%;margin-bottom:8px;'>\n"
                    "<input type='file' name='config'>\n"
                    "<input type='submit' value='Send' style='float:right;'>\n"
                "</form>\n"
                "<div style='width:100%;background-color:#e0e0e0;border-radius:8px;'>\n"
                    "<div id='configProgress' style='width:0%;background-color:#2196F3;padding:2px;border-radius:8px;color:white;text-align:center;'>0%</div>\n"
                "</div>\n"
            "</div>\n"
            "<div style='width:400px;padding:20px;border-radius:10px;border:solid 2px #e0e0e0;margin:auto;margin-top:20px;'>\n"
                "<div style='width:100%;text-align:center;font-size:18px;font-weight:bold;margin-bottom:12px;'>Read configuration</div>\n"
                "<div style='text-align:center;'>\n"
                    "<input type='button' value='Read' onclick='readConfig();'/>\n"
                "</div>\n"
            "</div>\n"
        "</body>\n"
        "<script>\n"
            "function readConfig() {\n"
                "const link = document.createElement('a');\n"
                "link.href = '/readConfig';\n"
                "link.download = 'settings.json'\n"
                "document.body.appendChild(link);\n"
                "link.click();\n"
                "document.body.removeChild(link);\n"
            "};\n"
            "var firmwareProgress = document.getElementById('firmwareProgress');\n"
            "var firmwareForm = document.getElementById('firmwareForm');\n"
            "firmwareForm.addEventListener('submit', e=>{\n"
                "e.preventDefault();\n"
                "var data = new FormData(firmwareForm);\n"
                "var req = new XMLHttpRequest();\n"
                "req.open('POST', '/writeFirmware');\n"
                "req.upload.addEventListener('progress', p=>{\n"
                    "let w = Math.round((p.loaded / p.total)*100) + '%';\n"
                    "if(p.lengthComputable){\n"
                        "firmwareProgress.innerHTML = w;\n"
                        "firmwareProgress.style.width = w;\n"
                    "}\n"
                    "if(w == '100%') firmwareProgress.style.backgroundColor = '#04AA6D';\n"
                "});\n"
                "req.send(data);\n"
            "});\n"
            "var configProgress = document.getElementById('configProgress');\n"
            "var configForm = document.getElementById('configForm');\n"
            "configForm.addEventListener('submit', e=>{\n"
                "e.preventDefault();\n"
                "var data = new FormData(configForm);\n"
                "var req = new XMLHttpRequest();\n"
                "req.open('POST', '/writeConfig');\n"
                "req.upload.addEventListener('progress', p=>{\n"
                    "let w = Math.round((p.loaded / p.total)*100) + '%';\n"
                    "if(p.lengthComputable){\n"
                        "configProgress.innerHTML = w;\n"
                        "configProgress.style.width = w;\n"
                    "}\n"
                    "if(w == '100%') configProgress.style.backgroundColor = '#04AA6D';\n"
                "});\n"
                "req.send(data);\n"
            "});\n"
        "</script>\n"
    "</html>\n"
;

void OTA_init(void)
{
    /* SETUP FOR WEB OWN ENTRY POINTS */
    server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", 
            "Use /update for OTA\n"
            "    /readConfig to get configuration\n"
            "    /restart to reboot\n"
            "    /debug to get information\n"
        #if LV_USE_SNAPSHOT != 0
            "    /snapshot to get a screen copy\n"
        #endif
        );
    });

  //Returns index.html page
  server.on("/update", HTTP_GET, [&]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", indexHtml);
  });

  /* Handling uploading firmware file */
  server.on("/writeFirmware", HTTP_POST, [&]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    delay(1000);
    ESP.restart();
  }, [&]()
  {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START){
        Serial.printf("Update firmware: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            //start with max available size
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        /* flashing firmware to ESP*/
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Firmware ok, size %u\nRebooting...\n", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
    }
  });

  /* Handling uploading configuration file */
  server.on("/writeConfig", HTTP_POST, [&]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", downloadError ? "FAIL" : "OK");
    if (!downloadError) {
        delay(1000);
        ESP.restart();
    }
    }, [&]()
  {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {                       // We're starting file upload
        Serial.printf("Download: %s\n", upload.filename.c_str());
        downloadError = false;                                      // Clear error flag
        tmpFile = LittleFS.open(TMP_FILE, "w");                     // Open a temporary file to avoid destroying target if error in process
        if (!tmpFile) downloadError = true;                         // Error openiung temp file
    } else if (upload.status == UPLOAD_FILE_WRITE) {                // We're loading a buffer
        if (!downloadError) {
            if (tmpFile.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Serial.printf("Error writting %s\n", TMP_FILE);
                tmpFile.close();
                LittleFS.remove(TMP_FILE);
                downloadError = true;
            }
        }
    } else if (upload.status == UPLOAD_FILE_END) {                  // We're at end of file
        if (!downloadError) {                                       // No error seen
            tmpFile.flush();
            tmpFile.close();
            if (checkJsonConfig((char *) TMP_FILE)) {               // Is received file a correct one?
                Serial.print("Upload ok\nRebooting...\n");
                LittleFS.remove(SETTINGS_FILE);                     // Delete existing settings file
            } else {
                downloadError = true;                               // File is bad
                LittleFS.remove(TMP_FILE);                          // Delete it
            }

        } else {                                                    // We got an error
            if (tmpFile) {                                          // Do we open a file?
                tmpFile.close();                                    // Close it
                LittleFS.remove(TMP_FILE);                          // Delete it
            }
        }
    }
  });

  /* Handling uploading configuration file */
  server.on("/readConfig", HTTP_GET, [&]() {
    writeJsonConfig();                                              // Create file
    File file = LittleFS.open(SETTINGS_FILE, "r");                  // Open the file
    if (file) {                                                     // File opened?
        server.streamFile(file, "application/json");                // Send it to the client
        file.close();
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", "OK");
    } else {                                                        // Error opening file
        server.sendHeader("Connection", "close");
        server.send(404, "text/plain", "File not found");
    }
  });

  /* Handling ESP restart */
  server.on("/restart", HTTP_GET, [&]() {
        Serial.println("/restart received");
        delay(1000);
        ESP.restart();
    });

    server.on("/debug", HTTP_GET, [&]() {
        Serial.println("/debug received");
        server.sendHeader("Connection", "close");
        JsonDocument debugInfo;

        char text[350];
        loadInfo(text, sizeof(text));
        char* oneLine = strtok(text, "\n");
        int i = 0;
        while (oneLine != NULL) {
            debugInfo["state"][i++] = oneLine;
            oneLine = strtok(NULL, "\n");
        }
        debugInfo["page"]["panel"] = GetActivePanel();
        getPanelName(GetActivePanel(), text, sizeof(text));
        debugInfo["page"]["name"] = text;
        if (isActivePanel(GetActivePanel())) {
            debugInfo["page"]["page"] = GetActiveWidgetPage();
            for (i=0; i < TOTAL_ICONX*TOTAL_ICONY; i++) {
                getType(myDevices[i].type, text, sizeof(text));
                debugInfo["page"]["device"][i]["idx"] = myDevices[i].idx;
                debugInfo["page"]["device"][i]["ID"] = myDevices[i].ID;
                debugInfo["page"]["device"][i]["name"] = myDevices[i].name;
                debugInfo["page"]["device"][i]["type"] = myDevices[i].type;
                debugInfo["page"]["device"][i]["typeName"] = text;
                debugInfo["page"]["device"][i]["data"] = myDevices[i].data;
                debugInfo["page"]["device"][i]["lenData"] = myDevices[i].lenData;
                debugInfo["page"]["device"][i]["level"] = myDevices[i].level;
                debugInfo["page"]["device"][i]["levelname"] = myDevices[i].levelname;
                debugInfo["page"]["device"][i]["maxlevel"] = myDevices[i].maxlevel;
            }
        }
        String buffer;
        serializeJsonPretty(debugInfo, buffer);
        server.send(200, "application/json", buffer);
        });

    #if LV_USE_SNAPSHOT != 0
    /* Handling screen snapshot */
    server.on("/snapshot", HTTP_GET, [&]() {
            Serial.println("/snapshot received");
            takeSnapshot();
            File file = LittleFS.open(SNAPSHOT_FILE, "r");                  // Open the file
            if (file) {                                                     // File opened?
                server.streamFile(file, "image/bmp");                       // Send it to the client
                file.close();
                server.sendHeader("Connection", "close");
                server.send(200, "text/plain", "OK");
            } else {                                                        // Error opening file
                server.sendHeader("Connection", "close");
                server.send(404, "text/plain", "File not found");
            }
        });
    #endif

    server.begin();
}

void OTA_loop(void)
{
  /* HANDLE UPDATE REQUESTS */
  server.handleClient();
}

#endif
