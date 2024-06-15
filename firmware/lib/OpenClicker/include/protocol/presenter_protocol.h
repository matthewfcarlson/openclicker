/**
 AUTOGENERATED ON 2024-06-15 13:03:38.300726
 DO NOT MODIFY THIS FILE MANUALLY
 IT WILL BE OVERWRITTEN ON THE NEXT BUILD
 Use the generate.py script in the protocol folder to regenerate this file
**/

#pragma once

#pragma pack(push, 1)
#ifndef __PRESENTER_PROTOCOL_H__
#define __PRESENTER_PROTOCOL_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#define MIN_PRESENTER_MESSAGE_ID 20
enum PresenterMessageId {
    RemoteButtonPressed = 20,
    RemoteRequestEmojis = 21,
    PresenterOfferEmojiOptions = 22,
    RemotePickEmojis = 23,
    PresenterAssignEmojis = 24,
    BridgeButtonPressed = 25,
    RemoteHeartBeat = 26,
    RemoteRequestState = 27,
    PresenterSetState = 28,
    PresenterRemotePacket = 29,
};

#define LITTE_STATE_DARKSTATE_NAME "dark"
#define LITTE_STATE_DARKSTATE_LENGTH 4
#define LITTE_STATE_MULTIPLECHOICESTATE_NAME "mc"
#define LITTE_STATE_MULTIPLECHOICESTATE_LENGTH 2

typedef struct {
    uint8_t id; // RemoteButtonPressed
    uint8_t button_id; // 8bit unsigned integer between 0 and 4
} PresenterProtocolRemoteButtonPressed_t;

#define PRESENTER_REMOTEBUTTONPRESSED(_0,_1) \
    PresenterProtocolRemoteButtonPressed_t _0 = { \
        .id = RemoteButtonPressed, \
        .button_id = _1, \
    };

typedef struct {
    uint8_t id; // RemoteRequestEmojis
} PresenterProtocolRemoteRequestEmojis_t;

#define PRESENTER_REMOTEREQUESTEMOJIS(_0) \
    PresenterProtocolRemoteRequestEmojis_t _0 = { \
        .id = RemoteRequestEmojis, \
    };

typedef struct {
    uint8_t id; // PresenterOfferEmojiOptions
    uint32_t emoji_choice_1; // 32bit unsigned integer
    uint32_t emoji_choice_2; // 32bit unsigned integer
    uint32_t emoji_choice_3; // 32bit unsigned integer
    uint8_t minimum_emojis; // 8bit unsigned integer
} PresenterProtocolPresenterOfferEmojiOptions_t;

#define PRESENTER_PRESENTEROFFEREMOJIOPTIONS(_0,_1,_2,_3,_4) \
    PresenterProtocolPresenterOfferEmojiOptions_t _0 = { \
        .id = PresenterOfferEmojiOptions, \
        .emoji_choice_1 = _1, \
        .emoji_choice_2 = _2, \
        .emoji_choice_3 = _3, \
        .minimum_emojis = _4, \
    };

typedef struct {
    uint8_t id; // RemotePickEmojis
    bool emoji_choice_1; // boolean
    bool emoji_choice_2; // boolean
    bool emoji_choice_3; // boolean
} PresenterProtocolRemotePickEmojis_t;

#define PRESENTER_REMOTEPICKEMOJIS(_0,_1,_2,_3) \
    PresenterProtocolRemotePickEmojis_t _0 = { \
        .id = RemotePickEmojis, \
        .emoji_choice_1 = _1, \
        .emoji_choice_2 = _2, \
        .emoji_choice_3 = _3, \
    };

typedef struct {
    uint8_t id; // PresenterAssignEmojis
    uint32_t emoji_choice_1; // 32bit unsigned integer
    uint32_t emoji_choice_2; // 32bit unsigned integer
    uint32_t emoji_choice_3; // 32bit unsigned integer
} PresenterProtocolPresenterAssignEmojis_t;

