#include <remote/remote.hpp>
#include "webgl.h"
#include <stdio.h>
#include <emscripten.h>

#pragma once


#define WIDTH 320
#define HEIGHT 170

class RemoteGraphicsWebgl: public RemoteGraphicsAdapter {
private:
    void TurnOn() {
        this->screenOn = true;
        printf("[gfx] TFT power on\n");
    }
public:
    RemoteGraphicsWebgl() {
        printf("[gfx] TFT init\n");
    }
    bool TurnOff() override {
        printf("[gfx] TFT power off\n");
        clear_screen(0,0,0,1.0);
        return true;
    }
    bool FillScreen(uint32_t color) override {
        if (!screenOn) TurnOn();
        uint16_t scaling = 255 / 32;
        float red = (color >> 10 & 0x1F) / 32.0;
        float green = ((color >> 5) & 0x1F) / 32.0;
        float blue = (color & 0x1F) / 32.0;

        printf("[gfx] Filling screen with color %x %f,%f,%f\n", color, red, green, blue);
        clear_screen(red,green, blue,1.0);
        return true;
    }
};