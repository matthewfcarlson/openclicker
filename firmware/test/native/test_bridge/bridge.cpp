#include <unity.h>
#include "common_test.hpp"
#include "bridge/bridge.hpp"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_create_bridge(void) {
    BridgeDevice* bridge = new BridgeDevice(VoidPrint, reboot_unexpected);
    bridge->PreSetup();
    bridge->Setup();
    for (int i=0; i< 500; i++) bridge->Loop();
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_create_bridge);
    UNITY_END();
}