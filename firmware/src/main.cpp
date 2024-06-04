#ifndef NATIVE
#include <Arduino.h>
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
BaseDevice* device = new BridgeDevice(&Serial, abort);
#else
#include "remote/remote.hpp"
BaseDevice* device = new RemoteDevice(&Serial, abort);
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
    esp_wifi_get_mac((wifi_interface_t)ESP_IF_WIFI_STA, mac_address);
    device->AssignMacAddress(mac_address);
    device->PreSetup();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    esp_now_init();
    device->MeshRegisterSend(esp_now_send);
    device->MeshRegisterAddPeer(wrapper_esp_add_peer);
    // Call setup
    device->Setup();
    // Setup mesh receivers
    esp_now_register_recv_cb(wrapper_esp_receive_callback);
    esp_now_register_send_cb(wrapper_esp_send_callback);
}

void loop() {
    device->Loop();
}
#else
// This is just the bare minimum to get native to compile
// We just use the unit testing for native
int main( int argc, char **argv) {
   return 0;
}
#endif