#include <Arduino.h>
#include <ESPNowW.h>

#ifndef COMMON_H
#define COMMON_H

typedef void (*PreSetupRoutine)(void);
typedef void (*SetupRoutine)(void);
typedef void (*LoopRoutine)(void);

typedef void (*MeshOnReceive)(const uint8_t *mac_addr, const uint8_t *data, int data_len);
typedef void (*MeshOnSend)(const uint8_t *mac_addr, esp_now_send_status_t status);

typedef struct {
    PreSetupRoutine preSetup;
    SetupRoutine setup;
    LoopRoutine loop;
    MeshOnReceive onReceive;
    MeshOnSend onSend;
} platform_routines_t;

#define BRIDGE_MAC_VALUES {0x24, 0x6F, 0x28, 0xA5, 0xCD, 0x94}

#endif