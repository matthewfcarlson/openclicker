#include <unity.h>
#include "../common_test.hpp"
#include <remote/remote.hpp>
#include <test/fakemesh.hpp>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_create_remote(void) {

    FakeMesh* mesh = new FakeMesh();
    RemoteDevice* remote = new RemoteDevice(VoidPrint, reboot_unexpected);
    uint8_t mac[] = {0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0x00};
    FakeMeshCommunicator* comm = new FakeMeshCommunicator(mesh, mac);
    comm->registerDevice(remote);
    remote->PreSetup();
    remote->Setup();
    for (int i=0; i< 500; i++) remote->Loop();
}


int main( int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_create_remote);
    UNITY_END();
}