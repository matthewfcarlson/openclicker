#include "common/common.h"
#include "protocol/presenter_protocol.h"
#include "protocol/remote_protocol.h"
#include <cstdio>

#pragma once

class BridgeTransport {
protected:
    MeshReceive_t bridgeCallback = nullptr;

    void ConvertMessageToString(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_len, char* msg, uint32_t msg_size) {
        // Convert a message to a string
        // TODO: explode in a better way
        if (msg == nullptr || msg_size < 3) return;
        char macStr[18];
        bzero(msg, msg_size);
        strncat(msg, "MSG=", msg_size);
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
                mac_addr[5]);
        strncat(msg, macStr, msg_size);
        // TODO: figure out how much space I'll need to encode images
        // char* base64 = (char*)malloc(10 + data_len);
        // unsigned int base64_length = encode_base64(data, data_len, (unsigned char*)base64);
        // printer->println(base64);
        // free(base64);
    }
    void ConvertMessageFromString(uint8_t* mac_addr, uint8_t *data, uint32_t* data_len, const char* msg, const uint32_t msg_size) {
        // convert a message from a string
        // Check if the first four chars are MSG=
        // TODO: explode in a better way
        uint32_t current_msg_size = msg_size;
        if (msg == nullptr || current_msg_size < 4) return;
        if (strncmp(msg, "MSG=", current_msg_size) != 0) return;
        msg += 4;
        current_msg_size -= 4;
        // Next parse the mac address
        if (msg_size < 18) return;
    }
public:
    BridgeTransport() {};
    virtual void SendMessageToPresenter(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_len) {
        // Serial print this to the console
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
                mac_addr[5]);
        // TODO: figure out how much space I'll need to encode images
        // char* base64 = (char*)malloc(10 + data_len);
        // unsigned int base64_length = encode_base64(data, data_len, (unsigned char*)base64);
        // printer->println(base64);
        // free(base64);
        printf("PRESENTER: \n");
    }

    virtual void SendTextMessageToBridge(const char* msg, uint32_t msg_size) {
        uint8_t data[255] = {0};
        uint8_t mac[6] = {0};
        uint32_t data_size = sizeof(data);
        ConvertMessageFromString(mac, data, &data_size, msg, msg_size);
        this->bridgeCallback(mac, data, data_size);
    }

    void RegisterReceiveCallback(MeshReceive_t callback) {
        this->bridgeCallback = callback;
    }

};

class BridgeDevice: public BaseDevice {
private:
    BridgeTransport* transport;

public:

    BridgeDevice(Print* printer, Callback rebootFunc, BridgeTransport* transport = nullptr): BaseDevice(printer, rebootFunc) {
        if (transport == nullptr ){
            // Create default serial transport
            transport = new BridgeTransport();
        }
        auto callback = std::bind(&BridgeDevice::PresenterOnReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        transport->RegisterReceiveCallback(callback);
        this->transport = transport;
    };
    void PreSetup() {
        // TODO: how to do serial
        this->printer->printf("Hello from the bridge\n");
    }
    void Setup() {
        
    }

    void Loop() {

    }

    void PresenterOnReceive(const uint8_t* mac_addr, const uint8_t* data, uint32_t data_len) {
        // TODO: check if this is a message for us!
        // if so, MeshOnReceive this message
        // otherwise we get a message from the presenter, send it out
        MeshSend(mac_addr, data, data_len);
    }

    void MeshOnSend(const uint8_t* mac_addr, uint8_t status) {

    }

    void MeshOnReceive(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_len) {
        
        // output.concat(macStr);
        // output.concat(' ');
        if (data_len == 0) {
            // Empty message
            this->printer->printf("Warning: empty message!\n");
            return;
        }
        uint8_t type = data[0];
        // If this a message that isn't meant for us, but the presenter
        if (type >= MIN_PRESENTER_MESSAGE_ID) {
            transport->SendMessageToPresenter(mac_addr, data, data_len);
            return;
        }
        // Otherwise it's remote to remote and we should respond
        if (type == BridgeRequest) {
            MeshAddPeer(mac_addr, 0);
            RemoteMessageBridgeResponse_t msg = {.id = BridgeResponse};
            MeshSend(mac_addr, (uint8_t*)&msg, sizeof(msg));
        }
        else {
            this->printer->printf("Unknown message %d\n", type);
        }
    }

    MeshReceive_t CreateReceiveCallback() {
        return std::bind(&BridgeDevice::MeshOnReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }
};