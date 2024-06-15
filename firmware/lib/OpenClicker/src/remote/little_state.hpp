#include "common/common.h"
#include <remote/remote_graphics.hpp>
#include <stdbool.h>

#pragma once

class RemoteLittleState {
private:
   uint32_t djb2(const void *buff, size_t length, uint32_t DJB2_INIT) {
        uint32_t hash = DJB2_INIT;
        const uint8_t *data = (uint8_t *)buff;
        for(size_t i = 0; i < length; i++) {
            hash = ((hash << 5) + hash) + data[i];
        }
        return hash;
    }
    uint32_t sdbm(const void *buff, size_t length) {
        uint32_t hash = 0;
        const uint8_t *data = (uint8_t *)buff;
        for(size_t i = 0; i < length; i++) {
            hash = data[i] + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }

    static inline uint32_t _rotl32(uint32_t x, int32_t bits) {
        return x<<bits | x>>(32-bits);      // C idiom: will be optimized to a single operation
    }

    static inline uint32_t jenkins_one_at_a_time_hash(char *key, size_t len) {
        uint32_t hash, i;
        for(hash = i = 0; i < len; ++i)
        {
            hash += key[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash;
    }

    static uint32_t inline MurmurOAAT32 ( const char * key)
    {
        uint32_t h(3323198485ul);
        for (;*key;++key) {
            h ^= *key;
            h *= 0x5bd1e995;
            h ^= h >> 15;
        }
        return h;
    }

    static uint64_t inline MurmurOAAT64 ( const char * key)
    {
        uint64_t h(525201411107845655ull);
        for (;*key;++key) {
            h ^= *key;
            h *= 0x5bd1e9955bd1e995;
            h ^= h >> 47;
        }
        return h;
    }

    static inline uint32_t Coffin_hash(char const *input, uint32_t input_len) {
        int result = 0x55555555;
        unsigned int index = 0;

        while (*input && (index++ < input_len)) {
            result ^= *input++;
            result = _rotl32(result, 5);
        }
        return result;
    }

    static inline uint32_t HashToBit(uint32_t hash) {
        uint8_t bitNum = hash % 32;
        return 1 << bitNum;
    }
protected:
    Print* printer;
    RemoteGraphicsAdapter* graphics;

    void HashStringToLittleBloomHash(char* name, uint32_t name_len, uint32_t* hash1, uint32_t* hash2, uint32_t* hash3, uint32_t* hash4) {
        // this->printer->printf("Hashing %s into %x %x %x %x\n", name, *hash1, *hash2, *hash3, *hash4);
        // TODO: figure out which hash functions are too slow
        *hash1 = *hash1 | HashToBit(this->jenkins_one_at_a_time_hash(name, name_len));
        *hash2 = *hash2 | HashToBit(this->sdbm(name, name_len));
        *hash3 = *hash3 | HashToBit(this->Coffin_hash(name, name_len));
        *hash4 = *hash4 | HashToBit(this->MurmurOAAT32(name));
        // this->printer->printf("After: %x %x %x %x\n", *hash1, *hash2, *hash3, *hash4);
    }

public:
    RemoteLittleState(Print* printer, RemoteGraphicsAdapter* graphics): printer(printer), graphics(graphics) {}

    virtual bool DoesMatchStateName(char* name, uint32_t name_len) {
        return false;
    }

    virtual void GetStateName(char* name, uint32_t name_len) = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;

    virtual void GenerateLittleStateBloomHashes(uint32_t* hash1, uint32_t* hash2, uint32_t* hash3, uint32_t* hash4) = 0;

    // If we return true, prevent the default event from being triggered
    virtual bool ButtonPressed(uint8_t index) {
        return true;
    }
    virtual bool ButtonReleased(uint8_t index) {
        return true;
    }

    virtual void HandlePresenterMessage(const uint8_t* data, uint32_t data_size) = 0;

    // The loop is intentionally a no-op
    virtual void Loop() {}
};