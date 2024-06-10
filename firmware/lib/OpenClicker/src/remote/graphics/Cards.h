#include "common.h"

// We need this header file to use FLASH as storage with PROGMEM directive:
#pragma once

int framesNumber=53;
int aniWidth=60;
int aniHeigth=84;

const RemoteGraphic_t Graphics_card0 = {
    .tag = "card0",
    .data = (const uint8_t*)card[10],
    .width = aniWidth,
    .height = aniHeigth,
};