#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <algorithm>

#include "../core/simulator.h"
#include "../core/models/process.h"
#include "../core/models/event.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    nextPid             = 1;
    currentTime         = 0;
    isSimulationRunning = false;
    activeSimulator     = nullptr;
    ganttX              = 0;

    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->view_Gantt->setScene(scene);
    ui->view_Gantt->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ui->label_Quantum->setVisible(false);
    ui->input_Quantum->setVisible(false);
    ui->label_Priority->setVisible(false);
    ui->input_Priority->setVisible(false);
}

MainWindow::~MainWindow()
{
    if (activeSimulator) {
        activeSimulator->stop();
        delete activeSimulator;
    }
    delete ui;
}

// ── UI Event Handlers ────────────────────────────────────────────────────

void MainWindow::on_combo_Scheduler_currentIndexChanged(int /*index*/)
{
    QString algorithm = ui->combo_Scheduler->currentText();

    ui->label_Quantum->setVisible(false);
    ui->input_Quantum->setVisible(false);
    ui->label_Priority->setVisible(false);
    ui->input_Priority->setVisible(false);

    if (algorithm == "Round Robin") {
        ui->label_Quantum->setVisible(true);
        ui->input_Quantum->setVisible(true);
    } else if (algorithm.contains("Priority")) {
        ui->label_Priority->setVisible(true);
        ui->input_Priority->setVisible(true);
    }
}

void MainWindow::on_btn_Add_clicked()
{
    if (isSimulationRunning) {
        QMessageBox::warning(this, "Warning", "Cannot add processes while simulation is running live!");
        return;
    }

    Process p;
    p.pid           = nextPid;
    p.arrivalTime   = ui->input_Arrival->value();
    p.burstTime     = ui->input_Burst->value();
    p.remainingTime = p.burstTime;
    p.priority      = ui->input_Priority->isVisible() ? ui->input_Priority->value() : 0;

    processList.push_back(p);
    nextPid++;

    ui->input_Arrival->setValue(0);
    ui->input_Burst->setValue(1);
    ui->input_Priority->setValue(0);

    updateProcessTable();
}

void MainWindow::on_btn_Reset_clicked()
{
    if (activeSimulator) {
        activeSimulator->stop();
        delete activeSimulator;
        activeSimulator = nullptr;
    }

    isSimulationRunning = false;
    processList.clear();
    nextPid     = 1;
    currentTime = 0;
    ganttX      = 0;

    ui->btn_Add->setEnabled(true);
    ui->label_Time->setText("Current Time: 0s");
    ui->label_Wait->setText("Avg Waiting Time: 0.00");
    ui->label_Turnaround->setText("Avg Turnaround Time: 0.00");

    ui->table_Processes->setRowCount(0);
    ui->view_Gantt->scene()->clear();
}

// ── Helpers ───────────────────────────────────────────────────────────────

