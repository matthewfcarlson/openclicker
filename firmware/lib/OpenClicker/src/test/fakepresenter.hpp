#include <common/common.h>
#include <functional>
#include <map>
#include <inttypes.h>
#include <bridge/bridge.hpp>
#include <vector>
#include "protocol/presenter_protocol.h"
#include "protocol/remote_protocol.h"

#pragma once

class FakePresenterTransport : public BridgeTransport {

public:
    FakePresenterTransport() {}

    void SendMessageToPresenter(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_len) override {
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
        if (data_len == 0) {
            printf("[FakePresenter] Empty message: %s \n", macStr);
        }
        uint8_t msgType = data[0];
        if (msgType == RemoteRequestState) {
            printf("[FakePresenter] sending dark to %s\n", macStr);
            // Queue a message to this user
            PRESENTER_PRESENTERSETSTATE(setState, "dark");
            this->bridgeCallback(mac_addr, (uint8_t*)&setState, sizeof(setState));
        }
    }
   
};