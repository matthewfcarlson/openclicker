#include "common/common.h"
#include "protocol/presenter_protocol.h"
#include "protocol/remote_protocol.h"
#include <cstdio>
#include "base64.h"
#include "assert.h"

#pragma once

class BridgeTransport {
protected:
    MeshReceive_t bridgeCallback = nullptr;
public:
    void MacToString(const uint8_t* mac_addr, char* mac_str, uint32_t mac_str_size) {
        bzero(mac_str, mac_str_size);
        snprintf(mac_str, mac_str_size, "%02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
                mac_addr[5]);
    }
    const char* ReadMacFromString(const char* str, uint32_t* str_size, uint8_t* mac_addr) {
        assert(str != nullptr);
        assert(str_size != nullptr);
        assert(mac_addr != nullptr);
        assert(*str_size != 0);

        uint32_t current_msg_size = *str_size;
        if (current_msg_size < 17) {
            printf("Failed to get a long enough message %d\n", current_msg_size);
            return nullptr;
        }

        for (uint8_t i = 0; i < 6; i++) {
            char hexByte[3] = {0};
            // Copy two characters for the hex byte
            hexByte[0] = *str++;
            hexByte[1] = *str++;
            current_msg_size -= 2;
            
            // Move past the colon separator if it's not the last byte
            if (i < 5) {
                if (*str != ':') {
                    printf("Unexpected character of %c\n", *str);
                    return nullptr;
                }
                current_msg_size -= 1;
                str++;
            }

            // Convert hex byte to integer
            mac_addr[i] = (uint8_t)strtol(hexByte, NULL, 16);
        }
        *str_size = current_msg_size;
        return str;
    }
    void ConvertMessageToString(const uint8_t* from_mac_addr, const uint8_t* to_mac_addr, const uint8_t *data, uint32_t data_len, char* msg, uint32_t msg_size) {
        // Convert a message to a string
        // TODO: explode in a better way
        char macStr[18];
        MacToString(from_mac_addr, macStr, sizeof(macStr));
        char macStr2[18];
        MacToString(to_mac_addr, macStr2, sizeof(macStr));
        CreateMessageToString(macStr, macStr2, data, data_len, msg, msg_size);
    }
    void CreateMessageToString(const char* from_mac, const char* to_mac, const uint8_t *data, uint32_t data_len, char* msg, uint32_t msg_size) {
        // Convert a message to a string
        // TODO: explode in a better way
        if (msg == nullptr || msg_size < 3) return;
        bzero(msg, msg_size);
        strncat(msg, "MSG=", msg_size);
        strncat(msg, from_mac, msg_size);
        strncat(msg, "->", msg_size);
        strncat(msg, to_mac, msg_size);
        strncat(msg, " ", msg_size);
        size_t base64_len = 0;
        unsigned char* base64_str = base64_encode(data, data_len, &base64_len);
        strncat(msg, (const char*)base64_str, msg_size);
        free(base64_str);
    }
    bool ConvertMessageFromString(uint8_t* from_mac_addr, uint8_t* to_mac_addr, uint8_t *data, uint32_t* data_len, const char* msg, const uint32_t msg_size) {
        // convert a message from a string
        // Check if the first four chars are MSG=
        // TODO: explode in a better way
        uint32_t current_msg_size = msg_size;
        if (msg == nullptr || current_msg_size < 4) return false;
        if (strncmp(msg, "MSG=", 4) != 0) {
            printf("STRNCMP failed %s\n", msg);
            return false;
        }
        msg += 4;
        current_msg_size -= 4;
        // Next parse the mac address
        if (current_msg_size < 18) {
            printf("Failed to get a long enough message %d\n", current_msg_size);
            return false;
        }

        msg = ReadMacFromString(msg, &current_msg_size, from_mac_addr);
        if (msg == nullptr ) {
            printf("Could not read first mac\n");
            return false;
        }
        if (current_msg_size < 2 || strncmp(msg, "->", 2) != 0) {
            printf("Did not find -> at %s\n", msg);
            return false;
        }
        msg += 2;
        current_msg_size -= 2;

        msg = ReadMacFromString(msg, &current_msg_size, to_mac_addr);
        if (msg == nullptr) {
            printf("Could not read second mac\n");
            return false;
        }
        if (current_msg_size < 1) {
            printf("Expected more characters after second mac\n");
            return false;
        }

        if (*msg != ' ') {
            printf("Unexpected character of %c\n", *msg);
            return false;
        }
        msg++;
        current_msg_size -= 1;
        size_t decoded_data_size = 0;
        unsigned char* decoded_data = base64_decode((unsigned char*)msg, current_msg_size, &decoded_data_size);
        // Check that data is big enough
        if (data == nullptr || *data_len < decoded_data_size) {
            printf("Output data is too small\n");
            return false;
        }
        memcpy(data, decoded_data, decoded_data_size);
        *data_len = decoded_data_size;
        free(decoded_data);
        return true;
    }
public:
    BridgeTransport() {};
    virtual void SendMessageToPresenter(const uint8_t* from_mac_addr, const uint8_t *data, uint32_t data_len) {
        // Serial print this to the console
        char message[512] = {0};
        ConvertMessageToString(from_mac_addr, PRESENTER_MAC, data, data_len, message, sizeof(message));
        printf("PRESENTER %d: %s\n", data[0], message);
    }

