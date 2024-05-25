#include "bridge.h"
#include "base64.hpp"

void bridgeOnRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    String output;
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);
    output.concat(macStr);
    output.concat(' ');
    char* base64 = (char*)malloc(data_len * 2);
    unsigned int base64_length = encode_base64(data, data_len, (unsigned char*)base64);
    output.concat(base64);
    free(base64);
    Serial.println(output);
    queueSerialOut(output);
}
void bridgeOnSend(const uint8_t *mac_addr, esp_now_send_status_t status){
     char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);
    Serial.print("Last Packet Recv from: ");
    Serial.print(macStr);
    Serial.printf(" Status: %d\n", status);
}