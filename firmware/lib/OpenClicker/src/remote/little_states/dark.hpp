#include <remote/little_state.hpp>
#include <protocol/presenter_protocol.h>

#pragma once

class DarkLittleState : public RemoteLittleState {

public:
    DarkLittleState(Print* printer): RemoteLittleState(printer) {}

    bool DoesMatchStateName(char* name, uint32_t name_len) {
        return strncmp(LITTE_STATE_DARKSTATE_NAME, name, name_len) == 0;
    }

    void GenerateLittleStateBloomHashes(uint32_t* hash1, uint32_t* hash2, uint32_t* hash3, uint32_t* hash4){
        char name[32] = {0};
        bzero(name, sizeof(name));
        strncpy(name, LITTE_STATE_DARKSTATE_NAME, sizeof(name));
        HashStringToLittleBloomHash(name, sizeof(name), hash1, hash2, hash3, hash4);
    }
};