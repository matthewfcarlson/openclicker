#include <common/common.h>
#include <functional>
#include <map>
#include <inttypes.h>

#pragma once

class FakeMesh {
private:
    typedef struct {
        MeshReceive_t receiver;
        float x;
        float y;
    } MeshEndpoint_t;
    std::map<uint64_t, MeshEndpoint_t> receiverMap;
    uint16_t rangeSquared;

    bool inRange(MeshEndpoint_t a, MeshEndpoint_t b) {
        uint16_t distanceSquared = (uint16_t)(pow((a.x - b.x),2) + pow((a.y - b.y),2));
        return distanceSquared <= this->rangeSquared;
    }
public:
    FakeMesh(uint16_t rangeSquared = 10) : rangeSquared(rangeSquared) {}
    static const uint64_t broadcastMacId = 0xffffffffffff;
    static uint64_t mac_to_uint64_t(const uint8_t* from_mac) {
        return ((uint64_t)(from_mac[5]) << 40)
            | ((uint64_t)(from_mac[4]) << 32)
            | ((uint64_t)(from_mac[3]) << 24)
            | ((uint64_t)(from_mac[2]) << 16)
            | ((uint64_t)(from_mac[1]) << 8)
            | ((uint64_t)(from_mac[0]));
    }

    int SendMessage(const uint8_t* from_mac, const uint8_t* to_mac, const uint8_t * msg, uint32_t msg_size) {
        uint64_t fromMacId = mac_to_uint64_t(from_mac);
        uint64_t toMacId = mac_to_uint64_t(to_mac);
        printf("[FakeMesh] Sending message of %u bytes - first byte is %d. %" PRIx64 "-> %" PRIx64 "\n", msg_size, msg[0], fromMacId, toMacId);
        if (receiverMap.count(fromMacId) == 0) {
            return MESH_ERR_INVALID_STATE;
        }
        MeshEndpoint_t sender = receiverMap[fromMacId];
        // Check if it's a broadcast
        if (toMacId == broadcastMacId) {
            for (auto const& x : receiverMap) {
                if (x.first == fromMacId) continue;
                if (this->inRange(sender, x.second)) {
                    printf("[FakeMesh] Broadcasting to 0x%" PRIx64 "\n", x.first);
                    x.second.receiver(from_mac, msg, msg_size);
                }
                else {
                    printf("[FakeMesh] Out of range 0x%" PRIx64 "\n", x.first);
                    x.second.receiver(from_mac, msg, msg_size);
                }
            }
            return MESH_OK;
        }
        else if (receiverMap.count(toMacId) == 1) {
            if (!this->inRange(sender, receiverMap[toMacId])) {
                printf("[FakeMesh] Out of range\n");
                return MESH_ERR_ESPNOW_NOT_FOUND;
            }
            receiverMap[toMacId].receiver(from_mac, msg, msg_size);
            return MESH_OK;
        }
        else {
            // We did not find this participant
            printf("[FakeMesh] This participant did not register");
            return MESH_ERR_INVALID_STATE;
        }
        return MESH_OK;
    }

    void AddReceiver(const uint8_t* to_mac, MeshReceive_t callback, float x = 0, float y = 0) {
        uint64_t toMacId = mac_to_uint64_t(to_mac);
        if (receiverMap.count(toMacId) == 1) {
            printf("[FakeMesh] This participant registered twice");
        }
        printf("[FakeMesh] Adding receiver 0x%" PRIx64 "\n", toMacId);
        MeshEndpoint_t endpoint = {
            .receiver = callback,
            .x  = x,
            .y = y
        };
        receiverMap[toMacId] = endpoint;
    }
};

class FakeMeshCommunicator {
private:
    std::map<uint64_t, bool> peers;
    FakeMesh* mesh;
    float x, y;
    uint8_t* mac[6];
    mesh_err_t send(const uint8_t* to_mac, const uint8_t* msg, uint32_t msg_size) {
        uint64_t toMacId = FakeMesh::mac_to_uint64_t(to_mac);
        // Check if this isn't a broadcast
        if (toMacId != FakeMesh::broadcastMacId) {
            if (peers.count(toMacId) == 0) {
                // We don't have this person in our peer list
                return MESH_ERR_ESPNOW_NOT_FOUND;
            }
            // Otherwise, go ahead
        }
        return mesh->SendMessage((const uint8_t*)this->mac, to_mac, msg, msg_size);
    }
    mesh_err_t add_peer(const uint8_t* peer_mac, int channel) {
        uint64_t macId = FakeMesh::mac_to_uint64_t(peer_mac);
        peers[macId] = true;
        return MESH_OK;
    }

    MeshSend_t createSendCallback() {
        return std::bind(&FakeMeshCommunicator::send, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }
    MeshAddPeer_t createAddPeerCallback() {
        return std::bind(&FakeMeshCommunicator::add_peer, this, std::placeholders::_1, std::placeholders::_2);
    }

    void registerReceiveCallback(BaseDevice* device) {
        MeshReceive_t receiveCallback = device->CreateReceiveCallback();
        mesh->AddReceiver((const uint8_t*)mac, receiveCallback, this->x, this->y);
    }

public:
    FakeMeshCommunicator(FakeMesh* mesh, uint8_t* mac, float x = 0, float y = 0) : mesh(mesh), x(x), y(y) {
        memcpy(this->mac, mac, sizeof(this->mac));
    }

    void registerDevice(BaseDevice* device) {
        device->MeshRegisterAddPeer(createAddPeerCallback());
        device->MeshRegisterSend(createSendCallback());
        registerReceiveCallback(device);
    }
};