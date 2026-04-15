#include "simulator.h"

simulator::simulator(SchedulerType type, int rr_quantum) {
    switch (type) {
        case SchedulerType::FCFS:
            sched = std::make_unique<FCFS>();
            break;
        case SchedulerType::SJF_NonPreemptive:
            sched = std::make_unique<SJF>();
            break;
        case SchedulerType::SJF_Preemptive:
            sched = std::make_unique<SRTF>();
            break;
        case SchedulerType::Priority_NonPreemptive:
            sched = std::make_unique<Priority_Non_Prm>();
            break;
        case SchedulerType::Priority_Preemptive:
            sched = std::make_unique<Priority_Prm>();
            break;
        case SchedulerType::RoundRobin:
            sched = std::make_unique<RR>(rr_quantum);
            break;
    }
}

simulator::~simulator() {
    stop();
}

void simulator::loadProcesses(const std::vector<process>& list) {
    sched->loadProcesses(list);
}

void simulator::addProcess(const process& p) {
    // Thread-safe: guards access to the scheduler's incoming queue.
    std::lock_guard<std::mutex> lock(process_mutex);
    sched->addIncomingProcess(p);
}

void simulator::start() {
    if (running.load()) return;
    running = true;
    paused  = false;

    sim_thread = std::thread([this]() {
        while (running.load()) {
            if (!paused.load()) {
                {
                    std::lock_guard<std::mutex> lock(process_mutex);
                    bool still_running = sched->tick();
                    if (!still_running) {
                        running = false;
                    }
                }
                if (on_tick_callback) on_tick_callback();
            }
            // 1 unit of time = 1 second as per project spec.
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void simulator::stop() {
    running = false;
    if (sim_thread.joinable())
        sim_thread.join();
}

void simulator::pause()  { paused = true;  }
void simulator::resume() { paused = false; }

void simulator::runBatch() {
    // Batch mode: run to completion instantly.
    sched->run();
    if (on_tick_callback) on_tick_callback();
}