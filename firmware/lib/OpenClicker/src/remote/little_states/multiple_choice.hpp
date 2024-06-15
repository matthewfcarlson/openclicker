#include <remote/little_state.hpp>
#include <remote/remote_graphics.hpp>
#include <protocol/presenter_protocol.h>

#pragma once

class MultipleChoiceLittleState : public RemoteLittleState {
private:
    uint8_t numChoices = 0;
    int8_t selectedChoice = -1;

public:
    MultipleChoiceLittleState(Print* printer,RemoteGraphicsAdapter *graphics): RemoteLittleState(printer, graphics) {}

    bool DoesMatchStateName(char* name, uint32_t name_len) override {
        return strncmp(LITTE_STATE_MULTIPLECHOICESTATE_NAME, name, name_len) == 0;
    }

    void GetStateName(char* name, uint32_t name_len) override {
        strncpy(name, LITTE_STATE_MULTIPLECHOICESTATE_NAME, name_len);
    }

    void GenerateLittleStateBloomHashes(uint32_t* hash1, uint32_t* hash2, uint32_t* hash3, uint32_t* hash4) override {
        char name[32] = {0};
        // TODO: figure out what we want to do with name
        for (char i = '1'; i < '9'; i++) {
            bzero(name, sizeof(name));
            GetStateName(name, sizeof(name));
            uint32_t strlen = strnlen(name, sizeof(name));
            name[strlen] = i;
            HashStringToLittleBloomHash(name, sizeof(name), hash1, hash2, hash3, hash4);
        }
    }

    void Start() override {
        // TODO: turn off the screen and go into low power mode?
        graphics->FillScreen(graphics->COLOR_GOLD);
        this->printer->println("[mc] Multiple choice state started");
        numChoices = 0;
        selectedChoice = -1;
    }

    void HandlePresenterMessage(const uint8_t* data, uint32_t data_size) override {
        // The first message we should handle is the setState message
        // Parse it and get the question text and message
        this->printer->printf("[mc] Handling message of %d bytes\n", data_size);
        uint8_t msgType = data[0];
        if (msgType == PresenterSetState && sizeof(PresenterProtocolPresenterSetStateMultipleChoiceState_t) == data_size) {
            PresenterProtocolPresenterSetStateMultipleChoiceState_t* state = (PresenterProtocolPresenterSetStateMultipleChoiceState_t*)data;
            this->printer->printf("[mc] Got multiple choice Q %s state with %d choices\n", state->question_text, state->num_choices);
            this->numChoices = state->num_choices;
            this->graphics->DrawText(state->question_text, 10, 10);
        }
        else {
            this->printer->printf("Unknown message type %d/%d. We expected %d/%lu\n", msgType, data_size, PresenterSetState, sizeof(PresenterProtocolPresenterSetStateMultipleChoiceState_t));
        }
    }

    bool ButtonPressed(uint8_t index) override {
        if (selectedChoice != -1) {
            return true;
        }
        if (index < numChoices) {
            this->selectedChoice = index;
            this->printer->printf("[mc] Selected choice %d\n", selectedChoice);
            graphics->FillScreen(graphics->COLOR_GREEN);
            this->graphics->DrawText("Done", 10, 10);
            return false;
        }
        return false;
    }

    void Stop() override {}
};
