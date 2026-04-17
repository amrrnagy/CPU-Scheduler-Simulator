#ifndef BACKEND_SCHEDULER_H
#define BACKEND_SCHEDULER_H

#include "../models/event.h"
#include "../models/process.h"
#include <vector>
#include <queue>
#include <deque>

class scheduler {
protected:
    std::vector<event>  timeline;
    std::deque<process> processesList;
    std::queue<process> incomingProcesses;

    int incomingIndex = 0;

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

    void addIncomingProcess(const process& p) { incomingProcesses.push(p); }

    [[nodiscard]] std::vector<event> getTimeline()          const { return timeline;              }
    [[nodiscard]] double             getAvgWaitingTime()    const { return averageWaitingTime;    }
    [[nodiscard]] double             getAvgTurnaroundTime() const { return averageTurnaroundTime; }
    [[nodiscard]] int                getCurrentTime()       const { return currentTime;           }

    void loadProcesses(const std::deque<process>& list) {
        processesList = list;
        incomingIndex = 0;
    }

    [[nodiscard]] virtual int getCurrentProcessId() const { return -1; }
    
    [[nodiscard]] virtual std::vector<std::pair<int,int>> getRemainingTimes() const {
        std::vector<std::pair<int,int>> result;
        result.reserve(processesList.size());
        for (const process& p : processesList)
            result.emplace_back(p.getId(), p.getRemainingTime());
        return result;
    }
};

#endif //BACKEND_SCHEDULER_H
