#include <remote/remote.hpp>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include "s3_pin_config.h"

#pragma once

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
        this->tft.init();  // Invoke library, pins defined in User_Setup.h
        tft.setRotation(1);
        tft.fillScreen(TFT_BLACK);
    }
    bool TurnOff() override {
        this->screenOn = false;
        Serial.println("[gfx] TFT power off");
        digitalWrite(PIN_POWER_ON, LOW);
        digitalWrite(PIN_LCD_BL, LOW);
        return true;
    }
    bool FillScreen(uint32_t color) override {
        if (!screenOn) TurnOn();
        Serial.printf("[gfx] Filling screen with color %x\n", color);
        tft.fillScreen(color);
        return true;
    }
};