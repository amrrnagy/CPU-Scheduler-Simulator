#ifndef BACKEND_RR_H
#define BACKEND_RR_H

// FIX (Bug 8): RR was a completely empty class that didn't even inherit
// from scheduler. This is now a proper implementation.

#include "scheduler.h"
#include <queue>

class RR : public scheduler {
    std::queue<process*> ready_queue;
    process* curr_process    = nullptr;
    int      quantum         = 2;    // default time quantum; set via constructor
    int      quantum_counter = 0;    // how many ticks the current process has run
    int      block_start_time = 0;

public:
    explicit RR(int q = 2) : quantum(q) {}

    bool tick() override;
    void run()  override;
};

#endif //BACKEND_RR_H