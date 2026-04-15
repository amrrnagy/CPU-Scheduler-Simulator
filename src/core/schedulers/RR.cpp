#include "RR.h"
#include <algorithm>

bool RR::tick() {
    // Admit new arrivals (from static list and dynamic queue).
    for (process& p : processesList) {
        if (p.getArrivalTime() == currentTime && p.getRemainingTime() == p.getBurstTime())
            ready_queue.push(&p);
    }
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
        ready_queue.push(&processesList.back());
    }

    // If the current process has exhausted its quantum (and isn't finished),
    // push it back to the tail of the ready queue.
    if (curr_process != nullptr && quantum_counter >= quantum) {
        timeline.push_back(event(curr_process->getId(), block_start_time, currentTime));
        if (curr_process->getRemainingTime() > 0)
            ready_queue.push(curr_process);
        curr_process    = nullptr;
        quantum_counter = 0;
    }

    // Pick next process if CPU is idle.
    if (curr_process == nullptr && !ready_queue.empty()) {
        curr_process     = ready_queue.front();
        ready_queue.pop();
        block_start_time = currentTime;
        quantum_counter  = 0;
    }

    // Execute one time unit.
    if (curr_process != nullptr) {
        curr_process->setRemainingTime(curr_process->getRemainingTime() - 1);
        quantum_counter++;

        if (curr_process->getRemainingTime() == 0) {
            // Process finished before the quantum expired.
            timeline.push_back(event(curr_process->getId(), block_start_time, currentTime + 1));

            int turnaround = (currentTime + 1) - curr_process->getArrivalTime();
            int waiting    = turnaround - curr_process->getBurstTime();
            totalTurnaroundTime += turnaround;
            totalWaitingTime    += waiting;
            completedProcesses++;
            averageTurnaroundTime = totalTurnaroundTime / completedProcesses;
            averageWaitingTime    = totalWaitingTime    / completedProcesses;

            curr_process    = nullptr;
            quantum_counter = 0;
        }
    }

    currentTime++;
    bool anyPending = (int)processesList.size() > completedProcesses;
    return anyPending || !ready_queue.empty() || curr_process != nullptr;
}

void RR::run() {
    std::sort(processesList.begin(), processesList.end(),
              [](const process& a, const process& b) {
                  return a.getArrivalTime() < b.getArrivalTime();
              });
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
    }
    while (tick()) { /* run to completion */ }
}