// A definition of the protocol for communicating remote to remote
#include <stdint.h>

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
