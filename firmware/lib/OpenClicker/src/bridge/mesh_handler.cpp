// #include "bridge.h"
// #include "base64.hpp"
// #include "presenter_protocol.h"
// #include "remote_protocol.h"

// void bridgeOnRecv(const uint8_t* mac_addr, const uint8_t *data, int data_len) {
//     String output;
//     char macStr[18];
//     snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
//              mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
//              mac_addr[5]);
    
//     output.concat(macStr);
//     output.concat(' ');
//     if (data_len == 0) {
//         // Empty message
//         queueSerialOut(output);
//         return;
//     }
//     // TODO: figure out how much space I'll need to encode images
//     char* base64 = (char*)malloc(10 + data_len);
//     unsigned int base64_length = encode_base64(data, data_len, (unsigned char*)base64);
//     output.concat(base64);
//     free(base64);
//     Serial.println(output);
//     uint8_t type = data[0];
//     if (type >= MIN_PRESENTER_MESSAGE_ID) {
//         queueSerialOut(output);
//         return;
//     }
//     // Otherwise it's remote to remote and we should respond
//     if (type == BridgeRequest) {
//         ESPNow.add_peer((uint8_t*)mac_addr);
//         RemoteMessageBridgeResponse_t msg = {.id = BridgeResponse};
//         ESPNow.send_message((uint8_t*)mac_addr, (uint8_t*)&msg, sizeof(msg));
//     }
// }
// void bridgeOnSend(const uint8_t* mac_addr, esp_now_send_status_t status){
//      char macStr[18];
//     snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
//              mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
//              mac_addr[5]);
//     Serial.print("Last Packet Recv from: ");
//     Serial.print(macStr);
//     Serial.printf(" Status: %d\n", status);
// }