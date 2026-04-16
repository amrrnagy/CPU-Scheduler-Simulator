#ifndef BACKEND_PRIORITY_H
#define BACKEND_PRIORITY_H

#include "scheduler.h"
#include <queue>

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
    bool tick() override;
    void run()  override;

    [[nodiscard]] int getCurrentProcessId() const override {
        return curr_process ? curr_process->getId() : -1;
    }
};

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

    [[nodiscard]] int getCurrentProcessId() const override {
        return curr_process ? curr_process->getId() : -1;
    }
};

#endif //BACKEND_PRIORITY_H