void MainWindow::updateProcessTable()
{
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

static SchedulerType algorithmToType(const QString& algorithm) {
    if (algorithm == "Round Robin")              return SchedulerType::RoundRobin;
    if (algorithm == "SJF (Preemptive)")         return SchedulerType::SJF_Preemptive;
    if (algorithm == "SJF (Non-Preemptive)")     return SchedulerType::SJF_NonPreemptive;
    if (algorithm == "Priority (Preemptive)")    return SchedulerType::Priority_Preemptive;
    if (algorithm == "Priority (Non-Preemptive)")return SchedulerType::Priority_NonPreemptive;
    return SchedulerType::FCFS;
}

// Draw a single Gantt block at the current ganttX position.
static void addGanttBlock(QGraphicsScene* scene, int x, int pid, int blockW = 30, int blockH = 50) {
    QColor color = (pid == -1) ? Qt::lightGray : QColor::fromHsv((pid * 50) % 360, 200, 200);
    scene->addRect(x, 0, blockW, blockH, QPen(Qt::black), QBrush(color));
    QString label = (pid == -1) ? "IDLE" : ("P" + QString::number(pid));
    QGraphicsTextItem* text = scene->addText(label);
    text->setPos(x + (pid == -1 ? 2 : 5), 15);
}

// ── Live Mode ─────────────────────────────────────────────────────────────

void MainWindow::on_btn_RunLive_clicked()
{
    if (processList.empty()) {
        QMessageBox::warning(this, "Warning", "Please add processes before running!");
        return;
    }

    isSimulationRunning = true;
    ui->btn_Add->setEnabled(false);

    std::vector<process> backendList;
    for (const auto& p : processList)
        backendList.emplace_back(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime);

    std::sort(backendList.begin(), backendList.end(),
              [](const process& a, const process& b){ return a.getArrivalTime() < b.getArrivalTime(); });

    ganttX      = 0;
    currentTime = 0;
    ui->view_Gantt->scene()->clear();

    if (activeSimulator) { activeSimulator->stop(); delete activeSimulator; activeSimulator = nullptr; }

    const QString algorithm = ui->combo_Scheduler->currentText();
    const int     quantum   = ui->input_Quantum->value();

    activeSimulator = new simulator(algorithmToType(algorithm), quantum);
    activeSimulator->loadProcesses(backendList);

    // FIX (Bug 2): on_tick_callback is now public — direct assignment works.
    activeSimulator->on_tick_callback = [this]() {
        QMetaObject::invokeMethod(this, "onLiveTimerTick", Qt::QueuedConnection);
    };

    activeSimulator->start();
}

void MainWindow::onLiveTimerTick()
{
    if (!activeSimulator) return;

    QGraphicsScene* scene = ui->view_Gantt->scene();

    // Draw who was running during the tick that just completed.
    int runningId = activeSimulator->getCurrentProcessId();
    addGanttBlock(scene, ganttX, runningId);
    ganttX      += 30;
    currentTime++;

    ui->label_Time->setText("Current Time: " + QString::number(currentTime) + "s");

    // FIX (Bug 8): Update avg time labels on every tick so they're always current.
    ui->label_Wait->setText(
        "Avg Waiting Time: " + QString::number(activeSimulator->getAvgWaitingTime(), 'f', 2));
    ui->label_Turnaround->setText(
        "Avg Turnaround Time: " + QString::number(activeSimulator->getAvgTurnaroundTime(), 'f', 2));

    // FIX (Bug 7): was checking e.type == "FINISHED" which doesn't exist on event.
    // Just check isRunning() — the background thread sets it to false when done.
    if (!activeSimulator->isRunning()) {
        isSimulationRunning = false;
        ui->btn_Add->setEnabled(true);
        QMessageBox::information(this, "Finished", "Live Simulation Complete!");
    }
}

// ── Instant (Batch) Mode ──────────────────────────────────────────────────

void MainWindow::on_btn_RunInstant_clicked()
{
    if (processList.empty()) {
        QMessageBox::warning(this, "Warning", "Please add processes before running!");
        return;
    }

    if (isSimulationRunning && activeSimulator) {
        activeSimulator->stop();
        isSimulationRunning = false;
        ui->btn_Add->setEnabled(true);
    }

    std::vector<process> backendList;
    for (const auto& p : processList)
        backendList.emplace_back(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime);

    std::sort(backendList.begin(), backendList.end(),
              [](const process& a, const process& b){ return a.getArrivalTime() < b.getArrivalTime(); });

    if (activeSimulator) { delete activeSimulator; activeSimulator = nullptr; }

    const QString algorithm = ui->combo_Scheduler->currentText();
    const int     quantum   = ui->input_Quantum->value();

    activeSimulator = new simulator(algorithmToType(algorithm), quantum);
    activeSimulator->loadProcesses(backendList);
    activeSimulator->runBatch();

    // ── Draw Gantt chart ──────────────────────────────────────────────────
    // FIX (Bug 5 & 6): The old code used e.time / e.type / e.process_id which
    // don't exist on the event class. The event class has getStartTime(),
    // getEndTime(), getProcessId(). We draw one block per timeline segment
    // directly — no need to reconstruct second-by-second from type strings.

    ui->view_Gantt->scene()->clear();
    QGraphicsScene* scene = ui->view_Gantt->scene();
    const int blockW = 30;
    const int blockH = 50;

    const auto timeline = activeSimulator->getTimeline();

    // Sort timeline events by start time so the chart is left-to-right.
    std::vector<::event> sorted = timeline;
    std::sort(sorted.begin(), sorted.end(),
              [](const ::event& a, const ::event& b){ return a.getStartTime() < b.getStartTime(); });

    // Fill idle gaps and draw each event block.
    int drawX   = 0;
    int prevEnd = 0;

    for (const ::event& e : sorted) {
        // Draw idle gap if there's a hole before this block.
        int idleStart = prevEnd;
        while (idleStart < e.getStartTime()) {
            addGanttBlock(scene, drawX, -1, blockW, blockH);
            drawX++;          // 1 unit = 1 block-width worth of X
            idleStart++;
        }
        // Draw the process block (one visual block per time unit it ran).
        for (int t = e.getStartTime(); t < e.getEndTime(); ++t) {
            addGanttBlock(scene, drawX * blockW, e.getProcessId(), blockW, blockH);
            drawX++;
        }
        prevEnd = e.getEndTime();
    }

    // Draw time labels along the bottom
    int totalTime = prevEnd;
    for (int t = 0; t <= totalTime; ++t) {
        QGraphicsTextItem* label = scene->addText(QString::number(t));
        label->setPos(t * blockW - 4, blockH + 2);
    }

    currentTime = totalTime;
    ui->label_Time->setText("Total Time: " + QString::number(currentTime) + "s");

    // FIX (Bug 8): Update avg time labels — they were never set after instant run.
    ui->label_Wait->setText(
        "Avg Waiting Time: " + QString::number(activeSimulator->getAvgWaitingTime(), 'f', 2));
    ui->label_Turnaround->setText(
        "Avg Turnaround Time: " + QString::number(activeSimulator->getAvgTurnaroundTime(), 'f', 2));

    QMessageBox::information(this, "Finished", "Simulation Complete!");
}
