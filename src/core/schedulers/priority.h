#ifndef BACKEND_PRIORITY_H
#define BACKEND_PRIORITY_H

#include "scheduler.h"
#include <queue>

// Non-preemptive priority scheduling.
// Lower priority number = higher priority (as per project spec).
class Priority_Non_Prm : public scheduler {
    struct Comp_priority {
        bool operator()(const process& a, const process& b) const {
            if (a.getPriority() == b.getPriority())
                return a.getArrivalTime() > b.getArrivalTime(); // tie-break
            return a.getPriority() > b.getPriority();           // lower number = higher priority
        }
    };

    std::priority_queue<process, std::vector<process>, Comp_priority> ready_queue;
    process* curr_process    = nullptr;
    int      block_start_time = 0;
    bool     is_sorted        = false;

public:
    bool tick() override;
    void run()  override;
};

// FIX (Bug 7): Preemptive priority scheduling — was completely missing.
// On each tick, the running process can be preempted if a higher-priority
// process arrives.
class Priority_Prm : public scheduler {
    struct Comp_priority {
        bool operator()(const process* a, const process* b) const {
            if (a->getPriority() == b->getPriority())
                return a->getArrivalTime() > b->getArrivalTime();
            return a->getPriority() > b->getPriority();
        }
    };

    std::priority_queue<process*, std::vector<process*>, Comp_priority> ready_queue;
    process* curr_process    = nullptr;
    int      block_start_time = 0;

public:
    bool tick() override;
    void run()  override;
};

#endif //BACKEND_PRIORITY_H
