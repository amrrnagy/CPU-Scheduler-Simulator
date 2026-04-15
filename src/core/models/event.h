#ifndef BACKEND_EVENT_H
#define BACKEND_EVENT_H

class event {
    int processId;
    int startTime;
    int endTime;

public:
    event(const int id, const int start, const int end)
        : processId(id), startTime(start), endTime(end) {}

    [[nodiscard]] int getProcessId() const { return processId; }
    [[nodiscard]] int getStartTime()  const { return startTime;  }
    [[nodiscard]] int getEndTime()    const { return endTime;    }
};

#endif //BACKEND_EVENT_H