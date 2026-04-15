#ifndef BACKEND_SCHEDULER_H
#define BACKEND_SCHEDULER_H

// FIX (Bug 1): Added missing includes that derived classes depend on.
#include "../models/event.h"
#include "../models/process.h"
#include <vector>
#include <queue>

class scheduler {
protected:
    // FIX (Bug 1): All shared state moved here so derived classes can access it.
    // Previously this class only had `timeline`, but SJF and Priority were
    // referencing currentTime, processesList, incomingProcesses, and avg times
    // that didn't exist in the base — causing undefined references.

    std::vector<event>   timeline;
    std::vector<process> processesList;       // static list loaded at startup
    std::queue<process>  incomingProcesses;   // dynamic arrivals added at runtime

    int    currentTime           = 0;
    int    completedProcesses    = 0;
    double totalTurnaroundTime   = 0;
    double totalWaitingTime      = 0;
    double averageTurnaroundTime = 0;
    double averageWaitingTime    = 0;

public:
    virtual ~scheduler() = default;

    // FIX (Bug 2 & 4): Unified tick() and run() declared here so all
    // derived classes override the same signatures.
    //
    // tick()  — advances simulation by one time unit (live mode, 1 call/second).
    //           Returns false when all processes are done.
    // run()   — runs all currently loaded processes to completion instantly
    //           (offline/batch mode, no live delay).
    virtual bool tick() = 0;
    virtual void run()  = 0;

    // Called by the GUI to add a process dynamically while the scheduler runs.
    void addIncomingProcess(const process& p) {
        incomingProcesses.push(p);
    }

    // Accessors for the GUI to read results.
    std::vector<event> getTimeline()           const { return timeline;              }
    double             getAvgWaitingTime()     const { return averageWaitingTime;    }
    double             getAvgTurnaroundTime()  const { return averageTurnaroundTime; }
    int                getCurrentTime()        const { return currentTime;           }

    // Load the initial process list (called before tick() or run()).
    void loadProcesses(const std::vector<process>& list) {
        processesList = list;
    }
};

#endif //BACKEND_SCHEDULER_H
