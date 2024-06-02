#include <stdio.h>
#include <common/common.h>
#include <functional>
#include <map>
#include <inttypes.h>

void reboot_unexpected() {
    TEST_FAIL_MESSAGE("Reboot handler was unexpectedly called");
}
void reboot_expected() {
    printf("Reboot was called\n");
}