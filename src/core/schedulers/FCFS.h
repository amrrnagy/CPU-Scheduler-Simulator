#ifndef BACKEND_FCFS_H
#define BACKEND_FCFS_H

#include "scheduler.h"
#include <queue>

class FCFS : public scheduler {
private:
    std::queue<process*> ready_queue;
    process* current_process = nullptr;
    int      block_start_time = 0;

public:
    FCFS() = default;

    // FIX (Bug 2): Now matches the base class signature — no parameters.
    // currentTime is managed as shared state in the base class.
    bool tick() override;
    void run()  override;
};

#endif //BACKEND_FCFS_H