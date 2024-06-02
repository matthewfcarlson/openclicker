// A definition of the protocol for communicating remote to presenter
#include <stdint.h>
#pragma once
#define MIN_PRESENTER_MESSAGE_ID 20

#pragma pack(push, 1)

enum PresenterMessageIds {
    RemoteButtonPressed = MIN_PRESENTER_MESSAGE_ID,
    RemoteHeartbeat,
    RemoteRequestState,
    PresenterSetState,
};

typedef struct {
    uint8_t id; // PresenterMessageIds.RemoteButtonPressed
    uint8_t button_id; // 0-4
} PresenterRemoteButtonPressed_t;

typedef struct {
    uint8_t id; // PresenterMessageIds.RemoteHeartbeat
    uint8_t battery_level;
} PresenterRemoteHeartbeat_t;

typedef struct {
    uint8_t id; // PresenterMessageIds.RemoteRequestState
    uint32_t state_hash1;
    uint32_t state_hash2;
    uint32_t state_hash3;
    uint32_t state_hash4;
} PresenterRemoteRequestState_t;

typedef struct {
    uint8_t id; // PresenterMessageIds.PresenterSetState
    char state_name[31];
} PresenterSetState_t;

#pragma pack(pop)