    virtual void SendTextMessageToBridge(const char* msg, uint32_t msg_size) {
        uint8_t data[255] = {0};
        uint8_t from_mac[6] = {0};
        uint8_t to_mac[6] = {0};
        uint32_t data_size = sizeof(data);
        if (ConvertMessageFromString(from_mac, to_mac, data, &data_size, msg, msg_size)) {
            this->bridgeCallback(to_mac, data, data_size);
        }
        else {
            printf("Failed to convert message\n");
        }
    }

    void RegisterReceiveCallback(MeshReceive_t callback) {
        this->bridgeCallback = callback;
    }

};

class BridgeDevice: public BaseDevice {
private:
    BridgeTransport* transport;

public:

    BridgeDevice(Print* printer, Callback rebootFunc, BridgeTransport* transport = nullptr, const uint8_t* mac = nullptr): BaseDevice(printer, rebootFunc, mac) {
        if (transport == nullptr ){
            // Create default serial transport
            transport = new BridgeTransport();
        }
        auto callback = std::bind(&BridgeDevice::PresenterOnReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        transport->RegisterReceiveCallback(callback);
        this->transport = transport;
    };

    void PreSetup() override {
        // TODO: how to do serial
        this->printer->printf("Hello from the bridge\n");
    }
    void Setup() override {
        
    }

    void Loop() override {

    }

    void PresenterOnReceive(const uint8_t* mac_addr, const uint8_t* data, uint32_t data_len) {
        // TODO: check if this is a message for us!
        // if so, MeshOnReceive this message
        // otherwise we get a message from the presenter, send it out
        this->printer->printf("Got Message from presenter of %d\n", data_len);
        int status = MeshSend(mac_addr, data, data_len);
        if (status != MESH_OK) {
            this->printer->printf("Failed to send presenter message: %x\n", status);
        }
    }

    void MeshOnSend(const uint8_t* mac_addr, uint8_t status) override {

    }

    void MeshOnReceive(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_len) override {
        
        // output.concat(macStr);
        // output.concat(' ');
        if (data_len == 0) {
            // Empty message
            this->printer->printf("Warning: empty message!\n");
            return;
        }
        uint8_t type = data[0];
        // Check if it's remote to remote and we should respond
        if (type == BridgeRequest) {
            MeshAddPeer(mac_addr, 0);
            this->printer->printf("Received BridgeRequest\n");
            RemoteMessageBridgeResponse_t msg = {.id = BridgeResponse};
            int status = MeshSend(mac_addr, (uint8_t*)&msg, sizeof(msg));
            if (status != MESH_OK){
                this->printer->printf("Failed to send BridgeResponse = %x\n", status);
            }
        }
        else {
            this->printer->printf("Forwarding message to bridge: %d\n", type);
            transport->SendMessageToPresenter(mac_addr, data, data_len);
        }
    }

    MeshReceive_t CreateReceiveCallback() override {
        return std::bind(&BridgeDevice::MeshOnReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    void ButtonPressed(uint8_t index) override {
        PRESENTER_BRIDGEBUTTONPRESSED(msg, index);
        // PresenterBridgeButtonPressed_t msg = {
        //     .id = BridgeButtonPressed,
        //     .button_id = index
        // };
        this->printer->printf("Button %d pressed\n", index);
        transport->SendMessageToPresenter(this->macAddress, (const uint8_t*)&msg, sizeof(msg));
    }
};