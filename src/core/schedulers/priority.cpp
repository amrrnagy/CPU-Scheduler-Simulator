#include "priority.h"
#include <algorithm>

// ─── Non-preemptive Priority ──────────────────────────────────────────────

bool Priority_Non_Prm::tick() {
    if (!is_sorted) {
        std::sort(processesList.begin(), processesList.end(),
                  [](const process& a, const process& b) {
                      return a.getArrivalTime() < b.getArrivalTime();
                  });
        is_sorted = true;
    }

    for (process& p : processesList) {
        if (p.getArrivalTime() == currentTime && p.getRemainingTime() == p.getBurstTime())
            ready_queue.push(p);
    }
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        ready_queue.push(incomingProcesses.front());
        incomingProcesses.pop();
    }

    if (curr_process == nullptr) {
        if (ready_queue.empty()) { currentTime++; return true; }
        curr_process     = new process(ready_queue.top());
        block_start_time = currentTime;
        ready_queue.pop();
    }

    curr_process->setRemainingTime(curr_process->getRemainingTime() - 1);
    currentTime++;

    if (curr_process->getRemainingTime() == 0) {
        timeline.push_back(event(curr_process->getId(), block_start_time, currentTime));

        int turnaround = currentTime - curr_process->getArrivalTime();
        int waiting    = turnaround  - curr_process->getBurstTime();
        totalTurnaroundTime += turnaround;
        totalWaitingTime    += waiting;
        completedProcesses++;
        averageTurnaroundTime = totalTurnaroundTime / completedProcesses;
        averageWaitingTime    = totalWaitingTime    / completedProcesses;

        delete curr_process;
        curr_process = nullptr;
    }

    bool anyPending = (int)processesList.size() > completedProcesses;
    return anyPending || !ready_queue.empty() || curr_process != nullptr;
}

void Priority_Non_Prm::run() {
    std::sort(processesList.begin(), processesList.end(),
              [](const process& a, const process& b) {
                  return a.getArrivalTime() < b.getArrivalTime();
              });
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
    }

    struct Cmp {
        bool operator()(const process& a, const process& b) const {
            if (a.getPriority() == b.getPriority()) return a.getArrivalTime() > b.getArrivalTime();
            return a.getPriority() > b.getPriority();
        }
    };
    std::priority_queue<process, std::vector<process>, Cmp> pq;

    int idx = 0, n = (int)processesList.size();
    while (completedProcesses < n) {
        while (idx < n && processesList[idx].getArrivalTime() <= currentTime)
            pq.push(processesList[idx++]);

        if (pq.empty()) { currentTime++; continue; }

        process curr = pq.top(); pq.pop();
        int start    = currentTime;
        currentTime += curr.getBurstTime();

        timeline.push_back(event(curr.getId(), start, currentTime));

        int turnaround = currentTime - curr.getArrivalTime();
        int waiting    = turnaround  - curr.getBurstTime();
        totalTurnaroundTime += turnaround;
        totalWaitingTime    += waiting;
        completedProcesses++;
    }

    if (completedProcesses > 0) {
        averageTurnaroundTime = totalTurnaroundTime / completedProcesses;
        averageWaitingTime    = totalWaitingTime    / completedProcesses;
    }
}

// ─── Preemptive Priority (FIX Bug 7) ─────────────────────────────────────

bool Priority_Prm::tick() {
    for (process& p : processesList) {
        if (p.getArrivalTime() == currentTime && p.getRemainingTime() == p.getBurstTime())
            ready_queue.push(&p);
    }
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
        ready_queue.push(&processesList.back());
    }

    // Preemption check: if a higher-priority process is waiting, switch to it.
    if (!ready_queue.empty()) {
        process* highest = ready_queue.top();
        if (curr_process == nullptr || highest->getPriority() < curr_process->getPriority()) {
            if (curr_process != nullptr) {
                // Save partial execution block.
                timeline.push_back(event(curr_process->getId(), block_start_time, currentTime));
                ready_queue.push(curr_process);
            }
            curr_process     = ready_queue.top();
            block_start_time = currentTime;
            ready_queue.pop();
        }
    }

    if (curr_process != nullptr) {
        curr_process->setRemainingTime(curr_process->getRemainingTime() - 1);

        if (curr_process->getRemainingTime() == 0) {
            timeline.push_back(event(curr_process->getId(), block_start_time, currentTime + 1));

            int turnaround = (currentTime + 1) - curr_process->getArrivalTime();
            int waiting    = turnaround - curr_process->getBurstTime();
            totalTurnaroundTime += turnaround;
            totalWaitingTime    += waiting;
            completedProcesses++;
            averageTurnaroundTime = totalTurnaroundTime / completedProcesses;
            averageWaitingTime    = totalWaitingTime    / completedProcesses;

            curr_process = nullptr;
        }
    }

    currentTime++;
    bool anyPending = (int)processesList.size() > completedProcesses;
    return anyPending || !ready_queue.empty() || curr_process != nullptr;
}

void Priority_Prm::run() {
    while (tick()) { /* run to completion */ }
}