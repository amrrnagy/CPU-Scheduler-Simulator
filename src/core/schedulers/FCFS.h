#ifndef BACKEND_FCFS_H
#define BACKEND_FCFS_H

#include "scheduler.h"
class FCFS : public scheduler {
    std::queue<process> ready_queue;
    process * curr_process= nullptr;
    int curr_index=0;
    int block_start_time=0;
    int offline_count=0;
    int com_process=0;
    bool is_sorted=false;
    double totalTurnaroundTime = 0;
    double totalWaitingTime = 0;

public:
    bool tick() override;
};

#endif //BACKEND_FCFS_H