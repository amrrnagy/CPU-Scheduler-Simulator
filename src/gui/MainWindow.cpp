#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <algorithm>

#include "../core/simulator.h"
#include "../core/models/process.h"
#include "../core/models/event.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/app_icon.ico"));

    nextPid             = 1;
    currentTime         = 0;
    isSimulationRunning = false;
    activeSimulator     = nullptr;

    auto* scene = new QGraphicsScene(this);
    ui->view_Gantt->setScene(scene);
    ui->view_Gantt->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ui->label_Quantum->setVisible(false);
    ui->input_Quantum->setVisible(false);
    ui->label_Priority->setVisible(false);
    ui->input_Priority->setVisible(false);
}

MainWindow::~MainWindow() {
    if (activeSimulator) { activeSimulator->stop(); delete activeSimulator; }
    delete ui;
}

void MainWindow::on_combo_Scheduler_currentIndexChanged(int) const {
    QString alg = ui->combo_Scheduler->currentText();
    ui->label_Quantum->setVisible(false);  ui->input_Quantum->setVisible(false);
    ui->label_Priority->setVisible(false); ui->input_Priority->setVisible(false);
    if (alg == "Round Robin")       { ui->label_Quantum->setVisible(true);  ui->input_Quantum->setVisible(true); }
    else if (alg.contains("Priority")) { ui->label_Priority->setVisible(true); ui->input_Priority->setVisible(true); }
}

void MainWindow::on_btn_Add_clicked() {
    if (isSimulationRunning && ui->radio_Static->isChecked()) {
        QMessageBox::warning(this, "Warning", "Cannot add processes in static mode while running!");
        return;
    }
    Process p;
    p.pid           = nextPid;
    p.burstTime     = ui->input_Burst->value();
    p.remainingTime = p.burstTime;
    p.priority      = ui->input_Priority->isVisible() ? ui->input_Priority->value() : 0;
    p.arrivalTime   = (isSimulationRunning && ui->radio_Dynamic->isChecked())
                      ? currentTime : ui->input_Arrival->value();

    processList.push_back(p);
    nextPid++;

    if (isSimulationRunning && ui->radio_Dynamic->isChecked() && activeSimulator)
        activeSimulator->addProcess(::process(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime));

    ui->input_Arrival->setValue(0);
    ui->input_Burst->setValue(1);
    ui->input_Priority->setValue(0);
    updateProcessTable();
}

void MainWindow::on_btn_Reset_clicked() {
    if (activeSimulator) { activeSimulator->stop(); delete activeSimulator; activeSimulator = nullptr; }
    isSimulationRunning = false;
    processList.clear();
    nextPid = 1; currentTime = 0;
    ui->btn_Add->setEnabled(true);
    ui->input_Arrival->setVisible(true); ui->label_Arrival->setVisible(true);
    ui->groupBox_Scheduler->setEnabled(true); ui->groupBox_Mode->setEnabled(true);
    ui->btn_RunLive->setVisible(true);
    ui->btn_RunInstant->setVisible(!ui->radio_Dynamic->isChecked());
    ui->label_Time->setText("Current Time: 0s");
    ui->label_Wait->setText("Avg Waiting Time: 0.00");
    ui->label_Turnaround->setText("Avg Turnaround Time: 0.00");
    ui->table_Processes->setRowCount(0);
    ui->view_Gantt->scene()->clear();
}

// ── Helpers ───────────────────────────────────────────────────────────────

void MainWindow::updateProcessTable() const {
    ui->table_Processes->setRowCount(static_cast<int>(processList.size()));
    for (int i = 0; i < static_cast<int>(processList.size()); ++i) {
        const Process& p = processList[i];
        auto* pidItem   = new QTableWidgetItem("P" + QString::number(p.pid));
        auto* arrItem   = new QTableWidgetItem(QString::number(p.arrivalTime));
        auto* burstItem = new QTableWidgetItem(QString::number(p.burstTime));
        auto* prioItem  = new QTableWidgetItem(ui->input_Priority->isVisible() ? QString::number(p.priority) : "-");
        auto* remItem   = new QTableWidgetItem(QString::number(p.remainingTime));
        for (auto* item : {pidItem, arrItem, burstItem, prioItem, remItem})
            item->setTextAlignment(Qt::AlignCenter);
        ui->table_Processes->setItem(i, 0, pidItem);
        ui->table_Processes->setItem(i, 1, arrItem);
        ui->table_Processes->setItem(i, 2, burstItem);
        ui->table_Processes->setItem(i, 3, prioItem);
        ui->table_Processes->setItem(i, 4, remItem);
    }
}

