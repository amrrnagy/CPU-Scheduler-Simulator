#include "SJF.h"
#include <algorithm>

// FIX (Bug 5): comp_arrival_time was defined twice in the original file.
// Define it once here as a local lambda inside each function instead
// so there is no duplicate symbol.

// ─── Non-preemptive SJF ───────────────────────────────────────────────────

bool SJF::tick() {
    if (!is_sorted) {
        std::sort(processesList.begin(), processesList.end(),
                  [](const process& a, const process& b) {
                      return a.getArrivalTime() < b.getArrivalTime();
                  });
        is_sorted = true;
    }

    // Admit processes that have arrived.
    for (process& p : processesList) {
        if (p.getArrivalTime() == currentTime && p.getRemainingTime() == p.getBurstTime())
            ready_queue.push(p);
    }

    // Drain dynamic arrivals.
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        ready_queue.push(incomingProcesses.front());
        incomingProcesses.pop();
    }

    if (curr_process == nullptr) {
        if (ready_queue.empty()) {
            currentTime++;
            return true;
        }
        // Copy top process to heap-allocated object so we can mutate remainingTime.
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

// FIX (Bug 6): run() now has a proper closing brace.
// FIX (Bug 5): comp_arrival_time lambda defined inline — no duplicate symbol.
void SJF::run() {
    std::sort(processesList.begin(), processesList.end(),
              [](const process& a, const process& b) {
                  return a.getArrivalTime() < b.getArrivalTime();
              });

    // Also drain any dynamic processes added before run() was called.
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
    }

    // Use a local priority queue for the batch simulation.
    struct Cmp {
        bool operator()(const process& a, const process& b) const {
            if (a.getBurstTime() == b.getBurstTime())
                return a.getArrivalTime() > b.getArrivalTime();
            return a.getBurstTime() > b.getBurstTime();
        }
    };
    std::priority_queue<process, std::vector<process>, Cmp> pq;

    int idx = 0;
    int n   = (int)processesList.size();

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
} // FIX (Bug 6): this brace was missing in the original.

// ─── Preemptive SJF (SRTF) ───────────────────────────────────────────────

bool SRTF::tick() {
    // Admit processes that have arrived and push their pointer into the ready queue.
    for (process& p : processesList) {
        if (p.getArrivalTime() == currentTime && p.getRemainingTime() == p.getBurstTime())
            ready_queue.push(&p);
    }
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
        ready_queue.push(&processesList.back());
    }

    // Preemption: if the ready queue has a shorter remaining time, switch.
    if (!ready_queue.empty()) {
        process* shortest = ready_queue.top();
        if (curr_process == nullptr || shortest->getRemainingTime() < curr_process->getRemainingTime()) {
            if (curr_process != nullptr) {
                // Record the partial block before preempting.
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

void SRTF::run() {
    // SRTF batch mode: simulate tick-by-tick but without the 1-second delay.
    while (tick()) { /* run to completion */ }
}
