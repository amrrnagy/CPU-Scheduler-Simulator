#ifndef CPUSCHEDULERSIMULATOR_SIMULATOR_H
#define CPUSCHEDULERSIMULATOR_SIMULATOR_H

#include "schedulers/scheduler.h"
#include "schedulers/FCFS.h"
#include "schedulers/SJF.h"
#include "schedulers/RR.h"
#include "schedulers/priority.h"

#include <memory>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

enum class SchedulerType {
    FCFS,
    SJF_NonPreemptive,
    SJF_Preemptive,
    Priority_NonPreemptive,
    Priority_Preemptive,
    RoundRobin
};

// single atomic snapshot
struct SimSnapshot {
    std::vector<event>              timeline;
    int                             currentProcessId = -1;
    double                          avgWaitingTime   = 0;
    double                          avgTurnaroundTime= 0;
    int                             currentTime      = 0;
    std::vector<std::pair<int,int>> remainingTimes;  // (pid, remaining) for table updates
};

class simulator {
    std::unique_ptr<scheduler> sched;
    std::atomic<bool>          running { false };
    std::atomic<bool>          paused  { false };
    std::thread                sim_thread;
    std::mutex                 process_mutex;

public:
    std::function<void()> on_tick_callback;

    explicit simulator(SchedulerType type, int rr_quantum = 2);
    ~simulator();

    void loadProcesses(const std::deque<process>& list) const;
    void addProcess(const process& p);

    void start();
    void stop();
    [[nodiscard]] bool isRunning() const { return running.load(); }

    void runBatch() const;

    // The GUI calls this once per tick and gets a consistent view of state.
    [[nodiscard]] SimSnapshot getSnapshot();

    // For batch (instant) mode — safe because the thread is stopped.
    [[nodiscard]] std::vector<event> getTimelineUnsafe() const {
        return sched ? sched->getTimeline() : std::vector<event>{};
    }
    [[nodiscard]] double getAvgWaitingTimeUnsafe()    const { return sched ? sched->getAvgWaitingTime()    : 0.0; }
    [[nodiscard]] double getAvgTurnaroundTimeUnsafe() const { return sched ? sched->getAvgTurnaroundTime() : 0.0; }
};

#endif //CPUSCHEDULERSIMULATOR_SIMULATOR_H