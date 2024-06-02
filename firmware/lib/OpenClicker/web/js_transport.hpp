#include <bridge/bridge.hpp>
#include <emscripten.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void presenter_relay(const char* msg);
#ifdef __cplusplus
}
#endif

class JSBridgeTransport : public BridgeTransport {

public:
    JSBridgeTransport() {}
    void SendMessageToPresenter(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_len) override {
        // Serial print this to the console
        char msg[255] = {0};
        this->ConvertMessageToString(mac_addr, data, data_len, msg, sizeof(msg));
        printf("js bridge: %s\n", msg);
        presenter_relay(msg);
    }
};