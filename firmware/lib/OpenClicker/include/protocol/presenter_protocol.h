// A definition of the protocol for communicating remote to presenter
#include <stdint.h>
#pragma once
#define MIN_PRESENTER_MESSAGE_ID 20

enum PresenterMessageIds {
    ButtonPressed = MIN_PRESENTER_MESSAGE_ID,
};

typedef struct {
    uint8_t id;
    uint8_t button_id; // 0-4
} PresenterMessageButtonPressed;
