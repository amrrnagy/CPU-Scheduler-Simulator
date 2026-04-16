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

class simulator {
    std::unique_ptr<scheduler> sched;
    std::atomic<bool>          running { false };
    std::atomic<bool>          paused  { false };
    std::thread                sim_thread;
    std::mutex                 process_mutex;

public:
    // FIX (Bug 2): was private — GUI needs to assign this directly.
    std::function<void()> on_tick_callback;

    explicit simulator(SchedulerType type, int rr_quantum = 2);
    ~simulator();

    void loadProcesses(const std::deque<process> &list) const;
    void addProcess(const process& p);

    void setOnTickCallback(std::function<void()> cb) { on_tick_callback = std::move(cb); }

    void start();
    void stop();
    void pause();
    void resume();
    [[nodiscard]] bool isRunning() const { return running.load(); }

    void runBatch() const;

    // FIX (Bug 1): getTimeline() was declared twice here — duplicate causes a compile error.
    // Only one declaration kept.
    [[nodiscard]] std::vector<event> getTimeline()         const { return sched ? sched->getTimeline()         : std::vector<event>{}; }
    [[nodiscard]] double             getAvgWaitingTime()   const { return sched ? sched->getAvgWaitingTime()   : 0.0; }
    [[nodiscard]] double             getAvgTurnaroundTime()const { return sched ? sched->getAvgTurnaroundTime(): 0.0; }
    [[nodiscard]] int                getCurrentTime()      const { return sched ? sched->getCurrentTime()      : 0;   }
    [[nodiscard]] int                getCurrentProcessId() const { return sched ? sched->getCurrentProcessId() : -1;  }
};

#endif //CPUSCHEDULERSIMULATOR_SIMULATOR_H
