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
#include "uart.hpp"
#include "s2_pin_config.h"
BridgeTransport* transport = new BridgeUartTransport();
BaseDevice* device = new BridgeDevice(&Serial, abort, transport);
#else
#include <esp_adc_cal.h>
#include "remote/remote.hpp"
#include "tft.hpp"
uint32_t read_battery() {
    esp_adc_cal_characteristics_t adc_chars;
    // Get the internal calibration value of the chip
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    uint32_t raw = analogRead(PIN_BAT_VOLT);
    uint32_t v1 = esp_adc_cal_raw_to_voltage(raw, &adc_chars) * 2; //The partial pressure is one-half
    return v1;
}
RemoteDevice* remote = new RemoteDevice(&Serial, abort, nullptr, new RemoteGraphicsTFT());
BaseDevice* device = remote;
#endif

OneButton button1(PIN_BUTTON_1, true);
OneButton button2(PIN_BUTTON_2, true);
// OneButton button2(PIN_BUTTON_3, true);
// OneButton button2(PIN_BUTTON_4, true);


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

#ifdef PIN_POWER_ON
    // This is a power pin, if we ever need to shutdown the board, just set this pin to low
    // https://github.com/Xinyuan-LilyGO/T-Display-S3/issues/155
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
#endif
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
#ifndef ISBRIDGE
    remote->RegisterBatteryFunc(read_battery);
    remote->RegisterMinHeapFunc(esp_get_minimum_free_heap_size);
    remote->RegisterFreeHeapFunc(esp_get_free_heap_size);
#endif
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