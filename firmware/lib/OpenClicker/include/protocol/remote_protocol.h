// A definition of the protocol for communicating remote to remote
#include <stdint.h>
#pragma once

#pragma pack(push, 1)

enum RemoteMessageIds {
    BridgeRequest = 1,
    BridgeResponse
};

typedef struct {
    uint8_t id;
} RemoteMessageBridgeRequest_t;

typedef struct {
    uint8_t id;
} RemoteMessageBridgeResponse_t;

#pragma pack(pop)
