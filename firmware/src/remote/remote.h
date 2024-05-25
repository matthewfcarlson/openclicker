#include <Arduino.h>
#include "ESPNowW.h"
#include "common.h"

#pragma once

void remoteSetup(void);
void remoteLoop(void);

void remoteOnRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void remoteOnSend(const uint8_t *mac_addr, esp_now_send_status_t status);


const platform_routines_t routines = {
    .setup = remoteSetup,
    .loop = remoteLoop,
    .onSend = remoteOnSend,
};