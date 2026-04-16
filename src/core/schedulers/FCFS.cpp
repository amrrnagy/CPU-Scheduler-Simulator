#include "FCFS.h"
#include <algorithm>

bool FCFS::tick() {
    // 1. Drain the thread-safe incoming queue into our main list (for dynamic live additions)
    bool addedNew = false;
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
        addedNew = true;
    }

    // If new processes were added live, sort the remaining upcoming processes
    if (addedNew) {
        std::sort(processesList.begin() + incomingIndex, processesList.end(),
                  [](const process& a, const process& b) {
                      return a.getArrivalTime() < b.getArrivalTime();
                  });
    }

    // 2. Check for new arrivals using incomingIndex
    // (This is O(1) instead of looping the whole list every millisecond)
    while (incomingIndex < processesList.size() && processesList[incomingIndex].getArrivalTime() <= currentTime) {
        ready_queue.push(&processesList[incomingIndex]);
        incomingIndex++;
    }

    // 3. Dispatch to CPU if Idle
    if (current_process == nullptr && !ready_queue.empty()) {
        current_process  = ready_queue.front();
        ready_queue.pop();
        block_start_time = currentTime;
    }

    // 4. Execute 1 unit of time
    if (current_process != nullptr) {
        current_process->setRemainingTime(current_process->getRemainingTime() - 1);

        // Process finished!
        if (current_process->getRemainingTime() <= 0) {
            // currentTime + 1 because the process finishes at the END of this current tick
            timeline.emplace_back(current_process->getId(), block_start_time, currentTime + 1);

            double turnaround = (currentTime + 1) - current_process->getArrivalTime();
            double waiting    = turnaround - current_process->getBurstTime();

            totalTurnaroundTime += turnaround;
            totalWaitingTime    += waiting;
            completedProcesses++;

            // Floating point division ensures accurate averages
            averageTurnaroundTime = totalTurnaroundTime / static_cast<double>(completedProcesses);
            averageWaitingTime    = totalWaitingTime    / static_cast<double>(completedProcesses);

            current_process = nullptr; // Free the CPU
        }
    }

    // 5. Tick the clock forward
    currentTime++;

    // 6. Keep thread alive if there are pending processes, queued processes, or a running process
    return (incomingIndex < processesList.size() || !ready_queue.empty() || current_process != nullptr);
}

void FCFS::run() {
    // 1. Drain the queue
    while (!incomingProcesses.empty()) {
        processesList.push_back(incomingProcesses.front());
        incomingProcesses.pop();
    }

    // 2. Sort all processes chronologically
    std::sort(processesList.begin(), processesList.end(),
              [](const process& a, const process& b) {
                  return a.getArrivalTime() < b.getArrivalTime();
              });

    // 3. Instantly calculate metrics (Batch mode)
    for (process& p : processesList) {
        // Jump to time forward if CPU is idle
        if (currentTime < p.getArrivalTime()) {
            currentTime = p.getArrivalTime();
        }

        const int start = currentTime;
        currentTime += p.getBurstTime();

        timeline.emplace_back(p.getId(), start, currentTime);

        double turnaround = currentTime - p.getArrivalTime();
        double waiting    = turnaround  - p.getBurstTime();

        totalTurnaroundTime += turnaround;
        totalWaitingTime    += waiting;
        completedProcesses++;

        p.setRemainingTime(0); // Mark as done to keep data consistent
    }

    if (completedProcesses > 0) {
        averageTurnaroundTime = totalTurnaroundTime / static_cast<double>(completedProcesses);
        averageWaitingTime    = totalWaitingTime    / static_cast<double>(completedProcesses);
    }
}

int FCFS::getCurrentProcessId() const {
    return (current_process != nullptr) ? current_process->getId() : -1;
}