# CPU Scheduler Simulator

A live, interactive CPU scheduling simulator built with **C++** and **Qt 6**. This desktop application visualizes how different scheduling algorithms manage process execution over time using dynamic Gantt charts and live metric tracking.

## ✨ Supported Algorithms
- **FCFS** (First-Come, First-Served)
- **SJF** (Shortest Job First) — *Preemptive & Non-Preemptive*
- **Priority** — *Preemptive & Non-Preemptive* (Lower number = Higher priority)
- **Round Robin**

## 🚀 Features
- **Live Execution Mode:** 1 unit of CPU time is mapped to 1 real-world second.
- **Static Execution Mode:** Instantly calculates and displays results for pre-existing processes.
- **Dynamic Insertion:** Add new processes on the fly while the live scheduler is running.
- **Live Gantt Chart:** Visualizes the timeline and order of execution in real-time.
- **Real-Time Metrics:** Live updates of the Remaining Burst Time table, Average Waiting Time, and Average Turnaround Time.


## 🛠️ How to Build from Source
This project uses CMake and requires Qt 6.
