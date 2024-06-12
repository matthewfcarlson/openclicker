#include <remote/remote.hpp>
#define CONFIG_TFT_RGB_ORDER TFT_BGR
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include "s3_pin_config.h"

#pragma once

/* The product now has two screens, and the initialization code needs a small change in the new version. The LCD_MODULE_CMD_1 is used to define the
 * switch macro. */
#define LCD_MODULE_CMD_1

#if defined(LCD_MODULE_CMD_1)
typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};
#endif

class RemoteGraphicsTFT: public RemoteGraphicsAdapter {
private:
    TFT_eSPI tft = TFT_eSPI();
    void TurnOn() {
        this->screenOn = true;
        Serial.println("[gfx] TFT power on");
        digitalWrite(PIN_POWER_ON, HIGH);
        digitalWrite(PIN_LCD_BL, HIGH);
    }
public:
    RemoteGraphicsTFT() {
        pinMode(PIN_POWER_ON, OUTPUT);
        pinMode(PIN_LCD_BL, OUTPUT);
        digitalWrite(PIN_LCD_BL, LOW);
        Serial.println("[gfx] TFT init");
        tft.init();  // Invoke library, pins defined in User_Setup.h
        tft.setSwapBytes(1);
#if defined(LCD_MODULE_CMD_1)
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        tft.writecommand(lcd_st7789v[i].cmd);
        for (int j = 0; j < (lcd_st7789v[i].len & 0x7f); j++) {
            tft.writedata(lcd_st7789v[i].data[j]);
        }

        if (lcd_st7789v[i].len & 0x80) {
            delay(120);
        }
    }
#endif
        tft.setRotation(3);
        tft.fillScreen(TFT_BLACK);
    }
    bool TurnOff() override {
        this->screenOn = false;
        Serial.println("[gfx] TFT power off");
        digitalWrite(PIN_POWER_ON, LOW);
        digitalWrite(PIN_LCD_BL, LOW);
        return true;
    }
    bool FillScreen(uint16_t color) override {
        if (!screenOn) TurnOn();
        Serial.printf("[gfx] Filling screen with color %x\n", color);
        lastBackgroundColor = color;
        tft.fillScreen(color);
        return true;
    }

    bool DrawImage(const RemoteGraphic_t* graphic, uint32_t x, uint32_t y) override {
        printf("[gfx] Drawing image %s at %d %d\n", graphic->tag, x, y);
        tft.pushImage(x,y, graphic->width, graphic->height, (uint16_t*)graphic->data, 0x0120);
        return true;
    }
};