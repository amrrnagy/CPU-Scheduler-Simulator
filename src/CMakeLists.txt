cmake_minimum_required(VERSION 3.16)
project(CPUSchedulerSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(
        Scheduler

        app.cpp

        core/models/process.h
        core/models/event.h
        core/schedulers/scheduler.h
        core/schedulers/FCFS.h
        core/schedulers/FCFS.cpp
        core/schedulers/SJF_prm.cpp
        core/schedulers/SJF_prm.h
        core/schedulers/priority.h
        core/schedulers/priority.cpp
        core/schedulers/RRS.cpp
        core/schedulers/RRS.h
        core/schedulers/SJF.cpp
        core/schedulers/SJF.h
        core/schedulers/priority_non_prm.cpp
        core/schedulers/priority_non_prm.h
        core/simulator.cpp
        core/simulator.h
)