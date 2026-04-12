//
// Created by Lenovo on 4/9/2026.
//

#ifndef BACKEND_EVENT_H
#define BACKEND_EVENT_H
class event {
    int processId;
    int startTime;
    int endTime;
public:
    event(int id, int start, int end) : processId(id), startTime(start), endTime(end) {}
    int getId() const { return processId; }
    int getstartTime() const { return startTime; }
    int getendTime() const { return endTime; }

};

#endif //BACKEND_EVENT_H