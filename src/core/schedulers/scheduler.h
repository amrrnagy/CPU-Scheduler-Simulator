#ifndef BACKEND_SCHEDULER_H
#define BACKEND_SCHEDULER_H

#include "../models/event.h"
#include "../models/process.h"
#include <vector>
#include <queue>

class scheduler {
protected:
    // Protected means all your child classes (FCFS, RR, etc.) can write to this directly.
    // Every algorithm will generate a timeline, so keeping it in the base class saves duplicate code.
    std::vector<event> timeline;

public:
    // A virtual destructor is critical in C++ polymorphic base classes to prevent memory leaks.
    virtual ~scheduler() = default;

    virtual bool tick(int currentTime, std::vector<process>& incoming_list, int& incoming_index) = 0;

    [[nodiscard]] std::vector<event> getTimeline() const {
        return timeline;
    }
};

#endif //BACKEND_SCHEDULER_H