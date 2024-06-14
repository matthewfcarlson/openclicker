#include <stdint.h>

#pragma once

#ifndef PROGMEM
#define PROGMEM
#endif

typedef struct {
    const char* tag;
    const void* data;
    int width;
    int height;
} RemoteGraphic_t;