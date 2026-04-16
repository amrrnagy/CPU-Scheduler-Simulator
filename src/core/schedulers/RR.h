#ifndef BACKEND_RR_H
#define BACKEND_RR_H

#include "scheduler.h"
#include <queue>

class RR : public scheduler {
    std::queue<process*> ready_queue;
    process* curr_process    = nullptr;
    int      quantum         = 2;
    int      quantum_counter = 0;
    int      block_start_time = 0;

public:
    explicit RR(int q = 2) : quantum(q) {}

    bool tick() override;
    void run()  override;

    [[nodiscard]] int getCurrentProcessId() const override {
        return curr_process ? curr_process->getId() : -1;
    }
};

#endif //BACKEND_RR_H
