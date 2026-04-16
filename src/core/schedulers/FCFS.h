#ifndef BACKEND_FCFS_H
#define BACKEND_FCFS_H

#include "scheduler.h"
#include <queue>

class FCFS : public scheduler {
    std::queue<process*> ready_queue;
    process* current_process = nullptr;
    int      block_start_time = 0;

public:
    FCFS() = default;
    bool tick() override;
    void run()  override;

    [[nodiscard]] int getCurrentProcessId() const override;
};

#endif //BACKEND_FCFS_H
