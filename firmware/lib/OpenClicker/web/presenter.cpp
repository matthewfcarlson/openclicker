#include <emscripten.h>
// This is a list of functions that get pulled into the presenter.js
#include <protocol/presenter_protocol.h>
#include <bridge/bridge.hpp>
#include "helpers.hpp"
#include <remote/little_states/little_state_factory.hpp>

char* presenter_message_to_json(const uint8_t* message, const uint32_t message_size, char* from_mac, char* to_mac);
char* presenter_data_to_json(const uint8_t* message, const uint32_t message_size);

extern "C" const char* message_string_to_json(const char* message) {
    uint8_t from_mac[6] = {0};
    uint8_t to_mac[6] = {0};
    uint8_t data[255];
    uint32_t data_len = sizeof(data);
    bool status = transport->ConvertMessageFromString(from_mac, to_mac, data, &data_len, message, strlen(message));
    if (!status) {
        return "{'error': 'Failed to deserialize message'}";
    }
    char from_mac_str[18];
    char to_mac_str[18];
    transport->MacToString(from_mac, from_mac_str, sizeof(from_mac_str));
    transport->MacToString(to_mac, to_mac_str, sizeof(to_mac_str));
    char* result = presenter_message_to_json(data, data_len, from_mac_str, to_mac_str);
    if (result) return result;
    return "{'error': 'Failed to convert message'}";
}

extern "C" const char* message_json_to_string(const char* to_mac_str, const char* json) {
    printf("Converting message from JSON: %s %s. Temporarily saying DARK STATE\n", to_mac_str, json);
    // TODO: create like a unique function for each message type that takes in a to_mac_str and a list of parameters?
    PRESENTER_PRESENTERSETSTATE(data, "dark");
    uint32_t data_len = sizeof(data);
    uint32_t output_size = 255;
    char* output = (char*)malloc(output_size);
    bzero(output, output_size);
    transport->CreateMessageToString(PRESENTER_MAC_STR, to_mac_str, (const uint8_t*)&data, data_len, output, output_size);
    return output;
}

extern "C" const char* generate_base64_little_state_hash_json() {
    PRESENTER_REMOTEREQUESTSTATE(msg, 0,0,0,0);
    LittleStateFactory* factory = new LittleStateFactory(VoidPrint);
    factory->GenerateLittleStateBloomHashes(&msg.state_hash1, &msg.state_hash2, &msg.state_hash3, &msg.state_hash4);
    uint8_t* data = (uint8_t*)&msg;
    uint32_t data_len = sizeof(msg);
    char* result = presenter_data_to_json(data, data_len);
    if (result) return result;
    return "{ }";
}