#include "common/common.h"
#include "protocol/presenter_protocol.h"
#include "protocol/remote_protocol.h"
#include "common/periodic_task.hpp"
#include <functional>
#include <algorithm>
#include <vector>
#include "little_state.hpp"
#include <remote/little_states/dark.hpp>
#include <remote/little_states/multiple_choice.hpp>

#pragma once

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

class RemoteDevice: public BaseDevice {

private:

    PeriodicTask* bridgeRequestTask;
    PeriodicTask* presenterRequestTask;
    RemoteBigStates bigState = RemoteBigInit;
    RemoteLittleState* littleState = nullptr;
    std::vector<RemoteLittleState*> allLittleStates= {};
    uint8_t bridgeMac[6] = {0};
 
    void MeshRequestBridge() {
        uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        RemoteMessageBridgeRequest_t msg = { .id = BridgeRequest};
        int status = MeshSend(broadcast_mac, (uint8_t*)&msg, sizeof(msg));
    }
    void MeshPresenterRequestState() {
        // TODO: request the current state from the presenter
        PresenterRemoteRequestState_t msg = { .id = RemoteRequestState};
        GenerateLittleStateBloomHashes(&msg.state_hash1, &msg.state_hash2, &msg.state_hash3, &msg.state_hash4);
        int status = MeshSend(bridgeMac, (uint8_t*)&msg, sizeof(msg));
    }

    void DisplaySetToConnecting() {
        // TODO: write display logic
    }

    void DisplaySetToLowBattery() {
        // TODO: write to display logic
    }

    void DisplaySetConnected() {
        // TODO: write display logic
    }

    void DisplaySetToError() {
        // TODO: write display logic
    }

    // TODO: create some sort of little state store that we can compile to another file that's consumed by presenter
    // That way we know the hashes will match
    void GenerateLittleStateBloomHashes(uint32_t* hash1, uint32_t* hash2, uint32_t* hash3, uint32_t* hash4) {
        *hash1 = 0;
        *hash2 = 0;
        *hash3 = 0;
        *hash4 = 0;
        std::for_each(allLittleStates.begin(), allLittleStates.end(), [hash1,hash2,hash3,hash4](RemoteLittleState* x){ x->GenerateLittleStateBloomHashes(hash1, hash2, hash3, hash4); });
    }

public:

    RemoteDevice(Print* printer, Callback rebootFunc, const uint8_t* mac = nullptr): BaseDevice(printer, rebootFunc, mac) {
        bridgeRequestTask = new PeriodicTask(1000, std::bind(&RemoteDevice::MeshRequestBridge, this));
        presenterRequestTask = new PeriodicTask(1000, std::bind(&RemoteDevice::MeshPresenterRequestState, this));
        // Create all little states
        allLittleStates.push_back(new DarkLittleState(printer));
        allLittleStates.push_back(new MultipleChoiceLittleState(printer));
    };

    RemoteBigStates getBigState() {
        return bigState;
    }

    void PreSetup() override {
        this->printer->printf("Hello from the remote\n");
    }

    void Loop() override {
        // See how long it's been since we last updated the
        const RemoteBigStates currentBigState = bigState;
        RemoteBigStates nextBigState = bigState;
        if (currentBigState == RemoteBigInit) {
            bzero(bridgeMac, sizeof(bridgeMac));
            // Setup the screen 
            nextBigState = RemoteBigConnecting;
        }
        else if (currentBigState == RemoteBigConnecting) {
            if (bridgeMac[0] != 0 && bridgeMac[1] != 0) {
                nextBigState = RemoteBigConnected;
                // Set the screen to show we are connected
                DisplaySetConnected();
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
            PresenterSetState_t* state = (PresenterSetState_t*)data;
            for (const auto littleState : allLittleStates) {
                if (littleState->DoesMatchStateName(state->state_name, sizeof(state->state_name))) {
                    this->littleState = littleState;
                    break;
                }
            }
        }
        else {
            this->printer->printf("Unknown message %d\n", msgType);
        }

    }

    MeshReceive_t CreateReceiveCallback() override {
        return std::bind(&RemoteDevice::MeshOnReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    void ButtonPressed(uint8_t index) override {
        PresenterRemoteButtonPressed_t msg = {
            .id = RemoteButtonPressed,
            .button_id = index
        };
        MeshSend(bridgeMac, (uint8_t*)&msg, sizeof(msg));
    }
};