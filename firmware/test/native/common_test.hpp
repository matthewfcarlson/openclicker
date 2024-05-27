#include <stdio.h>
#include <common/common.h>
#include <functional>
#include <map>
#include <inttypes.h>

void cout_print(const char* buffer, size_t size) {
    if (size == 0) return;
    if (size == 1) {
        printf("%c", buffer[0]);
    }
    else {
        printf("%s", buffer);
    }
}

class VoidPrinter: public Print {
public:
    VoidPrinter() {}
    size_t write(uint8_t c) {
        cout_print((const char*)&c, 1);
        return 0;
    }
    size_t write(const char* buffer, size_t size) {
        cout_print(buffer, size);
        return 0;
    }
};

class FakeMesh {
private:
    std::map<uint64_t, MeshReceive_t> receiverMap;
public:
    FakeMesh() {}
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
        printf("Sending message of %u bytes. %"PRIu64"->%%"PRIu64"\n", msg_size, fromMacId, toMacId);
        if (receiverMap.count(fromMacId) == 0) {
            TEST_FAIL_MESSAGE("This sender did not register");
            return MESH_ERR_INVALID_STATE;
        }
        // Check if it's a broadcast
        else if (toMacId == broadcastMacId) {
            for (auto const& x : receiverMap) {
                if (x.first == fromMacId) continue;
                printf("Broadcasting to %"PRIu64"\n", x.first);
                x.second(from_mac, msg, msg_size);
            }
            return MESH_OK;
        }
        else if (receiverMap.count(toMacId) == 1) {
            receiverMap[toMacId](from_mac, msg, msg_size);
            return MESH_OK;
        }
        else {
            // We did not find this participant
            TEST_FAIL_MESSAGE("This participant did not register");
            return MESH_ERR_INVALID_STATE;
        }
        return MESH_OK;
    }

    void AddReceiver(const uint8_t* to_mac, MeshReceive_t callback) {
        uint64_t toMacId = mac_to_uint64_t(to_mac);
        if (receiverMap.count(toMacId) == 1) {
            TEST_FAIL_MESSAGE("This participant registered twice");
        }
        printf("Adding receiver %"PRIu64"\n", toMacId);
        receiverMap[toMacId] = callback;
    }
};

class FakeMeshCommunicator {
private:
    std::map<uint64_t, bool> peers;
    FakeMesh* mesh;
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
        printf("Creating callback for receiving\n");
        return std::bind(&FakeMeshCommunicator::send, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }
    MeshAddPeer_t createAddPeerCallback() {
        printf("Creating callback for add peer\n");
        return std::bind(&FakeMeshCommunicator::add_peer, this, std::placeholders::_1, std::placeholders::_2);
    }

    void registerReceiveCallback(BaseDevice* device) {
        MeshReceive_t receiveCallback = device->CreateReceiveCallback();
        printf("Registering callback for receiving\n");
        mesh->AddReceiver((const uint8_t*)mac, receiveCallback);
    }

public:
    FakeMeshCommunicator(FakeMesh* mesh, uint8_t* mac) : mesh(mesh) {
        memcpy(this->mac, mac, sizeof(this->mac));
    }

    void registerDevice(BaseDevice* device) {
        device->MeshRegisterAddPeer(createAddPeerCallback());
        device->MeshRegisterSend(createSendCallback());
        registerReceiveCallback(device);
    }
};

VoidPrinter* VoidPrint = new VoidPrinter();

void reboot_unexpected() {
    TEST_FAIL_MESSAGE("Reboot handler was unexpectedly called");
}
void reboot_expected() {
    printf("Reboot was called\n");
}