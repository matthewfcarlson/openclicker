#include <Arduino.h>
#include "ESPNowW.h"
#include "common.h"

#pragma once

void bridgeSetup(void);
void bridgeLoop(void);
void bridgePreSetup(void);

void bridgeOnRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void bridgeOnSend(const uint8_t *mac_addr, esp_now_send_status_t status);

void queueSerialOut(const String str);

const platform_routines_t routines = {
    .setup = &bridgeSetup,
    .loop = &bridgeLoop,
    .onReceive = bridgeOnRecv,
    .onSend = bridgeOnSend,
};