#include <bridge/bridge.hpp>
#include <emscripten.h>

#pragma once

class JSBridgeTransport : public BridgeTransport {

public:
    JSBridgeTransport() {}
    void SendMessageToPresenter(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_len) override {
        // Serial print this to the console
        char msg[512] = {0};
        this->ConvertMessageToString(mac_addr, PRESENTER_MAC, data, data_len, msg, sizeof(msg));
        EM_ASM_PTR({
            const msg = UTF8ToString($0);
            console.log(msg);
            window['electronAPI']['sendMessageToPresenter'](msg);
        }, msg);
    }
};