// A definition of the protocol for communicating remote to presenter
#include <stdint.h>
#pragma once
#define MIN_PRESENTER_MESSAGE_ID 20

enum PresenterMessageIds {
    RemoteButtonPressed = MIN_PRESENTER_MESSAGE_ID,
    RemoteRequestState,
    PresenterSetState,
};

typedef struct {
    uint8_t id; // ButtonPressed
    uint8_t button_id; // 0-4
} PresenterRemoteButtonPressed_t;

typedef struct {
    uint8_t id; // RemoteRequestState
} PresenterRemoteRequestState_t;

typedef struct {
    uint8_t id; // PresenterSetState
    char state_name[31];
} PresenterSetState_t;
