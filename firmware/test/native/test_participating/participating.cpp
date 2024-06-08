#include <unity.h>
#include "../common_test.hpp"
#include <remote/remote.hpp>
#include <bridge/bridge.hpp>
#include <test/fakemesh.hpp>
#include <test/fakepresenter.hpp>
#include <vector>
#include <algorithm>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_create_bridge_and_remote(void) {

    FakeMesh* mesh = new FakeMesh();
    // Remote
    uint8_t remote_mac[] = {0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0x00};
    RemoteDevice* remote = new RemoteDevice(VoidPrint, reboot_unexpected, remote_mac);
    FakeMeshCommunicator* remote_comm = new FakeMeshCommunicator(mesh);
    remote_comm->registerDevice(remote);
    // Bridge
    uint8_t bridge_mac[] = {0x01, 0x12, 0x23, 0x34, 0x45, 0x56};
    BridgeDevice* bridge = new BridgeDevice(VoidPrint, reboot_unexpected, nullptr, bridge_mac);
    FakeMeshCommunicator* bridge_comm = new FakeMeshCommunicator(mesh);
    bridge_comm->registerDevice(bridge);
    // Setup
    bridge->PreSetup();
    remote->PreSetup();
    bridge->Setup();
    remote->Setup();
    for (int i=0; i< 100; i++) {
        remote->Loop();
        bridge->Loop();
    }

    // Make sure we are participating by this point
    TEST_ASSERT_EQUAL_INT(remote->getBigState(), RemoteBigConnected);
}

void test_create_bridge_and_two_remotes(void) {

    FakeMesh* mesh = new FakeMesh(100);
    std::vector<BaseDevice*> listOfDevices = {};
    // Fake presenter
    FakePresenterTransport* transport = new FakePresenterTransport();
    // Remote
    uint8_t remote_mac[] = {0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0x00};
    NamespacedPrinter* remote1Print = new NamespacedPrinter("remote1");
    RemoteDevice* remote = new RemoteDevice(remote1Print, reboot_unexpected, remote_mac);
    FakeMeshCommunicator* remote_comm = new FakeMeshCommunicator(mesh);
    remote_comm->registerDevice(remote);
    listOfDevices.push_back(remote);
    // Second remote
    uint8_t remote2_mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    NamespacedPrinter* remote2Print = new NamespacedPrinter("remote2");
    RemoteDevice* remote2 = new RemoteDevice(remote2Print, reboot_unexpected, remote2_mac);
    FakeMeshCommunicator* remote2_comm = new FakeMeshCommunicator(mesh);
    remote2_comm->registerDevice(remote2);
    listOfDevices.push_back(remote2);
    // Bridge
    uint8_t bridge_mac[] = {0x01, 0x12, 0x23, 0x34, 0x45, 0x56};
    NamespacedPrinter* bridgePrint = new NamespacedPrinter("bridge");
    BridgeDevice* bridge = new BridgeDevice(bridgePrint, reboot_unexpected, transport, bridge_mac);
    listOfDevices.push_back(bridge);
    FakeMeshCommunicator* bridge_comm = new FakeMeshCommunicator(mesh);
    bridge_comm->registerDevice(bridge);
    // Setup
    std::for_each(listOfDevices.begin(), listOfDevices.end(), [](BaseDevice* x){ x->PreSetup(); });
    std::for_each(listOfDevices.begin(), listOfDevices.end(), [](BaseDevice* x){ x->Setup(); });
    bridge->Setup();
    for (int i=0; i< 100; i++) std::for_each(listOfDevices.begin(), listOfDevices.end(), [](BaseDevice* x){ x->Loop(); });
    // Make sure we are participating by this point
    TEST_ASSERT_EQUAL_INT(remote->getBigState(), RemoteBigParticipating);
    TEST_ASSERT_EQUAL_INT(remote2->getBigState(), RemoteBigParticipating);
}

void test_create_bridge_and_one_close_one_far_remote(void) {

    FakeMesh* mesh = new FakeMesh(100);
    std::vector<BaseDevice*> listOfDevices = {};
    // Fake presenter
    FakePresenterTransport* transport = new FakePresenterTransport();
    // Remote
    NamespacedPrinter* remotePrint = new NamespacedPrinter("remote1");
    uint8_t remote_mac[] = {0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0x00};
    RemoteDevice* remote = new RemoteDevice(remotePrint, reboot_unexpected, remote_mac);
    FakeMeshCommunicator* remote_comm = new FakeMeshCommunicator(mesh, 7);
    remote_comm->registerDevice(remote);
    listOfDevices.push_back(remote);
    // Second remote
    NamespacedPrinter* remote2Print = new NamespacedPrinter("remote2");
    uint8_t remote2_mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    RemoteDevice* remote2 = new RemoteDevice(remote2Print, reboot_unexpected, remote2_mac);
    FakeMeshCommunicator* remote2_comm = new FakeMeshCommunicator(mesh, 12);
    remote2_comm->registerDevice(remote2);
    listOfDevices.push_back(remote2);
    // Bridge
    NamespacedPrinter* bridgePrint = new NamespacedPrinter("bridge");
    uint8_t bridge_mac[] = {0x01, 0x12, 0x23, 0x34, 0x45, 0x56};
    BridgeDevice* bridge = new BridgeDevice(bridgePrint, reboot_unexpected, transport, bridge_mac);
    FakeMeshCommunicator* bridge_comm = new FakeMeshCommunicator(mesh);
    bridge_comm->registerDevice(bridge);
    listOfDevices.push_back(bridge);
    // Setup
    std::for_each(listOfDevices.begin(), listOfDevices.end(), [](BaseDevice* x){ x->PreSetup(); });
    std::for_each(listOfDevices.begin(), listOfDevices.end(), [](BaseDevice* x){ x->Setup(); });
    for (int i=0; i< 100; i++) std::for_each(listOfDevices.begin(), listOfDevices.end(), [](BaseDevice* x){ x->Loop(); });
    // Make sure we are participating by this point
    TEST_ASSERT_EQUAL_INT(remote->getBigState(), RemoteBigParticipating);
    TEST_ASSERT_EQUAL_INT(remote2->getBigState(), RemoteBigConnecting);
}


int main( int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_create_bridge_and_remote);
    RUN_TEST(test_create_bridge_and_two_remotes);
    RUN_TEST(test_create_bridge_and_one_close_one_far_remote);
    UNITY_END();
}