#include <stdint.h>
#include "common.h"
#include <chrono>

#pragma once

class PeriodicTask {
protected:
    uint32_t millsBetweenChecks;
    uint64_t last_task_time = 0;
    std::function<void()> callback;

    uint64_t getCurrentTimeInMilliseconds() {
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
        if (currentTime >= (this->last_task_time + millsBetweenChecks)) {
            this->last_task_time = currentTime;
            callback();
        }
    }
};