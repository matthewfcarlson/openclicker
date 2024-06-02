#include "common/common.h"
#include "protocol/presenter_protocol.h"
#include "protocol/remote_protocol.h"
#include "common/periodic_task.hpp"
#include <functional>

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
    uint8_t bridgeMac[6] = {0};
    char littleState[32] = {0};
 
    void MeshRequestBridge() {
        uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        RemoteMessageBridgeRequest_t msg = { .id = BridgeRequest};
        int status = MeshSend(broadcast_mac, (uint8_t*)&msg, sizeof(msg));
    }
    void MeshPresenterRequestState() {
        // TODO: request the current state from the presenter
        PresenterRemoteRequestState_t msg = { .id = RemoteRequestState};
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

public:

    RemoteDevice(Print* printer, Callback rebootFunc): BaseDevice(printer, rebootFunc) {
        bridgeRequestTask = new PeriodicTask(1000, std::bind(&RemoteDevice::MeshRequestBridge, this));
        presenterRequestTask = new PeriodicTask(1000, std::bind(&RemoteDevice::MeshPresenterRequestState, this));
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
            if (littleState[0] != 0) {
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
            strncpy(this->littleState, state->state_name, sizeof(this->littleState));
        }
        else {
            this->printer->printf("Unknown message %d\n", msgType);
        }

    }

    MeshReceive_t CreateReceiveCallback() override {
        return std::bind(&RemoteDevice::MeshOnReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }
};