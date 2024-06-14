#include "common/common.h"
#include "protocol/presenter_protocol.h"
#include "protocol/remote_protocol.h"
#include "common/periodic_task.hpp"
#include <functional>
#include <algorithm>
#include <vector>
#include <remote/little_states/little_state_factory.hpp>
// graphics
#include <images.h>

#pragma once

#define TFT_WIDTH 320
#define TFT_HEIGHT 170

enum RemoteBigStates {
    RemoteBigInit, // Startup state
    RemoteBigConnecting, // Quickly moves to Connecting
    RemoteBigConnected, // We are connected to a bridge, but we aren't participating
    RemoteBigParticipating,
    RemoteBigLowBattery,
    RemoteBigOtaNeeded,
    RemoteBigOta,
    RemoteBigRebooting,
    RemoteBigError,
};

class RemoteGraphicsAdapter {
protected:
    bool screenOn = false;
    uint16_t lastBackgroundColor = 0;
public:
    INCLUDE_ALL_IMAGES;
    const RemoteGraphic_t* EMOJI_MOVIE_CAMERA = &Graphics_movie_camera_1f3a5;
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
        PreloadImage(GRAPHIC_ALERT_ICON);
        PreloadImage(GRAPHIC_INFO_ICON);
        PreloadImage(GRAPHIC_CLOSE_ICON);
        PreloadImage(GRAPHIC_CARD_0);
        PreloadImage(EMOJI_MOVIE_CAMERA);
    }
    virtual void PreloadImage(const RemoteGraphic_t* graphic) {}
    virtual bool DrawImage(const RemoteGraphic_t* graphic, uint32_t x, uint32_t y) = 0;

};

class RemoteGraphicsTextAdapter: public RemoteGraphicsAdapter {
private:
    Print* printer;
public:
    RemoteGraphicsTextAdapter(Print* printer): printer(printer), RemoteGraphicsAdapter() {}
    bool FillScreen(uint16_t color) override {
        screenOn = true;
        lastBackgroundColor = color;
        printer->printf("[gfx] Filling screen with color %x\n", color);
        return true;
    }
    bool TurnOff() override {
        printer->printf("[gfx] Turning off screen\n");
        screenOn = false;
        return true;
    }

    bool DrawImage(const RemoteGraphic_t* graphic, uint32_t x, uint32_t y) override {
        printer->printf("[gfx] Drawing image %s at %d %d\n", graphic->tag, x, y);
        return true;
    }
};

class RemoteDevice: public BaseDevice {

private:

    PeriodicTask* bridgeRequestTask;
    PeriodicTask* presenterRequestTask;
    RemoteBigStates bigState = RemoteBigInit;
    RemoteLittleState* littleState = nullptr;
    LittleStateFactory* littleStateFactory;
    RemoteGraphicsAdapter* graphics;

    uint8_t bridgeMac[6] = {0};

    void MeshRequestBridge() {
        this->printer->printf("Requesting bridge. Please come in\n");
        RemoteMessageBridgeRequest_t msg = { .id = BridgeRequest};
        int status = MeshSend(BROADCAST_MAC, (uint8_t*)&msg, sizeof(msg));
        if (status != MESH_OK) {
            this->printer->printf("Failed to send bridge request: %x\n", status);
        }
    }
    void MeshPresenterRequestState() {
        // Request the current state from the presenter
        PRESENTER_REMOTEREQUESTSTATE(msg, 0,0,0,0);
        littleStateFactory->GenerateLittleStateBloomHashes(&msg.state_hash1, &msg.state_hash2, &msg.state_hash3, &msg.state_hash4);
        int status = MeshSend(bridgeMac, (uint8_t*)&msg, sizeof(msg));
        if (status != MESH_OK) {
            this->printer->printf("Failed to request state: %x\n", status);
        }
    }

    void DisplaySetToConnecting() {
        // if (graphyi)
        graphics->FillScreen(graphics->COLOR_BLUE);
        // TODO: write display logic
    }

    void DisplaySetToLowBattery() {
        // TODO: write to display logic
        graphics->FillScreen(graphics->COLOR_YELLOW);
    }

    void DisplaySetToConnected() {
        graphics->FillScreen(graphics->COLOR_BLACK);
        // TODO: write display logic
        graphics->DrawImage(graphics->GRAPHIC_ALERT_ICON, 10, 10);
        graphics->DrawImage(graphics->GRAPHIC_CLOSE_ICON, 50, 10);
        graphics->DrawImage(graphics->GRAPHIC_INFO_ICON, 100, 10);
        graphics->DrawImage(graphics->EMOJI_MOVIE_CAMERA, 120, (TFT_HEIGHT/2) - 30);
    }