// Update only the remaining-time column from the backend snapshot.
// Avoids recreating all table items on every tick (which causes flicker).
void MainWindow::updateRemainingTimesFromSnapshot(const SimSnapshot& snap) const {
    // Build a pid → remaining map for O(1) lookup
    std::unordered_map<int,int> remMap;
    remMap.reserve(snap.remainingTimes.size());
    for (const auto& [pid, rem] : snap.remainingTimes)
        remMap[pid] = rem;

    for (int i = 0; i < static_cast<int>(processList.size()); ++i) {
        auto it = remMap.find(processList[i].pid);
        if (it != remMap.end()) {
            auto* item = ui->table_Processes->item(i, 4);
            if (!item) {
                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                ui->table_Processes->setItem(i, 4, item);
            }
            item->setText(QString::number(std::max(0, it->second)));
        }
    }
}

static SchedulerType algorithmToType(const QString& algorithm) {
    if (algorithm == "Round Robin")               return SchedulerType::RoundRobin;
    if (algorithm == "SJF (Preemptive)")          return SchedulerType::SJF_Preemptive;
    if (algorithm == "SJF (Non-Preemptive)")      return SchedulerType::SJF_NonPreemptive;
    if (algorithm == "Priority (Preemptive)")     return SchedulerType::Priority_Preemptive;
    if (algorithm == "Priority (Non-Preemptive)") return SchedulerType::Priority_NonPreemptive;
    return SchedulerType::FCFS;
}

void MainWindow::drawGanttFromSnapshot(const SimSnapshot& snap) const {
    ui->view_Gantt->scene()->clear();
    QGraphicsScene* scene = ui->view_Gantt->scene();
    constexpr int blockH = 50;

    if (snap.currentTime == 0) return;

    // Build segments from the completed timeline events.
    struct Segment { int pid, start, end; };
    std::vector<Segment> rawSegments;

    auto timeline = snap.timeline;
    std::sort(timeline.begin(), timeline.end(),
              [](const ::event& a, const ::event& b){ return a.getStartTime() < b.getStartTime(); });

    int trackTime = 0;
    for (const ::event& e : timeline) {
        if (e.getStartTime() >= snap.currentTime) break;
        if (trackTime < e.getStartTime())
            rawSegments.push_back({-1, trackTime, e.getStartTime()});
        int endT = std::min(e.getEndTime(), snap.currentTime);
        if (endT > trackTime) {
            rawSegments.push_back({e.getProcessId(), e.getStartTime(), endT});
            trackTime = endT;
        }
    }

    // The trailing segment uses snap.currentProcessId
    if (trackTime < snap.currentTime)
        rawSegments.push_back({snap.currentProcessId, trackTime, snap.currentTime});

    // Merge consecutive same-pid segments for clean display.
    std::vector<Segment> merged;
    for (const auto& seg : rawSegments) {
        if (!merged.empty() && merged.back().pid == seg.pid && merged.back().end == seg.start)
            merged.back().end = seg.end;
        else
            merged.push_back(seg);
    }

    // Draw.
    int drawX = 0;
    for (const auto& m : merged) {
        constexpr int blockW = 30;
        int rectWidth = (m.end - m.start) * blockW;
        QColor color = (m.pid == -1) ? Qt::lightGray : QColor::fromHsv((m.pid * 50) % 360, 200, 200);
        scene->addRect(drawX, 0, rectWidth, blockH, QPen(Qt::black), QBrush(color));

        QString label = (m.pid == -1) ? "IDLE" : ("P" + QString::number(m.pid));
        QGraphicsTextItem* text = scene->addText(label);
        text->setPos(drawX + (rectWidth - static_cast<int>(text->boundingRect().width())) / 2, 15);

        QGraphicsTextItem* tStart = scene->addText(QString::number(m.start));
        tStart->setPos(drawX - 5, blockH + 2);
        drawX += rectWidth;
    }
    if (!merged.empty()) {
        QGraphicsTextItem* tEnd = scene->addText(QString::number(snap.currentTime));
        tEnd->setPos(drawX - 5, blockH + 2);
    }
}

// ── Live Mode ─────────────────────────────────────────────────────────────

