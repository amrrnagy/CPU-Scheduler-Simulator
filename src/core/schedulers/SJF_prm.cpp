#include "../schedulers//SJF_prm.h"
#include <algorithm>
#include <queue>


bool comp_arrival_time(const process& a, const process& b) {
    return a.getArrivalTime() < b.getArrivalTime();
}
bool SJF::tick() {
    if (!is_sorted) {
        std::sort(processesList.begin(), processesList.end(), comp_arrival_time);
        offline_count = processesList.size();
        is_sorted = true;
    }

    while (curr_index < offline_count && processesList[curr_index].getArrivalTime() <= currentTime) {
        ready_queue.push(processesList[curr_index]);
        curr_index++;
    }

    while (!incomingProcesses.empty()) {
        process p = incomingProcesses.front();
        incomingProcesses.pop();
        processesList.push_back(p);
        ready_queue.push(p);
    }
    if (curr_process !=nullptr && ! ready_queue.empty()) {
        if (ready_queue.top().getRemainingTime()< curr_process -> getRemainingTime()) {
            timeline.push_back(event(curr_process->getId(), block_start_time, currentTime));
            ready_queue.push(*curr_process);
            delete curr_process;
            curr_process = nullptr;
        }

        }

        if (curr_process == nullptr) {
            if (ready_queue.empty()) {
                currentTime++;
                return true;
            }

            curr_process = new process(ready_queue.top());
            ready_queue.pop();

            block_start_time = currentTime;
        }

        curr_process->setRemainingTime(curr_process->getRemainingTime() - 1);
        currentTime++;

        if (curr_process->getRemainingTime() == 0) {
            timeline.push_back(event(curr_process->getId(), block_start_time, currentTime));

            int turnaroundTime = currentTime - curr_process->getArrivalTime();
            int waitingTime = turnaroundTime - curr_process->getBurstTime();

            totalTurnaroundTime += turnaroundTime;
            totalWaitingTime += waitingTime;
            com_process++;

            averageTurnaroundTime = totalTurnaroundTime / processesList.size();
            averageWaitingTime = totalWaitingTime / processesList.size();

            delete curr_process;
            curr_process = nullptr;
        }

        if (com_process == processesList.size() && ready_queue.empty() && incomingProcesses.empty() && curr_process == nullptr) {
            return false;

        }

        return true;



    }

