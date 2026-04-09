#include <queue>
#include "FCFS.h"
#include <algorithm>
bool comabre (const process & a , const process & b) {
    return a.getArrivalTime()<b.getArrivalTime();
}

void FCFS::run() {

    std::sort(processesList.begin(), processesList.end(), comabre);
    double totalTurnaroundTime=0,totalWaitingTime=0;
    std::queue<process> ready_queue;
    int curr_index=0;
    int n=processesList.size();
    int com_processes =0;

    while (com_processes<n) {

        while (curr_index<n && processesList[curr_index].getArrivalTime()<=currentTime){
            ready_queue.push(processesList[curr_index]);
            curr_index++;}
        if (ready_queue.empty()) {
            currentTime++;
            continue;
        }

        process curr_process=ready_queue.front();
        ready_queue.pop();
        int start_time=currentTime;
        int end_time=start_time+ curr_process.getBurstTime();
        currentTime=end_time;
        com_processes++;
        timeline.push_back(event(curr_process.getId(),start_time,end_time));
        int turnaroundTime = end_time - curr_process.getArrivalTime();
        int waitingTime = turnaroundTime - curr_process.getBurstTime();
        totalTurnaroundTime += turnaroundTime;
        totalWaitingTime += waitingTime;
    // for (process process : processesList) {
    //
    //     if (currentTime < process.getArrivalTime()) {
    //         currentTime = process.getArrivalTime();
    //     }
    //     int startTime = currentTime;
    //     currentTime += process.getBurstTime();
    //     int endTime = currentTime;
    //     timeline.push_back(event(process.getId(), startTime, endTime));
    //     int turnaroundTime = endTime - process.getArrivalTime();
    //     int waitingTime = turnaroundTime - process.getBurstTime();
    //     totalTurnaroundTime += turnaroundTime;
    //     totalWaitingTime += waitingTime;
    }
    if (processesList.size() > 0) {
        averageTurnaroundTime = totalTurnaroundTime / processesList.size();
        averageWaitingTime = totalWaitingTime / processesList.size();
    }



}
