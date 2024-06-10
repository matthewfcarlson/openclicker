#include <remote/remote.hpp>
#include "webgl.h"
#include <stdio.h>
#include <emscripten.h>

#pragma once


#define WIDTH 320
#define HEIGHT 170

class RemoteGraphicsWebgl: public RemoteGraphicsAdapter {
private:
    bool loaded = false;
    void TurnOn() {
        this->screenOn = true;
        if (!loaded) {
            printf("[gfx] first time texture load\n");
            load_tagged_image(GRAPHIC_ALERT_ICON);
            load_tagged_image(GRAPHIC_CLOSE_ICON);
            load_tagged_image(GRAPHIC_CARD_0);
            load_tagged_image(GRAPHIC_INFO_ICON);
            loaded = true;
        }
        printf("[gfx] web power on\n");
    }
    void load_tagged_image(const RemoteGraphic_t* graphic) {
        load_image_by_tag(graphic->tag, graphic->data, graphic->width, graphic->height);
    }
public:
    RemoteGraphicsWebgl() {
        printf("[gfx] web init\n");
    }
    bool TurnOff() override {
        printf("[gfx] TFT power off\n");
        clear_screen(0,0,0,1.0);
        return true;
    }
    bool FillScreen(uint32_t color) override {
        if (!screenOn) TurnOn();
        uint16_t scaling = 255 / 32;
        float red = (color >> 11 & 0x1F) / 32.0; // red = 5 bits
        float green = ((color >> 5) & 0x3F) / 64.0; // green = 6 bits
        float blue = (color & 0x1F) / 32.0; // blue = 5 bits

        printf("[gfx] Filling screen with color %x %f,%f,%f\n", color, red, green, blue);
        clear_screen(red,green, blue,1.0);
        return true;
    }
    bool DrawImage(const RemoteGraphic_t* graphic, uint32_t x, uint32_t y) override {
        printf("[gfx] Drawing image %s at %d %d. %d,%d\n", graphic->tag, x, y, graphic->width, graphic->height);
        float new_y = (float)(HEIGHT - y - graphic->height);
        fill_image_tag((float)x, new_y, graphic->tag);
        return true;
    }
};