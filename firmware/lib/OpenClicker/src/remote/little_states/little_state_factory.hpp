#include <remote/little_state.hpp>
#include "dark.hpp"
#include "multiple_choice.hpp"
#include <common/print.hpp>
#include <functional>
#include <algorithm>
#include <vector>


#pragma once

class LittleStateFactory {
private:
    std::vector<RemoteLittleState*> allLittleStates= {};
public:
    LittleStateFactory(Print* printer, RemoteGraphicsAdapter* graphicsAdapter) {
        // Create all little states
        allLittleStates.push_back(new DarkLittleState(printer, graphicsAdapter));
        allLittleStates.push_back(new MultipleChoiceLittleState(printer, graphicsAdapter));
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

    RemoteLittleState* GetMatchingLittleState(char* name, uint32_t name_len) {
        RemoteLittleState* result = nullptr;
        std::for_each(allLittleStates.begin(), allLittleStates.end(), [name, name_len, &result](RemoteLittleState* x) {
            if (x->DoesMatchStateName(name, name_len)) {
                result = x;
                return true;
            }
            return false;
        });
        return result;
    }
};