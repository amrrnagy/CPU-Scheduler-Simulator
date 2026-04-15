#ifndef BACKEND_PROCESS_H
#define BACKEND_PROCESS_H

class process {
    int id;
    int arrivalTime;
    int burstTime;
    int priority;
    int remainingTime;

public:
    process (const int ID, const int arr, const int burst, const int priority, int rem)
        :id(ID) , arrivalTime(arr) , burstTime(burst) , priority(priority) , remainingTime(rem) {}

        bool operator<(const process & other) const {
            return this->arrivalTime > other.arrivalTime;
        }

    int getId() const { return id; }
    int getArrivalTime() const { return arrivalTime; }
    int getBurstTime() const { return burstTime; }
    int getPriority() const { return priority; }
    int getRemainingTime() const { return remainingTime; }
    void setRemainingTime(int t) { remainingTime = t; }
};














#endif //BACKEND_PROCESS_H