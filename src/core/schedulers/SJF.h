#ifndef BACKEND_SJF_H
#define BACKEND_SJF_H

#include "scheduler.h"
#include <queue>

class SJF : public scheduler {
    struct Comp_burst_time {
        bool operator()(const process& a, const process& b) const {
            if (a.getBurstTime() == b.getBurstTime())
                return a.getArrivalTime() > b.getArrivalTime();
            return a.getBurstTime() > b.getBurstTime();
        }
    };

    std::priority_queue<process, std::vector<process>, Comp_burst_time> ready_queue;
    process* curr_process    = nullptr;
    int      block_start_time = 0;
    bool     is_sorted        = false;

public:
    bool tick() override;
    void run()  override;
    // FIX (Bug 9): override added so live Gantt shows the correct process
    int getCurrentProcessId() const override {
        return curr_process ? curr_process->getId() : -1;
    }
};

class SRTF : public scheduler {
    struct Comp_remaining_time {
        bool operator()(const process* a, const process* b) const {
            if (a->getRemainingTime() == b->getRemainingTime())
                return a->getArrivalTime() > b->getArrivalTime();
            return a->getRemainingTime() > b->getRemainingTime();
        }
    };

    std::priority_queue<process*, std::vector<process*>, Comp_remaining_time> ready_queue;
    process* curr_process    = nullptr;
    int      block_start_time = 0;

public:
    bool tick() override;
    void run()  override;
    int getCurrentProcessId() const override {
        return curr_process ? curr_process->getId() : -1;
    }
};

#endif //BACKEND_SJF_H
