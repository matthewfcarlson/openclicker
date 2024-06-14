#include "common/common.h"
// graphics
#include <images.h>
#pragma once

#define TFT_WIDTH 320
#define TFT_HEIGHT 170

class RemoteGraphicsAdapter {
protected:
    bool screenOn = false;
    uint16_t lastBackgroundColor = 0;
public:
    static const uint16_t COLOR_BLACK       = 0x0000;      /*   0,   0,   0 */
    static const uint16_t COLOR_NAVY        = 0x000F;      /*   0,   0, 128 */
    static const uint16_t COLOR_DARKGREEN   = 0x03E0;      /*   0, 128,   0 */
    static const uint16_t COLOR_DARKCYAN    = 0x03EF;      /*   0, 128, 128 */
    static const uint16_t COLOR_MAROON      = 0x7800;      /* 128,   0,   0 */
    static const uint16_t COLOR_PURPLE      = 0x780F;      /* 128,   0, 128 */
    static const uint16_t COLOR_OLIVE       = 0x7BE0;      /* 128, 128,   0 */
    static const uint16_t COLOR_LIGHTGREY   = 0xD69A;      /* 211, 211, 211 */
    static const uint16_t COLOR_DARKGREY    = 0x7BEF;      /* 128, 128, 128 */
    static const uint16_t COLOR_BLUE        = 0x001F;      /*   0,   0, 255 */
    static const uint16_t COLOR_GREEN       = 0x07E0;      /*   0, 255,   0 */
    static const uint16_t COLOR_CYAN        = 0x07FF;      /*   0, 255, 255 */
    static const uint16_t COLOR_RED         = 0xF800;      /* 255,   0,   0 */
    static const uint16_t COLOR_MAGENTA     = 0xF81F;      /* 255,   0, 255 */
    static const uint16_t COLOR_YELLOW      = 0xFFE0;      /* 255, 255,   0 */
    static const uint16_t COLOR_WHITE       = 0xFFFF;      /* 255, 255, 255 */
    static const uint16_t COLOR_ORANGE      = 0xFDA0;      /* 255, 180,   0 */
    static const uint16_t COLOR_GREENYELLOW = 0xB7E0;      /* 180, 255,   0 */
    static const uint16_t COLOR_PINK        = 0xFE19;      /* 255, 192, 203 */ //Lighter pink, was 0xFC9F
    static const uint16_t COLOR_BROWN       = 0x9A60;      /* 150,  75,   0 */
    static const uint16_t COLOR_GOLD        = 0xFEA0;      /* 255, 215,   0 */
    static const uint16_t COLOR_SILVER      = 0xC618;      /* 192, 192, 192 */
    static const uint16_t COLOR_SKYBLUE     = 0x867D;      /* 135, 206, 235 */
    static const uint16_t COLOR_VIOLET      = 0x915C;      /* 180,  46, 226 */
    virtual bool FillScreen(uint16_t color) = 0;
    virtual bool TurnOff() = 0;
    void PreLoad() {
        PRELOAD_ALL_IMAGES;
    }
    virtual void PreloadImage(const RemoteGraphic_t* graphic) {}
    virtual bool DrawImage(const RemoteGraphic_t* graphic, uint32_t x, uint32_t y) = 0;
    bool DrawImageCenteredXY(const RemoteGraphic_t* graphic, uint32_t x, uint32_t y) {
        if (graphic == nullptr) return false;
        return DrawImage(graphic, x - graphic->width / 2, y - graphic->height / 2);
    }
    bool DrawImageCenteredX(const RemoteGraphic_t* graphic, uint32_t x) {
        return DrawImageCenteredXY(graphic, x, TFT_HEIGHT / 2);
    }
    bool DrawImageCenteredY(const RemoteGraphic_t* graphic, uint32_t y) {
        return DrawImageCenteredXY(graphic, TFT_WIDTH / 2, y);
    }
    bool DrawImageCentered(const RemoteGraphic_t* graphic) {
        return DrawImageCenteredXY(graphic, TFT_WIDTH / 2, TFT_HEIGHT / 2);
    }
    virtual bool DrawText(const char* text, uint32_t x, uint32_t y) = 0;

};