void MainWindow::on_btn_RunLive_clicked() {
    if (processList.empty()) { QMessageBox::warning(this,"Warning","Please add processes first!"); return; }

    isSimulationRunning = true;
    if (ui->radio_Static->isChecked()) ui->btn_Add->setEnabled(false);
    ui->input_Arrival->setVisible(false); ui->label_Arrival->setVisible(false);
    ui->groupBox_Scheduler->setEnabled(false); ui->groupBox_Mode->setEnabled(false);
    ui->btn_RunLive->setVisible(false);   ui->btn_RunInstant->setVisible(false);

    std::deque<process> backendList;
    for (const auto& p : processList)
        backendList.emplace_back(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime);
    std::sort(backendList.begin(), backendList.end(),
              [](const process& a, const process& b){ return a.getArrivalTime() < b.getArrivalTime(); });

    currentTime = 0;
    ui->view_Gantt->scene()->clear();

    if (activeSimulator) { activeSimulator->stop(); delete activeSimulator; activeSimulator = nullptr; }

    activeSimulator = new simulator(algorithmToType(ui->combo_Scheduler->currentText()),
                                    ui->input_Quantum->value());
    activeSimulator->loadProcesses(backendList);

    activeSimulator->on_tick_callback = [this]() {
        QMetaObject::invokeMethod(this, "onLiveTimerTick", Qt::QueuedConnection);
    };
    activeSimulator->start();
}

void MainWindow::onLiveTimerTick() {
    if (!activeSimulator) return;

    // FIX (Bugs 1, 2, 6): Single lock-guarded snapshot — all data is consistent.
    SimSnapshot snap = activeSimulator->getSnapshot();

    currentTime = snap.currentTime;
    drawGanttFromSnapshot(snap);   // Gantt: no more IDLE flicker

    ui->label_Time->setText("Current Time: " + QString::number(currentTime) + "s");
    ui->label_Wait->setText("Avg Waiting Time: " + QString::number(snap.avgWaitingTime, 'f', 2));
    ui->label_Turnaround->setText("Avg Turnaround Time: " + QString::number(snap.avgTurnaroundTime, 'f', 2));

    // FIX (Bug 4): Update remaining burst times live in the table.
    updateRemainingTimesFromSnapshot(snap);

    if (!activeSimulator->isRunning()) {
        isSimulationRunning = false;
        ui->btn_Add->setEnabled(true);
        ui->input_Arrival->setVisible(true); ui->label_Arrival->setVisible(true);
        ui->groupBox_Scheduler->setEnabled(true); ui->groupBox_Mode->setEnabled(true);
        ui->btn_RunLive->setVisible(true);
        ui->btn_RunInstant->setVisible(true);
        QMessageBox::information(this, "Finished", "Live Simulation Complete!");
    }
}

// ── Instant (Batch) Mode ──────────────────────────────────────────────────

void MainWindow::on_btn_RunInstant_clicked() {
    if (processList.empty()) { QMessageBox::warning(this,"Warning","Please add processes first!"); return; }

    if (isSimulationRunning && activeSimulator) {
        activeSimulator->stop();
        isSimulationRunning = false;
        ui->btn_Add->setEnabled(true);
    }

    std::deque<process> backendList;
    for (const auto& p : processList)
        backendList.emplace_back(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime);
    std::sort(backendList.begin(), backendList.end(),
              [](const process& a, const process& b){ return a.getArrivalTime() < b.getArrivalTime(); });

    if (activeSimulator) { delete activeSimulator; activeSimulator = nullptr; }

    activeSimulator = new simulator(algorithmToType(ui->combo_Scheduler->currentText()),
                                    ui->input_Quantum->value());
    activeSimulator->loadProcesses(backendList);
    activeSimulator->runBatch();

    // Batch mode: thread is stopped — direct reads are safe (no races).
    auto timeline = activeSimulator->getTimelineUnsafe();
    int maxTime = 0;
    for (const ::event& e : timeline)
        if (e.getEndTime() > maxTime) maxTime = e.getEndTime();
    currentTime = maxTime;

    // Build and draw using a manual snapshot (thread stopped, safe).
    SimSnapshot snap;
    snap.timeline          = timeline;
    snap.currentProcessId  = -1;
    snap.currentTime       = currentTime;
    drawGanttFromSnapshot(snap);

    ui->label_Time->setText("Total Time: " + QString::number(currentTime) + "s");
    ui->label_Wait->setText("Avg Waiting Time: " + QString::number(activeSimulator->getAvgWaitingTimeUnsafe(), 'f', 2));
    ui->label_Turnaround->setText("Avg Turnaround Time: " + QString::number(activeSimulator->getAvgTurnaroundTimeUnsafe(), 'f', 2));

    // Show final remaining times (all 0 after batch run).
    SimSnapshot finalSnap;
    finalSnap.remainingTimes.reserve(processList.size());
    for (const auto& p : processList) finalSnap.remainingTimes.emplace_back(p.pid, 0);
    updateRemainingTimesFromSnapshot(finalSnap);

    QMessageBox::information(this, "Finished", "Simulation Complete!");
}

void MainWindow::on_radio_Dynamic_toggled(const bool checked) const {
    ui->btn_RunInstant->setVisible(!checked);
}
