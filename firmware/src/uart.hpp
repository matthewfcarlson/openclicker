#include <bridge/bridge.hpp>
#include <Arduino.h>

#pragma once

class BridgeUartTransport : public BridgeTransport {
public:
    BridgeUartTransport() {}

    void CheckForMessages() override {
        // TODO: figure how to run this on the other core
        // Check for messages on the UART
        if (Serial.available() > 0) {
            // Read until we get a new line
            char message[300] = {0};
            uint32_t message_len = 0;
            while (true) {
                while (!Serial.available()) delay(1);
                // TODO: add some sort of timeout mechanism
                char c = Serial.read();
                if (c == '\n') {
                    break;
                }
                message[message_len++] = c;
            }
            message[message_len++] = '\0';
            Serial.printf("Received message: %s\n", message);
            uint8_t data[255] = {0};
            uint32_t data_len = sizeof(data);
            uint8_t from_mac[6] = {0};
            uint8_t to_mac[6] = {0};
            ConvertMessageFromString(from_mac, to_mac, data, &data_len, message, message_len);

            this->bridgeCallback(to_mac, data, data_len);
        }
    }
};