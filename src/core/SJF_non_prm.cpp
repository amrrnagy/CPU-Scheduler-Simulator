#include "SJF_non_prm.h"
#include <algorithm>
#include <queue>

struct compare {
    bool operator()(const process& a, const process& b) const {
        if (a.getBurstTime()==b.getBurstTime()) {
            return a.getArrivalTime()>b.getArrivalTime();
        }
        return a.getBurstTime()>b.getBurstTime();
    }
};
bool comp_arrival_time(const process& a, const process& b) {
    return a.getArrivalTime() < b.getArrivalTime();
}
void SJF::run() {
    std::sort(processesList.begin(), processesList.end(), comp_arrival_time);
    std::priority_queue<process, std::vector<process>, compare> ready_queue;
    int curr_index=0;
    int n=processesList.size();
    double totalTurnaroundTime=0,totalWaitingTime=0;
    int com_process =0;
    while (com_process < n) {
        while (curr_index<n && processesList[curr_index].getArrivalTime()<=currentTime) {
            ready_queue.push(processesList[curr_index]);
            curr_index++;
        }
        if (ready_queue.empty()) {
            currentTime++;
            continue;
        }
        process curr_process=ready_queue.top();
        ready_queue.pop();
        int start_time=currentTime;
        int end_time=start_time+ curr_process.getBurstTime();
        currentTime=end_time;
        com_process++;
        timeline.push_back(event(curr_process.getId(),start_time,end_time));
        int turnaroundTime = end_time - curr_process.getArrivalTime();
        int waitingTime = turnaroundTime - curr_process.getBurstTime();
        totalTurnaroundTime += turnaroundTime;
        totalWaitingTime += waitingTime;

    }
    if (n > 0) {
        averageTurnaroundTime = totalTurnaroundTime / n;
        averageWaitingTime = totalWaitingTime / n;
    }












}