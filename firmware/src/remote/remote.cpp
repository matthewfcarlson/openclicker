#include "remote.h"
#include "common.h"

uint8_t bridge_mac[] = BRIDGE_MAC_VALUES;
char message[] = "General Kenobi!";

void remoteSetup(void) {
    // Add the bridge as a peer
    ESPNow.add_peer(bridge_mac);
}

void remoteLoop(void) {
    static uint16_t a= 0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_err_t status = ESPNow.send_message(bridge_mac, (uint8_t*)&a, sizeof(a));
    Serial.printf("Sending %d = %x\n", a++, status);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    status = ESPNow.send_message(bridge_mac, (uint8_t*)&message, sizeof(message));
    Serial.printf("Sending %s = %x\n", message, status);
}

void remoteOnSend(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
      char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);
    Serial.print("Last Packet Recv from: ");
    Serial.print(macStr);
    Serial.printf(" Status: %d\n", status);
}