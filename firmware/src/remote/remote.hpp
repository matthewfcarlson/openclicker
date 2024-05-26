#include "common.h"
#include "protocol/presenter_protocol.h"
#include "protocol/remote_protocol.h"
#include "periodic_task.hpp"
#include <functional>

enum RemoteBigStates {
    RemoteBigInit, // Startup state
    RemoteBigConnecting, // Quickly moves to Connecting
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
    RemoteBigStates bigState = RemoteBigInit;
    bool foundBridge = false;

    void MeshRequestBridge() {
        uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        RemoteMessageBridgeRequest_t msg = { .id = BridgeRequest};
        esp_err_t status = this->meshSendFunc(broadcast_mac, (uint8_t*)&msg, sizeof(msg));
        this->printer->printf("Sending bridge request = %x\n",status);
    }
    void MeshRequestState() {
        // TODO: request the current state from the presenter
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
    };

    RemoteBigStates getBigState() {
        return bigState;
    }

    void PreSetup() override {
        this->printer->printf("Hello from the remote\n");
    }

    void Loop() {
        // See how long it's been since we last updated the
        const RemoteBigStates currentBigState = bigState;
        RemoteBigStates nextBigState = bigState;
        if (currentBigState == RemoteBigInit) {
            foundBridge = false;
            // Setup the screen 
            nextBigState = RemoteBigConnecting;
        }
        else if (currentBigState == RemoteBigConnecting) {
            if (foundBridge) {
                nextBigState = RemoteBigParticipating;
                // Set the screen to show we are connected
                DisplaySetConnected();
                // Request what state we should be in from the presenter
                MeshRequestState();
            }
            else {
                this->bridgeRequestTask->Check();
            }
        }
        else if (currentBigState == RemoteBigRebooting) {
            // Reboot the whole system
            rebootFunc();
        }

        // Check if we have a low battery, if so
        // nextBigState = RemoteBigLowBattery;
        // DisplaySetToLowBattery();


        if (currentBigState != nextBigState) {
            this->printer->printf("State %x->%x\n", currentBigState, nextBigState);
            this->bigState = nextBigState;
        }
    }

    void MeshOnSend(const uint8_t *mac_addr, uint8_t status) {

    }

    void MeshOnReceive(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
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
        if (msgType == BridgeResponse) {
            // Add this node as a peer and mark that we found a bridge
            this->meshAddPeerFunc(mac_addr, 0);
            this->foundBridge = true;
        }
    }
};
