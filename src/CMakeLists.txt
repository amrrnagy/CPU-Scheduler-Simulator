cmake_minimum_required(VERSION 3.16)
project(CPUSchedulerSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:\\Qt\\6.11.0\\mingw_64")
add_executable(
        Scheduler

        app.cpp

        core/models/process.h
        core/models/event.h
        core/schedulers/scheduler.h
        core/schedulers/FCFS.h
        core/schedulers/FCFS.cpp
        core/schedulers/priority.h
        core/schedulers/priority.cpp
        core/schedulers/RR.cpp
        core/schedulers/RR.h
        core/schedulers/SJF.cpp
        core/schedulers/SJF.h
        core/simulator.cpp
        core/simulator.h
)