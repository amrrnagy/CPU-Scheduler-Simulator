#ifndef BACKEND_PROCESS_H
#define BACKEND_PROCESS_H

class process {
    int id;
    int arrivalTime;
    int burstTime;
    int priority;
    int remainingTime;

public:
    process(const int ID, const int arr, const int burst, const int prio, const int rem)
        : id(ID), arrivalTime(arr), burstTime(burst), priority(prio), remainingTime(rem) {}

    [[nodiscard]] int getId()            const { return id;            }
    [[nodiscard]] int getArrivalTime()   const { return arrivalTime;   }
    [[nodiscard]] int getBurstTime()     const { return burstTime;     }
    [[nodiscard]] int getPriority()      const { return priority;      }
    [[nodiscard]] int getRemainingTime() const { return remainingTime; }

    void setRemainingTime(const int t) { remainingTime = t; }
};

#endif //BACKEND_PROCESS_H