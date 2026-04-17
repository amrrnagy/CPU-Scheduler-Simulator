#ifndef BACKEND_PRIORITY_H
#define BACKEND_PRIORITY_H

#include "scheduler.h"
#include <queue>

// Non-preemptive priority scheduling.
class Priority_Non_Prm : public scheduler {
    struct Comp_priority {
        bool operator()(const process& a, const process& b) const {
            if (a.getPriority() == b.getPriority())
                return a.getArrivalTime() > b.getArrivalTime();
            return a.getPriority() > b.getPriority();
        }
    };

    std::priority_queue<process, std::vector<process>, Comp_priority> ready_queue;
    process* curr_process    = nullptr;
    int      block_start_time = 0;
    bool     is_sorted        = false;

public:
    ~Priority_Non_Prm() override { delete curr_process; }
    bool tick() override;
    void run()  override;

    [[nodiscard]] int getCurrentProcessId() const override {
        return curr_process ? curr_process->getId() : -1;
    }

    [[nodiscard]] std::vector<std::pair<int,int>> getRemainingTimes() const override {
        auto result = scheduler::getRemainingTimes();
        if (curr_process) {
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

// Preemptive priority scheduling.
class Priority_Prm : public scheduler {
    struct Comp_priority {
        bool operator()(const process* a, const process* b) const {
            if (a->getPriority() == b->getPriority())
                return a->getArrivalTime() > b->getArrivalTime();
            return a->getPriority() > b->getPriority();
        }
    };

    std::priority_queue<process*, std::vector<process*>, Comp_priority> ready_queue;
    process* curr_process    = nullptr;   // pointer into processesList — NO copy
    int      block_start_time = 0;

public:
    bool tick() override;
    void run()  override;

    [[nodiscard]] int getCurrentProcessId() const override {
        return curr_process ? curr_process->getId() : -1;
    }
    // Priority_Prm uses pointers — base getRemainingTimes() is correct.
};

#endif //BACKEND_PRIORITY_H
