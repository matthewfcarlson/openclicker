#include <unity.h>
#include "../common_test.hpp"
#include <remote/remote.hpp>
#include <bridge/bridge.hpp>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_create_bridge_and_remote(void) {

    FakeMesh* mesh = new FakeMesh();
    // Remote
    RemoteDevice* remote = new RemoteDevice(VoidPrint, reboot_unexpected);
    uint8_t remote_mac[] = {0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0x00};
    FakeMeshCommunicator* remote_comm = new FakeMeshCommunicator(mesh, remote_mac);
    remote_comm->registerDevice(remote);
    // Bridge
    BridgeDevice* bridge = new BridgeDevice(VoidPrint, reboot_unexpected);
    uint8_t bridge_mac[] = {0x01, 0x12, 0x23, 0x34, 0x45, 0x56};
    FakeMeshCommunicator* bridge_comm = new FakeMeshCommunicator(mesh, bridge_mac);
    bridge_comm->registerDevice(bridge);
    // Setup
    bridge->PreSetup();
    remote->PreSetup();
    bridge->Setup();
    remote->Setup();
    for (int i=0; i< 500; i++) {
        remote->Loop();
        bridge->Loop();
    }

    // Make sure we are participating by this point
    TEST_ASSERT_EQUAL_INT(remote->getBigState(), RemoteBigParticipating);
}


int main( int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_create_bridge_and_remote);
    UNITY_END();
}