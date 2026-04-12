cmake_minimum_required(VERSION 3.16)
project(CPUSchedulerSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(
        Scheduler

        core/main.cpp

        core/process.h
        core/event.h
        core/scheduler.h
        core/FCFS.h
        core/FCFS.cpp
        core/SJF_prm.cpp
        core/SJF_prm.h
        core/priority.h
        core/priority.cpp
        core/RRS.cpp
        core/RRS.h
        core/SJF.cpp
        core/SJF.h
        core/priority_non_prm.cpp
        core/priority_non_prm.h
)