#include <remote/little_state.hpp>
#include <remote/remote_graphics.hpp>
#include <protocol/presenter_protocol.h>

#pragma once

class DarkLittleState : public RemoteLittleState {

public:
    DarkLittleState(Print* printer, RemoteGraphicsAdapter *graphics): RemoteLittleState(printer, graphics) {}

    bool DoesMatchStateName(char* name, uint32_t name_len) override {
        return strncmp(LITTE_STATE_DARKSTATE_NAME, name, name_len) == 0;
    }

    void GetStateName(char* name, uint32_t name_len) override {
        strncpy(name, LITTE_STATE_DARKSTATE_NAME, name_len);
    }

    void GenerateLittleStateBloomHashes(uint32_t* hash1, uint32_t* hash2, uint32_t* hash3, uint32_t* hash4) override {
        char name[32] = {0};
        bzero(name, sizeof(name));
        strncpy(name, LITTE_STATE_DARKSTATE_NAME, sizeof(name));
        HashStringToLittleBloomHash(name, sizeof(name), hash1, hash2, hash3, hash4);
    }

    void Start() override {
        // TODO: turn off the screen and go into low power mode?
        graphics->TurnOff();
    }

    // Dark mode doesn't respond to any messages, just is dormant
    void HandlePresenterMessage(const uint8_t* data, uint32_t data_size) override {}

    // Dark mode doesn't have anything to clean up
    // perhaps go back into a higher power state?
    void Stop() override {}

};