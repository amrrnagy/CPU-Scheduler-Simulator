#include "FCFS.h"
#include <algorithm>

bool FCFS::tick() {
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
    }

    for (process& p : processesList) {
        if (p.getArrivalTime() == currentTime && p.getRemainingTime() == p.getBurstTime())
            ready_queue.push(&p);
    }

    if (current_process == nullptr && !ready_queue.empty()) {
        current_process  = ready_queue.front();
        ready_queue.pop();
        block_start_time = currentTime;
    }

    if (current_process != nullptr) {
        current_process->setRemainingTime(current_process->getRemainingTime() - 1);

        if (current_process->getRemainingTime() <= 0) {
            timeline.emplace_back(current_process->getId(), block_start_time, currentTime + 1);

            const int turnaround = (currentTime + 1) - current_process->getArrivalTime();
            const int waiting    = turnaround - current_process->getBurstTime();
            totalTurnaroundTime += turnaround;
            totalWaitingTime    += waiting;
            completedProcesses++;
            averageTurnaroundTime = totalTurnaroundTime / completedProcesses;
            averageWaitingTime    = totalWaitingTime    / completedProcesses;

            current_process = nullptr;
        }
    }

    currentTime++;

    bool pendingArrivals = false;
    for (const process& p : processesList)
        if (p.getArrivalTime() > currentTime) { pendingArrivals = true; break; }

    return (pendingArrivals || !ready_queue.empty() || current_process != nullptr);
}

void FCFS::run() {
    std::sort(processesList.begin(), processesList.end(),
              [](const process& a, const process& b) {
                  return a.getArrivalTime() < b.getArrivalTime();
              });

    for (process& p : processesList) {
        if (currentTime < p.getArrivalTime())
            currentTime = p.getArrivalTime();

        const int start = currentTime;
        currentTime += p.getBurstTime();

        timeline.emplace_back(p.getId(), start, currentTime);

        const int turnaround = currentTime - p.getArrivalTime();
        const int waiting    = turnaround  - p.getBurstTime();
        totalTurnaroundTime += turnaround;
        totalWaitingTime    += waiting;
        completedProcesses++;
    }

    if (completedProcesses > 0) {
        averageTurnaroundTime = totalTurnaroundTime / completedProcesses;
        averageWaitingTime    = totalWaitingTime    / completedProcesses;
    }
}

// FIX (Bug 4): Original had no null check — crash when CPU is idle.
int FCFS::getCurrentProcessId() const {
    return (current_process != nullptr) ? current_process->getId() : -1;
}
