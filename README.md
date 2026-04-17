# CPU Scheduler Simulator

A live, interactive CPU scheduling simulator built with **C++** and **Qt 6**. This desktop application visualizes how different scheduling algorithms manage process execution over time using dynamic Gantt charts and live metric tracking.

## ✨ Supported Algorithms
- **FCFS** (First-Come, First-Served)
- **SJF** (Shortest Job First) — *Preemptive & Non-Preemptive*
- **Priority** — *Preemptive & Non-Preemptive* (Lower number = Higher priority)
- **Round Robin**

## 🚀 Features
- **Live Execution Mode:** 1 unit of CPU time is mapped to 1 real-world second.
- **Instant Execution Mode:** Instantly calculates and displays the entire timeline and metrics for batch processing.
- **Dynamic Live Insertion:** Add new processes on the fly while the live scheduler is actively running.
- **Optimized Gantt Chart:** Visualizes the timeline and order of execution, dynamically merging consecutive execution blocks for clean readability.
- **Real-Time Metrics:** Live updates of the Remaining Burst Time table, Average Waiting Time, and Average Turnaround Time.

## 📥 Installation (Windows)
You do not need to compile the code to use the simulator.
1. Go to the **[Releases](../../releases)** page on the right side of this repository.
2. Download the latest `setup.exe` file.
3. Run the installer to easily install the application and create a desktop shortcut.
   *(Note: If Windows SmartScreen blocks the application, click "More Info" and then "Run Anyway").*

## 🛠️ How to Build from Source
If you wish to modify the code, this project uses CMake and requires Qt 6.

### Prerequisites
- **C++ Compiler:** MinGW or MSVC (Windows), GCC/Clang (Linux/macOS)
- **Qt 6:** Ensure the Qt 6 framework is installed and your system path is configured correctly.
- **CMake:** Version 3.16 or higher.

### Build Instructions (using CLion or Qt Creator)
1. Clone this repository:
   `git clone https://github.com/amrrnagy/CPU-Scheduler-Simulator.git`
2. Open the project folder in your IDE.
3. Ensure your build profile is set to **Release** (to avoid performance overhead from debug assertions).
4. Build and Run the `CPU_Scheduler_Simulator` executable target.

## Future Enhancements
To further improve this project, the following features can be implemented:
- **Additional Scheduling Algorithms**:
  - Multilevel Queue Scheduling.
  - Multilevel Feedback Queue Scheduling.
- **Save/Load Functionality**:
  - Allow users to save and load process configurations for later use.
- **Improved GUI**:
  - Add more interactive features, such as tooltips and animations.
  - Enhance the visualization of the Gantt Chart and performance metrics.
