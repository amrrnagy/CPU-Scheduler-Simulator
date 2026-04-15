#ifndef BACKEND_PRIORITY_NON_PRM_H
#define BACKEND_PRIORITY_NON_PRM_H

#include "scheduler.h"
#include <queue>

class Priority_Non_Prm : public scheduler {
private:
    struct Comp_priority {
        bool operator()(const process& a, const process& b) const {
            if (a.getPriority() == b.getPriority()) {
                return a.getArrivalTime() > b.getArrivalTime();
            }
            return a.getPriority() > b.getPriority();
        }
    };

    std::priority_queue<process, std::vector<process>, Comp_priority> ready_queue;
    process* curr_process = nullptr;

    int curr_index = 0;
    int offline_count = 0;
    int com_process = 0;
    int block_start_time = 0;
    bool is_sorted = false;

    double totalTurnaroundTime = 0;
    double totalWaitingTime = 0;

public:
    bool tick() override;
};

#endif //BACKEND_PRIORITY_NON_PRM_H