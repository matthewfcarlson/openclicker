#include <functional>
#include <stdint.h>
#include "common/common.h"
#undef isnan     // Undefine any existing isnan macro
#include <cmath>
#include <chrono>


#pragma once

class PeriodicTask {
protected:
    uint32_t millsBetweenChecks;
    uint64_t lastTaskTime = 0;
    std::function<void()> callback;
    #ifdef PIO_UNIT_TESTING
    uint64_t fakeClockMillis = 0;
    #endif

    uint64_t getCurrentTimeInMilliseconds() {
        // If we are in a unit testing environment, every time we check it's another tenth of a second on the clock
        #ifdef PIO_UNIT_TESTING
        this->fakeClockMillis += 100;
        return this->fakeClockMillis;
        #endif
        // Get the current time point from steady_clock
        auto now = std::chrono::steady_clock::now();
        
        // Convert the time point to the duration since the epoch
        auto duration = now.time_since_epoch();
        
        // Convert the duration to milliseconds
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        
        // Return the time in milliseconds as uint64_t
        return static_cast<uint64_t>(millis);
    }
public:
    PeriodicTask(uint32_t millsBetweenChecks, std::function<void()> callback) : millsBetweenChecks(millsBetweenChecks), callback(callback) {};
    void Check() {
        uint64_t currentTime = getCurrentTimeInMilliseconds();
        if (currentTime >= (this->lastTaskTime + millsBetweenChecks)) {
            this->lastTaskTime = currentTime;
            callback();
        }
    }
};