#include "common.h"
#include "protocol/presenter_protocol.h"
#include "protocol/remote_protocol.h"
#include "base64.hpp"
#include <cstdio>

class BridgeDevice: public BaseDevice {

public:

    BridgeDevice(Print* printer, Callback rebootFunc): BaseDevice(printer, rebootFunc) {
        
    };
    void PreSetup() {
        // TODO: how to do serial
        this->printer->printf("Hello from the bridge\n");
    }
    void Setup() {
        
    }

    void MeshOnSend(const uint8_t *mac_addr, uint8_t status) {

    }

    void MeshOnReceive(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
                mac_addr[5]);
        
        // output.concat(macStr);
        // output.concat(' ');
        if (data_len == 0) {
            // Empty message
            // queueSerialOut(output);
            printer->println(macStr);
            return;
        }
        // TODO: figure out how much space I'll need to encode images
        char* base64 = (char*)malloc(10 + data_len);
        unsigned int base64_length = encode_base64(data, data_len, (unsigned char*)base64);
        printer->println(base64);
        free(base64);
        uint8_t type = data[0];
        // If this a message that isn't meant for us, but the presenter
        if (type >= MIN_PRESENTER_MESSAGE_ID) {
            // queueSerialOut(output);
            return;
        }
        // Otherwise it's remote to remote and we should respond
        if (type == BridgeRequest) {
            meshAddPeerFunc((uint8_t*)mac_addr, 0);
            RemoteMessageBridgeResponse_t msg = {.id = BridgeResponse};
            meshSendFunc((uint8_t*)mac_addr, (uint8_t*)&msg, sizeof(msg));
        }
    }
};