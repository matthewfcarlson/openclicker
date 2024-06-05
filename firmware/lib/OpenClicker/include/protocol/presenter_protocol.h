/**
 AUTOGENERATED ON 2024-06-04 22:25:19.976968
 DO NOT MODIFY THIS FILE MANUALLY
 IT WILL BE OVERWRITTEN ON THE NEXT BUILD
 Use the generate.py script in the protocol folder to regenerate this file
**/

#pragma once

#include <stdint.h>
#include <stdlib.h>

#define MIN_PRESENTER_MESSAGE_ID 20
enum PresenterMessageId {
    RemoteButtonPressed = 20,
    BridgeButtonPressed = 21,
    RemoteHeartBeat = 22,
    RemoteRequestState = 23,
    PresenterSetState = 24,
};
typedef struct {
    uint8_t id; // PresenterMessageIds.RemoteButtonPressed
    uint8_t button_id; // 8bit unsigned integer between 0 and 4
} PresenterRemoteButtonPressed_t;

#define PRESENTER_REMOTEBUTTONPRESSED(_0,_1) \
    PresenterRemoteButtonPressed_t _0 = { \
        .id = RemoteButtonPressed, \
        .button_id = _1, \
    }

typedef struct {
    uint8_t id; // PresenterMessageIds.BridgeButtonPressed
    uint8_t button_id; // 8bit unsigned integer between 0 and 4
} PresenterBridgeButtonPressed_t;

#define PRESENTER_BRIDGEBUTTONPRESSED(_0,_1) \
    PresenterBridgeButtonPressed_t _0 = { \
        .id = BridgeButtonPressed, \
        .button_id = _1, \
    }

typedef struct {
    uint8_t id; // PresenterMessageIds.RemoteHeartBeat
    uint8_t battery_level; // 8bit unsigned integer between 0 and 100
} PresenterRemoteHeartBeat_t;

#define PRESENTER_REMOTEHEARTBEAT(_0,_1) \
    PresenterRemoteHeartBeat_t _0 = { \
        .id = RemoteHeartBeat, \
        .battery_level = _1, \
    }

typedef struct {
    uint8_t id; // PresenterMessageIds.RemoteRequestState
    uint32_t state_hash1; // 32bit unsigned integer
    uint32_t state_hash2; // 32bit unsigned integer
    uint32_t state_hash3; // 32bit unsigned integer
    uint32_t state_hash4; // 32bit unsigned integer
} PresenterRemoteRequestState_t;

#define PRESENTER_REMOTEREQUESTSTATE(_0,_1,_2,_3,_4) \
    PresenterRemoteRequestState_t _0 = { \
        .id = RemoteRequestState, \
        .state_hash1 = _1, \
        .state_hash2 = _2, \
        .state_hash3 = _3, \
        .state_hash4 = _4, \
    }

typedef struct {
    uint8_t id; // PresenterMessageIds.PresenterSetState
    char state_name[32]; // 32 character string that is a little state name
} PresenterPresenterSetState_t;

#define PRESENTER_PRESENTERSETSTATE(_0,_1) \
    PresenterPresenterSetState_t _0 = { \
        .id = PresenterSetState, \
        .state_name = _1, \
    }


char* presenter_message_to_json(const uint8_t* message, const uint32_t message_size) {
    if (message[0] == RemoteButtonPressed) {
        PresenterRemoteButtonPressed_t* msg = (PresenterRemoteButtonPressed_t*)message;
        int size_needed = snprintf(NULL, 0, "{'id':'RemoteButtonPressed','button_id':%u}", msg->button_id);
        char* json = (char*)malloc(size_needed+1);
        if (json != NULL) return NULL;
        bzero(json, size_needed+1);
        snprintf(json, size_needed, "{'id':'RemoteButtonPressed','button_id':%u}", msg->button_id);
        return json;
    }
    if (message[0] == BridgeButtonPressed) {
        PresenterBridgeButtonPressed_t* msg = (PresenterBridgeButtonPressed_t*)message;
        int size_needed = snprintf(NULL, 0, "{'id':'BridgeButtonPressed','button_id':%u}", msg->button_id);
        char* json = (char*)malloc(size_needed+1);
        if (json != NULL) return NULL;
        bzero(json, size_needed+1);
        snprintf(json, size_needed, "{'id':'BridgeButtonPressed','button_id':%u}", msg->button_id);
        return json;
    }
    if (message[0] == RemoteHeartBeat) {
        PresenterRemoteHeartBeat_t* msg = (PresenterRemoteHeartBeat_t*)message;
        int size_needed = snprintf(NULL, 0, "{'id':'RemoteHeartBeat','battery_level':%u}", msg->battery_level);
        char* json = (char*)malloc(size_needed+1);
        if (json != NULL) return NULL;
        bzero(json, size_needed+1);
        snprintf(json, size_needed, "{'id':'RemoteHeartBeat','battery_level':%u}", msg->battery_level);
        return json;
    }
    if (message[0] == RemoteRequestState) {
        PresenterRemoteRequestState_t* msg = (PresenterRemoteRequestState_t*)message;
        int size_needed = snprintf(NULL, 0, "{'id':'RemoteRequestState','state_hash1':%u,'state_hash2':%u,'state_hash3':%u,'state_hash4':%u}", msg->state_hash1, msg->state_hash2, msg->state_hash3, msg->state_hash4);
        char* json = (char*)malloc(size_needed+1);
        if (json != NULL) return NULL;
        bzero(json, size_needed+1);
        snprintf(json, size_needed, "{'id':'RemoteRequestState','state_hash1':%u,'state_hash2':%u,'state_hash3':%u,'state_hash4':%u}", msg->state_hash1, msg->state_hash2, msg->state_hash3, msg->state_hash4);
        return json;
    }
    if (message[0] == PresenterSetState) {
        PresenterPresenterSetState_t* msg = (PresenterPresenterSetState_t*)message;
        int size_needed = snprintf(NULL, 0, "{'id':'PresenterSetState','state_name':'%s'}", msg->state_name);
        char* json = (char*)malloc(size_needed+1);
        if (json != NULL) return NULL;
        bzero(json, size_needed+1);
        snprintf(json, size_needed, "{'id':'PresenterSetState','state_name':'%s'}", msg->state_name);
        return json;
    }
    return NULL;
}
