#include <Arduino.h>
#include "common.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif ESP32
#include <WiFi.h>
#endif

// I'm basically implementing a c++ class with pointers, shrug
#ifdef ISBRIDGE
#include "bridge/bridge.h"
#else
#include "remote/remote.h"
#endif

void setup() {
    Serial.begin(115200);
    // If you need to do an pre-setup, call that
    if (routines.preSetup) (*routines.preSetup)();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
#ifdef ESP8266
    WiFi.mode(WIFI_STA); // MUST NOT BE WIFI_MODE_NULL
#elif ESP32
    WiFi.mode(WIFI_MODE_STA);
#endif
    WiFi.disconnect();
    ESPNow.init();
    // Call setup
    if (routines.setup) (*routines.setup)();
    // Setup mesh receivers
    if (routines.onReceive) ESPNow.reg_recv_cb(routines.onReceive);
    if (routines.onSend) ESPNow.reg_send_cb(routines.onSend);
}

void loop() {
    if (routines.loop) (*routines.loop)();
}