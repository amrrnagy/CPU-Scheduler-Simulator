#include "FCFS.h"
#include <algorithm>

bool FCFS::tick() {
    // Drain any dynamically added processes into the ready queue.
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
    }

    // Admit processes whose arrival time has been reached.
    for (process& p : processesList) {
        if (p.getArrivalTime() == currentTime && p.getRemainingTime() == p.getBurstTime()) {
            ready_queue.push(&p);
        }
    }

    // Pick the next process if the CPU is idle.
    if (current_process == nullptr && !ready_queue.empty()) {
        current_process  = ready_queue.front();
        ready_queue.pop();
        block_start_time = currentTime;
    }

    // Execute one time unit.
    if (current_process != nullptr) {
        current_process->setRemainingTime(current_process->getRemainingTime() - 1);

        if (current_process->getRemainingTime() <= 0) {
            // FIX (Bug 3): event constructor is event(id, start, end) — not {time, id, string}.
            timeline.push_back(event(current_process->getId(), block_start_time, currentTime + 1));

            int turnaround = (currentTime + 1) - current_process->getArrivalTime();
            int waiting    = turnaround - current_process->getBurstTime();
            totalTurnaroundTime += turnaround;
            totalWaitingTime    += waiting;
            completedProcesses++;

            if (completedProcesses > 0) {
                averageTurnaroundTime = totalTurnaroundTime / completedProcesses;
                averageWaitingTime    = totalWaitingTime    / completedProcesses;
            }

            current_process = nullptr;
        }
    }

    currentTime++;

    bool pendingArrivals = false;
    for (const process& p : processesList) {
        if (p.getArrivalTime() > currentTime) { pendingArrivals = true; break; }
    }

    return (pendingArrivals || !ready_queue.empty() || current_process != nullptr);
}

void FCFS::run() {
    // Sort by arrival time for non-live batch mode.
    std::sort(processesList.begin(), processesList.end(),
              [](const process& a, const process& b) {
                  return a.getArrivalTime() < b.getArrivalTime();
              });

    for (process& p : processesList) {
        if (currentTime < p.getArrivalTime())
            currentTime = p.getArrivalTime();   // skip idle gap

        int start = currentTime;
        currentTime += p.getBurstTime();

        timeline.push_back(event(p.getId(), start, currentTime));

        int turnaround = currentTime - p.getArrivalTime();
        int waiting    = turnaround  - p.getBurstTime();
        totalTurnaroundTime += turnaround;
        totalWaitingTime    += waiting;
        completedProcesses++;
    }

    if (completedProcesses > 0) {
        averageTurnaroundTime = totalTurnaroundTime / completedProcesses;
        averageWaitingTime    = totalWaitingTime    / completedProcesses;
    }
}