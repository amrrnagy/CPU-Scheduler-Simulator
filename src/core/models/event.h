#ifndef BACKEND_EVENT_H
#define BACKEND_EVENT_H

class event {
    int processId;
    int startTime;
    int endTime;

public:
    event(const int id, const int start, const int end)
        : processId(id), startTime(start), endTime(end) {}

    // FIX (Bug 9): Added public getters — without these the GUI
    // can never read timeline data to draw the Gantt chart.
    int getProcessId() const { return processId; }
    int getStartTime()  const { return startTime;  }
    int getEndTime()    const { return endTime;    }
};

#endif //BACKEND_EVENT_H