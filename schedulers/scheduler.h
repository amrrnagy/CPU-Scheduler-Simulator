//
// Created by Lenovo on 4/9/2026.
//

#ifndef BACKEND_SCHEDULER_H
#define BACKEND_SCHEDULER_H
#include "../models/event.h"
#include "../models/process.h"
#include <vector>
#include <queue>

class scheduler {
    protected:
    int currentTime;
    std::vector<process> processesList;
    std::vector<event> timeline;
    std::queue<process> incomingProcesses;
    double averageWaitingTime,averageTurnaroundTime;
public:
    scheduler() : currentTime(0), averageWaitingTime(0.0), averageTurnaroundTime(0.0) {}
    virtual ~scheduler() = default;
    void addProcess(const process& p) {
        incomingProcesses.push(p);
    }

    std::vector<event> getTimeline() const { return timeline; }
    std::vector<process> getProcessesList() const { return processesList; }
    double getAverageWaitingTime() const { return averageWaitingTime; }
    double getAverageTurnaroundTime() const { return averageTurnaroundTime; }

    // void run(){}
    virtual void run() = 0;
    virtual bool tick() = 0;
};



#endif //BACKEND_SCHEDULER_H