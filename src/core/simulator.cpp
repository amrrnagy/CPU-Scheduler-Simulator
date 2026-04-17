#include "simulator.h"

simulator::simulator(SchedulerType type, int rr_quantum) {
    switch (type) {
        case SchedulerType::FCFS:                 sched = std::make_unique<FCFS>();              break;
        case SchedulerType::SJF_NonPreemptive:    sched = std::make_unique<SJF>();               break;
        case SchedulerType::SJF_Preemptive:       sched = std::make_unique<SRTF>();              break;
        case SchedulerType::Priority_NonPreemptive:sched = std::make_unique<Priority_Non_Prm>(); break;
        case SchedulerType::Priority_Preemptive:  sched = std::make_unique<Priority_Prm>();      break;
        case SchedulerType::RoundRobin:           sched = std::make_unique<RR>(rr_quantum);      break;
    }
}

simulator::~simulator() { stop(); }

void simulator::loadProcesses(const std::deque<process>& list) const {
    sched->loadProcesses(list);
}

void simulator::addProcess(const process& p) {
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
                    if (!still_running) running = false;
                }
                if (on_tick_callback) on_tick_callback();
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void simulator::stop() {
    running = false;
    if (sim_thread.joinable()) sim_thread.join();
}

void simulator::pause()  { paused = true;  }
void simulator::resume() { paused = false; }

void simulator::runBatch() const {
    sched->run();
    if (on_tick_callback) on_tick_callback();
}

SimSnapshot simulator::getSnapshot() {
    std::lock_guard<std::mutex> lock(process_mutex);
    SimSnapshot snap;
    if (!sched) return snap;

    snap.timeline          = sched->getTimeline();        // safe: lock held
    snap.currentProcessId  = sched->getCurrentProcessId();// safe: lock held
    snap.avgWaitingTime    = sched->getAvgWaitingTime();
    snap.avgTurnaroundTime = sched->getAvgTurnaroundTime();
    snap.currentTime       = sched->getCurrentTime();
    snap.remainingTimes    = sched->getRemainingTimes();  // for table update
    return snap;
}
