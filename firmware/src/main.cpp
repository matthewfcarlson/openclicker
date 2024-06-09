#ifndef NATIVE
#include <Arduino.h>
#include "OneButton.h"
#include "common/common.h"
#include <esp_system.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <cstring>
#include <list>

// I'm basically implementing a c++ class with pointers, shrug
#ifdef ISBRIDGE
#include "bridge/bridge.hpp"
#include "s2_pin_config.h"
BaseDevice* device = new BridgeDevice(&Serial, abort);
#else
#include "remote/remote.hpp"
#include "s3_pin_config.h"
BaseDevice* device = new RemoteDevice(&Serial, abort);
#endif

OneButton button1(PIN_BUTTON_1, true);
OneButton button2(PIN_BUTTON_2, true);
// OneButton button2(PIN_BUTTON_3, true);
// OneButton button2(PIN_BUTTON_4, true);

#if defined(LCD_MODULE_CMD_1)
typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},

};
#endif

// Helpful wrapper to make creating a peer easier
int wrapper_esp_add_peer(const uint8_t* mac, uint8_t channel) {
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, mac, sizeof(peer.peer_addr));
    peer.channel = channel;
    peer.ifidx = WIFI_IF_STA;
    return esp_now_add_peer(&peer);
}

void wrapper_esp_receive_callback(const uint8_t* mac_addr, const uint8_t *data, int data_len) {
    device->MeshOnReceive(mac_addr, data, (uint32_t)data_len);
}

void wrapper_esp_send_callback(const uint8_t* mac_addr, esp_now_send_status_t status) {
    device->MeshOnSend(mac_addr, status);
}

void setup() {
    Serial.begin(115200);
    // If you need to do an pre-setup, call that
    uint8_t mac_address[6] = {0};
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    Serial.println("Starting ESP Board");
    esp_wifi_get_mac((wifi_interface_t)ESP_IF_WIFI_STA, mac_address);
    uint8_t primaryChan = 0;
    wifi_second_chan_t secondChan = WIFI_SECOND_CHAN_NONE;
    esp_wifi_get_channel(&primaryChan, &secondChan);
    Serial.printf("WiFi is on channel %d\n", primaryChan);
    device->AssignMacAddress(mac_address);
    device->PreSetup();
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    esp_now_init();
    wrapper_esp_add_peer(BROADCAST_MAC, MESH_DEFAULT_CHANNEL);
    device->MeshRegisterSend(esp_now_send);
    device->MeshRegisterAddPeer(wrapper_esp_add_peer);
    // Call setup
    device->Setup();
    // Setup mesh receivers
    esp_now_register_recv_cb(wrapper_esp_receive_callback);
    esp_now_register_send_cb(wrapper_esp_send_callback);

    button1.attachClick([]() {
        device->ButtonPressed(0);
    });
    button2.attachClick([]() {
        device->ButtonPressed(1);
    });
}

void loop() {
    button1.tick();
    button2.tick();
    device->Loop();
}
#else
// This is just the bare minimum to get native to compile
// We just use the unit testing for native
int main( int argc, char **argv) {
   return 0;
}
#endif