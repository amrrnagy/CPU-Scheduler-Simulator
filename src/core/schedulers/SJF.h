#ifndef BACKEND_SJF_H
#define BACKEND_SJF_H

#include "scheduler.h"
#include <queue>

// Non-preemptive SJF.
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
    ~SJF() override { delete curr_process; }
    bool tick() override;
    void run()  override;

    [[nodiscard]] int getCurrentProcessId() const override {
        return curr_process ? curr_process->getId() : -1;
    }

    // Report the running process's ACTUAL remaining time,
    [[nodiscard]] std::vector<std::pair<int,int>> getRemainingTimes() const override {
        auto result = scheduler::getRemainingTimes(); // base: reads processesList
        if (curr_process) {
            // Replace the stale entry for the currently running process.
            for (auto& [pid, rem] : result) {
                if (pid == curr_process->getId()) {
                    rem = curr_process->getRemainingTime();
                    break;
                }
            }
        }
        return result;
    }
};

// Preemptive SJF (SRTF).
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
    [[nodiscard]] int getCurrentProcessId() const override {
        return curr_process ? curr_process->getId() : -1;
    }
    // SRTF uses pointers into processesList — base implementation is correct.
};

#endif //BACKEND_SJF_H