#define PRESENTER_PRESENTERASSIGNEMOJIS(_0,_1,_2,_3) \
    PresenterProtocolPresenterAssignEmojis_t _0 = { \
        .id = PresenterAssignEmojis, \
        .emoji_choice_1 = _1, \
        .emoji_choice_2 = _2, \
        .emoji_choice_3 = _3, \
    };

typedef struct {
    uint8_t id; // BridgeButtonPressed
    uint8_t button_id; // 8bit unsigned integer between 0 and 4
} PresenterProtocolBridgeButtonPressed_t;

#define PRESENTER_BRIDGEBUTTONPRESSED(_0,_1) \
    PresenterProtocolBridgeButtonPressed_t _0 = { \
        .id = BridgeButtonPressed, \
        .button_id = _1, \
    };

typedef struct {
    uint8_t id; // RemoteHeartBeat
    uint8_t battery_level; // 8bit unsigned integer between 0 and 100
    char state_name[20]; // 20 character string that is a little state name
} PresenterProtocolRemoteHeartBeat_t;

#define PRESENTER_REMOTEHEARTBEAT(_0,_1,_2) \
    PresenterProtocolRemoteHeartBeat_t _0 = { \
        .id = RemoteHeartBeat, \
        .battery_level = _1, \
    }; \
   memcpy(_0.state_name, _2, sizeof(_0.state_name));

typedef struct {
    uint8_t id; // RemoteRequestState
    uint32_t state_hash1; // 32bit unsigned integer
    uint32_t state_hash2; // 32bit unsigned integer
    uint32_t state_hash3; // 32bit unsigned integer
    uint32_t state_hash4; // 32bit unsigned integer
} PresenterProtocolRemoteRequestState_t;

#define PRESENTER_REMOTEREQUESTSTATE(_0,_1,_2,_3,_4) \
    PresenterProtocolRemoteRequestState_t _0 = { \
        .id = RemoteRequestState, \
        .state_hash1 = _1, \
        .state_hash2 = _2, \
        .state_hash3 = _3, \
        .state_hash4 = _4, \
    };

typedef struct {
    uint8_t id; // PresenterSetState
    char state_name[20]; // "dark"
} PresenterProtocolPresenterSetStateDarkState_t;

#define PRESENTER_PRESENTERSETSTATEDARKSTATE(_0) \
    PresenterProtocolPresenterSetStateDarkState_t _0 = { \
        .id = PresenterSetState, \
        .state_name = "dark", \
    };

typedef struct {
    uint8_t id; // PresenterSetState
    char state_name[20]; // "mc"
    uint8_t num_choices; // 8bit unsigned integer
    char question_text[64]; // 64 character (including null terminator) string
} PresenterProtocolPresenterSetStateMultipleChoiceState_t;

#define PRESENTER_PRESENTERSETSTATEMULTIPLECHOICESTATE(_0,_1,_2) \
    PresenterProtocolPresenterSetStateMultipleChoiceState_t _0 = { \
        .id = PresenterSetState, \
        .state_name = "mc", \
        .num_choices = _1, \
    }; \
   memcpy(_0.question_text, _2, sizeof(_0.question_text));

typedef struct {
    uint8_t id; // PresenterSetState
    char state_name[20]; // 20 character string that is a little state name
} PresenterProtocolPresenterSetState_t;

#define PRESENTER_PRESENTERSETSTATE(_0,_1) \
    PresenterProtocolPresenterSetState_t _0 = { \
        .id = PresenterSetState, \
    }; \
   memcpy(_0.state_name, _1, sizeof(_0.state_name));

typedef struct {
    uint8_t id; // PresenterRemotePacket
    uint8_t from_mac[6]; // A 6 byte mac address that is encoded in and out of a string for ease
} PresenterProtocolPresenterRemotePacket_t;

#define PRESENTER_PRESENTERREMOTEPACKET(_0,_1) \
    PresenterProtocolPresenterRemotePacket_t _0 = { \
        .id = PresenterRemotePacket, \
    }; \
   memcpy(_0.from_mac, _1, sizeof(_0.from_mac));

#endif
#pragma pack(pop)
