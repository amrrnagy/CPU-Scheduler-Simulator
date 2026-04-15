
#ifndef BACKEND_SJF_H
#define BACKEND_SJF_H
#include "scheduler.h"
class SJF : public scheduler {

    struct Comp_rem_time {
        bool operator()(const process& a, const process& b) const {
            if (a.getRemainingTime() == b.getRemainingTime()) {
                return a.getArrivalTime() > b.getArrivalTime();
            }
            return a.getRemainingTime() > b.getRemainingTime();
        }
    };


    std::priority_queue<process, std::vector<process>, Comp_rem_time> ready_queue;
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
#endif //BACKEND_SJF_H