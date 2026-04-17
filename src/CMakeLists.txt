cmake_minimum_required(VERSION 3.16)
project(CPUSchedulerSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "D:/Programs/Qt/6.11.0/mingw_64")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(
        Scheduler
        app.cpp

        # GUI files
        gui/MainWindow.cpp
        gui/MainWindow.h
        gui/MainWindow.ui

        # Backend files
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

        app_icon.ico
        resources.qrc
)

# LINK THE LIBRARIES
target_link_libraries(Scheduler PRIVATE Qt6::Widgets)