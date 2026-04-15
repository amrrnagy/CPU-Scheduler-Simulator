#ifndef BACKEND_SCHEDULER_H
#define BACKEND_SCHEDULER_H

#include "../models/event.h"
#include "../models/process.h"
#include <vector>
#include <queue>

class scheduler {
protected:
    std::vector<event>   timeline;
    std::vector<process> processesList;
    std::queue<process>  incomingProcesses;

    int    currentTime           = 0;
    int    completedProcesses    = 0;
    double totalTurnaroundTime   = 0;
    double totalWaitingTime      = 0;
    double averageTurnaroundTime = 0;
    double averageWaitingTime    = 0;

public:
    virtual ~scheduler() = default;

    virtual bool tick() = 0;
    virtual void run()  = 0;

    void addIncomingProcess(const process& p) {
        incomingProcesses.push(p);
    }

    [[nodiscard]] std::vector<event> getTimeline()          const { return timeline;              }
    [[nodiscard]] double             getAvgWaitingTime()    const { return averageWaitingTime;    }
    [[nodiscard]] double             getAvgTurnaroundTime() const { return averageTurnaroundTime; }
    [[nodiscard]] int                getCurrentTime()       const { return currentTime;           }

    void loadProcesses(const std::vector<process>& list) {
        processesList = list;
    }

    // FIX (Bug 3): Was declared but never defined — caused a linker error.
    // Default returns -1 (idle). Derived classes override to return their
    // specific current_process pointer's id.
    virtual int getCurrentProcessId() const { return -1; }
};

#endif //BACKEND_SCHEDULER_H
