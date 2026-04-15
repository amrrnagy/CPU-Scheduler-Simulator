// simulator.h
// FIX (Bug 10): Previously an empty class. The simulator owns the scheduler,
// drives the tick() loop at 1-second intervals (live mode), and exposes an
// addProcess() method so the GUI can inject new processes at any time.

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
#include <chrono>
#include <mutex>

enum class SchedulerType {
    FCFS,
    SJF_NonPreemptive,
    SJF_Preemptive,       // SRTF
    Priority_NonPreemptive,
    Priority_Preemptive,
    RoundRobin
};

class simulator {
    std::unique_ptr<scheduler> sched;
    std::atomic<bool>          running   { false };
    std::atomic<bool>          paused    { false };
    std::thread                sim_thread;
    std::mutex                 process_mutex;

    // Called every tick so the GUI can refresh the Gantt chart and table.
    std::function<void()> on_tick_callback;

public:
    // Creates the correct scheduler for the chosen algorithm.
    // Call loadProcesses() on the returned simulator before starting.
    explicit simulator(SchedulerType type, int rr_quantum = 2);
    ~simulator();

    // Load the initial list of processes before calling start() or runBatch().
    void loadProcesses(const std::vector<process>& list);

    // Add a process dynamically while the simulator is running.
    // Thread-safe: can be called from the GUI thread.
    void addProcess(const process& p);

    // Register a callback the GUI receives on every tick (for live updates).
    void setOnTickCallback(std::function<void()> cb) { on_tick_callback = std::move(cb); }

    // Live mode: advances one tick per second in a background thread.
    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning() const { return running.load(); }

    // Batch mode: runs to completion instantly, no timer.
    void runBatch();

    // Read-only accessors for the GUI.
    std::vector<event> getTimeline()          const { return sched->getTimeline();          }
    double             getAvgWaitingTime()    const { return sched->getAvgWaitingTime();    }
    double             getAvgTurnaroundTime() const { return sched->getAvgTurnaroundTime(); }
    int                getCurrentTime()       const { return sched->getCurrentTime();       }
};

#endif //CPUSCHEDULERSIMULATOR_SIMULATOR_H