    void DisplaySetToError() {
        graphics->FillScreen(graphics->COLOR_RED);
        // TODO: write display logic
    }

public:

    RemoteDevice(Print* printer, Callback rebootFunc, const uint8_t* mac = nullptr, RemoteGraphicsAdapter* graphics = nullptr): BaseDevice(printer, rebootFunc, mac)
    {
        bridgeRequestTask = new PeriodicTask(1000, std::bind(&RemoteDevice::MeshRequestBridge, this));
        presenterRequestTask = new PeriodicTask(1000, std::bind(&RemoteDevice::MeshPresenterRequestState, this));
        littleStateFactory = new LittleStateFactory(printer);
        if (graphics == nullptr) this->graphics = new RemoteGraphicsTextAdapter(printer);
        else this->graphics = graphics;
        this->graphics->PreLoad();
    };

    RemoteBigStates getBigState() {
        return bigState;
    }

    void PreSetup() override {
        this->printer->printf("Hello from the remote\n");
        graphics->FillScreen(graphics->COLOR_BLACK);
    }

    void Loop() override {
        // See how long it's been since we last updated the
        const RemoteBigStates currentBigState = bigState;
        RemoteBigStates nextBigState = bigState;
        if (currentBigState == RemoteBigInit) {
            bzero(bridgeMac, sizeof(bridgeMac));
            // Setup the screen
            nextBigState = RemoteBigConnecting;
            DisplaySetToConnecting();
        }
        else if (currentBigState == RemoteBigConnecting) {
            if (bridgeMac[0] != 0 && bridgeMac[1] != 0) {
                nextBigState = RemoteBigConnected;
                // Set the screen to show we are connected
                DisplaySetToConnected();
            }
            else {
                this->bridgeRequestTask->Check();
            }
        }
        else if (currentBigState == RemoteBigConnected) {
            this->presenterRequestTask->Check();
            if (littleState != nullptr) {
                nextBigState = RemoteBigParticipating;
            }
        }
        else if (currentBigState == RemoteBigRebooting) {
            // Reboot the whole system
            this->Reboot();
        }

        // Check if we have a low battery, if so
        // nextBigState = RemoteBigLowBattery;
        // DisplaySetToLowBattery();


        if (currentBigState != nextBigState) {
            this->printer->printf("State %x->%x\n", currentBigState, nextBigState);
            this->bigState = nextBigState;
        }
    }

    void MeshOnSend(const uint8_t* mac_addr, uint8_t status) override  {

    }

    void MeshOnReceive(const uint8_t* mac_addr, const uint8_t *data, uint32_t data_len) override  {
        char macStr[18] = "";
        if (data_len == 0) {
            SprintMacAddress(mac_addr, macStr, sizeof(macStr));
            this->printer->printf("Received empty message from %s\n", macStr);
            return;
        }
        uint8_t msgType = data[0];
        if (msgType == BridgeRequest) {
            // Don't do anything
        }
        // TODO: check that this message comes from the bridge?
        else if (msgType == BridgeResponse) {
            // Add this node as a peer and mark that we found a bridge
            MeshAddPeer(mac_addr, 0);
            memcpy(bridgeMac, mac_addr, sizeof(bridgeMac));
        }
        else if (msgType == PresenterSetState) {
            // We had our state set, we should be presenting if we aren't already
            // TODO: create a macro that wraps this check
            // assert(sizeof(PresenterSetState_t) == data_len);
            PresenterProtocolPresenterSetState_t* state = (PresenterProtocolPresenterSetState_t*)data;
            this->littleState = littleStateFactory->GetMatchingLittleState(state->state_name, sizeof(state->state_name));
            if (this->littleState == nullptr) this->printer->printf("Failed to get little state for %s\n", state->state_name);
            else this->printer->printf("Got state for %s\n", state->state_name);
        }
        else {
            this->printer->printf("Unknown message %d\n", msgType);
        }

    }

    MeshReceive_t CreateReceiveCallback() override {
        return std::bind(&RemoteDevice::MeshOnReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    void ButtonPressed(uint8_t index) override {
        this->printer->printf("Button %d pressed\n", index);
        PRESENTER_REMOTEBUTTONPRESSED(msg, index);
        MeshSend(bridgeMac, (uint8_t*)&msg, sizeof(msg));
        graphics->TurnOff();
    }
};