#include <remote/remote.hpp>
#include "webgl.h"
#include <stdio.h>
#include <emscripten.h>

#pragma once


class RemoteGraphicsWebgl: public RemoteGraphicsAdapter {
private:
    bool loaded = false;
    void TurnOn() {
        this->screenOn = true;
        printf("[gfx] web power on\n");
    }
protected:
    void PreloadImage(const RemoteGraphic_t* graphic) override {
        printf("Preloading image %s\n", graphic->tag);
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
    bool FillScreen(uint16_t color) override {
        if (!screenOn) TurnOn();
        uint16_t scaling = 255 / 32;
        float red = (color >> 11 & 0x1F) / 32.0; // red = 5 bits
        float green = ((color >> 5) & 0x3F) / 64.0; // green = 6 bits
        float blue = (color & 0x1F) / 32.0; // blue = 5 bits
        lastBackgroundColor = color;

        printf("[gfx] Filling screen with color %x %f,%f,%f\n", color, red, green, blue);
        clear_screen(red,green, blue,1.0);
        return true;
    }
    bool DrawImage(const RemoteGraphic_t* graphic, uint32_t x, uint32_t y) override {
        if (graphic == nullptr) {
            printf("[gfx] No graphic to draw\n");
            return false;
        }
        printf("[gfx] Drawing image %s at %d %d. %d,%d\n", graphic->tag, x, y, graphic->width, graphic->height);
        // We need to flip it since we draw from the bottom right corner
        float new_y = (float)(TFT_HEIGHT - y - graphic->height);
        fill_image_tag((float)x, new_y, graphic->tag);
        return true;
    }

    bool DrawText(const char* text, uint32_t x, uint32_t y) override {
        int fontSize = 26;
        float new_y = (float)(TFT_HEIGHT - y - fontSize);
        fill_text(x, new_y, 1.0, 1.0, 1.0, 1.0, text, 0, fontSize, 0);
        return true;
    }
};