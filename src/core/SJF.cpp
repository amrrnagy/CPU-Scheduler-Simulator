#include "SJF.h"
#include <algorithm>
#include<queue>



struct Comp_rem_time {
    bool operator()(const process& a, const process& b) const {
        if (a.getRemainingTime() == b.getRemainingTime()) {
            return a.getArrivalTime() > b.getArrivalTime();
        }
        return a.getRemainingTime() > b.getRemainingTime();
    }
};
bool comp(const process& a, const process& b) {
    return a.getArrivalTime() < b.getArrivalTime();
}
void SJF::run() {
    std::sort(processesList.begin(), processesList.end(), comp);
    std::priority_queue<process, std::vector<process>, Comp_rem_time> ready_queue;
    int curr_index=0;
    int n=processesList.size();
    double totalTurnaroundTime=0,totalWaitingTime=0;
    int last_process_id=-1;
    int block_start_time = 0;
    int com_process =0;
    while (com_process < n) {
        while (curr_index < n &&  processesList[curr_index].getArrivalTime()<=currentTime) {
            ready_queue.push(processesList[curr_index]);
            curr_index++;
        }
        if (ready_queue.empty()) {
            currentTime++;
            continue;
        }
        process currentProcess = ready_queue.top();
        ready_queue.pop();

        if (currentProcess.getId() != last_process_id) {
            if (last_process_id != -1) {
                timeline.push_back(event(last_process_id, block_start_time, currentTime));
            }
            block_start_time = currentTime;
            last_process_id = currentProcess.getId();
        }
        currentProcess.setRemainingTime(currentProcess.getRemainingTime() - 1);
        currentTime++;
        if (currentProcess.getRemainingTime() == 0) {
            timeline.push_back(event(currentProcess.getId(), block_start_time, currentTime));
            last_process_id = -1;

            int turnaroundTime = currentTime - currentProcess.getArrivalTime();
            int waitingTime = turnaroundTime - currentProcess.getBurstTime();

            totalTurnaroundTime += turnaroundTime;
            totalWaitingTime += waitingTime;
            com_process++;}else {
                ready_queue.push(currentProcess);



            }


    }
    if (n > 0) {
        averageTurnaroundTime = totalTurnaroundTime / n;
        averageWaitingTime = totalWaitingTime / n;
    }
}