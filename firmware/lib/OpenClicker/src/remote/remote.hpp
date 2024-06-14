#include "common/common.h"
#include "protocol/presenter_protocol.h"
#include "protocol/remote_protocol.h"
#include "common/periodic_task.hpp"
#include <functional>
#include <algorithm>
#include <vector>
#include <remote/little_states/little_state_factory.hpp>
#include <remote/remote_graphics.hpp>

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

    bool DrawText(const char* text, uint32_t x, uint32_t y) override {
        printer->printf("[gfx] Drawing text %s at %d %d\n", text, x, y);
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
        graphics->FillScreen(graphics->COLOR_BLACK);
        graphics->DrawImageCentered(&Graphics_icons_connecting);
        graphics->DrawText("connecting...", TFT_WIDTH / 2, 20);
    }

    void DisplaySetToLowBattery() {
        graphics->FillScreen(graphics->COLOR_YELLOW);
    }

    void DisplaySetToConnected() {
        graphics->FillScreen(graphics->COLOR_BLACK);
        graphics->DrawImageCentered(&Graphics_icons_connected);
        graphics->DrawText("connected", TFT_WIDTH / 2, 20);
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
        if (graphics == nullptr) this->graphics = new RemoteGraphicsTextAdapter(printer);
        else this->graphics = graphics;
        this->graphics->PreLoad();
        littleStateFactory = new LittleStateFactory(printer, this->graphics);
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

        if (littleState != nullptr) {
            littleState->Loop();
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
        else if (msgType == PresenterSetState && data_len >= sizeof(PresenterProtocolPresenterSetState_t)) {
            // TODO: create a macro that wraps this check
            // assert(sizeof(PresenterSetState_t) == data_len);
            PresenterProtocolPresenterSetState_t* state = (PresenterProtocolPresenterSetState_t*)data;
            RemoteLittleState* newLittleState = littleStateFactory->GetMatchingLittleState(state->state_name, sizeof(state->state_name));
            if (newLittleState== nullptr) {
                this->printer->printf("Failed to get little state for %s\n", state->state_name);
            }
            else {
                this->printer->printf("Got state for %s\n", state->state_name);
                if (this->littleState != nullptr) this->littleState->Stop();
                newLittleState->Start();
                this->littleState = newLittleState;
            }
        }
        else {
            this->printer->printf("Unknown message %d\n", msgType);
        }
        // We give the little state the chance to do what it wants with the message
        if (this->littleState != nullptr) {
            this->littleState->HandlePresenterMessage(data, data_len);
        }

    }

    MeshReceive_t CreateReceiveCallback() override {
        return std::bind(&RemoteDevice::MeshOnReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    void ButtonPressed(uint8_t index) override {
        if (littleState == nullptr) return;
        // If the little state handled the button, don't do anything
        if (littleState->ButtonPressed(index)) return;
        PRESENTER_REMOTEBUTTONPRESSED(msg, index);
        MeshSend(bridgeMac, (uint8_t*)&msg, sizeof(msg));
    }
    void ButtonReleased(uint8_t index) override {
        if (littleState == nullptr) return;
        // If the little state handled the button, don't do anything
        if (littleState->ButtonPressed(index)) return;
        PRESENTER_REMOTEBUTTONPRESSED(msg, index);
        MeshSend(bridgeMac, (uint8_t*)&msg, sizeof(msg));
    }
};