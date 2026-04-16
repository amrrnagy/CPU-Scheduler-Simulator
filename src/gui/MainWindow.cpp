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
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    nextPid             = 1;
    currentTime         = 0;
    isSimulationRunning = false;
    activeSimulator     = nullptr;

    auto *scene = new QGraphicsScene(this);
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

void MainWindow::on_combo_Scheduler_currentIndexChanged(int /*index*/) const {
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
    if (isSimulationRunning && ui->radio_Static->isChecked()) {
        QMessageBox::warning(this, "Warning", "Cannot add processes while simulation is running live!");
        return;
    }

    Process p;
    p.pid           = nextPid;
    p.burstTime     = ui->input_Burst->value();
    p.remainingTime = p.burstTime;
    p.priority      = ui->input_Priority->isVisible() ? ui->input_Priority->value() : 0;

    // Sync arrival time with the live clock if running dynamically
    if (isSimulationRunning && ui->radio_Dynamic->isChecked()) {
        p.arrivalTime = currentTime;
    } else {
        p.arrivalTime = ui->input_Arrival->value();
    }

    processList.push_back(p);
    nextPid++;

    if (isSimulationRunning && ui->radio_Dynamic->isChecked() && activeSimulator) {
        activeSimulator->addProcess(
            ::process(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime));
    }

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

    ui->btn_Add->setEnabled(true);

    // Restore Arrival Time inputs!
    ui->input_Arrival->setVisible(true);
    ui->label_Arrival->setVisible(true);

    ui->groupBox_Scheduler->setEnabled(true);
    ui->groupBox_Mode->setEnabled(true);
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

static SchedulerType algorithmToType(const QString& algorithm) {
    if (algorithm == "Round Robin")              return SchedulerType::RoundRobin;
    if (algorithm == "SJF (Preemptive)")         return SchedulerType::SJF_Preemptive;
    if (algorithm == "SJF (Non-Preemptive)")     return SchedulerType::SJF_NonPreemptive;
    if (algorithm == "Priority (Preemptive)")    return SchedulerType::Priority_Preemptive;
    if (algorithm == "Priority (Non-Preemptive)")return SchedulerType::Priority_NonPreemptive;
    return SchedulerType::FCFS;
}

// THE NEW MASTER RENDERING FUNCTION
void MainWindow::drawGanttChart() const {
    ui->view_Gantt->scene()->clear();
    QGraphicsScene* scene = ui->view_Gantt->scene();
    constexpr int blockH = 50;

    if (!activeSimulator || currentTime == 0) return;

    // 1. Extract historical timeline
    auto timeline = activeSimulator->getTimeline();
    std::sort(timeline.begin(), timeline.end(),
              [](const ::event& a, const ::event& b){ return a.getStartTime() < b.getStartTime(); });

    // 2. Build mathematical segments mapping out exactly who ran when
    struct Segment { int pid, start, end; };
    std::vector<Segment> rawSegments;
    int trackTime = 0;

    for (const ::event& e : timeline) {
        if (e.getStartTime() >= currentTime) break;

        // Gap detected
        if (trackTime < e.getStartTime()) {
            rawSegments.push_back({-1, trackTime, e.getStartTime()});
            trackTime = e.getStartTime();
        }

        // Process detected
        int endT = std::min(e.getEndTime(), currentTime);
        if (endT > trackTime) {
            rawSegments.push_back({e.getProcessId(), trackTime, endT});
            trackTime = endT;
        }
    }

    // Fill the final stretch up to the current live time
    if (trackTime < currentTime) {
        rawSegments.push_back({activeSimulator->getCurrentProcessId(), trackTime, currentTime});
    }

    // 3. MERGE consecutive segments that have the same Process ID
    std::vector<Segment> mergedSegments;
    for (const auto& seg : rawSegments) {
        if (!mergedSegments.empty() && mergedSegments.back().pid == seg.pid && mergedSegments.back().end == seg.start) {
            mergedSegments.back().end = seg.end; // Stretch the previous block
        } else {
            mergedSegments.push_back(seg);
        }
    }

    // 4. Draw the merged blocks beautifully
    int drawX = 0;
    for (const auto& m : mergedSegments) {
        constexpr int blockW = 30;
        int duration = m.end - m.start;
        int rectWidth = duration * blockW;

        QColor color = (m.pid == -1) ? Qt::lightGray : QColor::fromHsv((m.pid * 50) % 360, 200, 200);
        scene->addRect(drawX, 0, rectWidth, blockH, QPen(Qt::black), QBrush(color));

        // Center the P# Label
        QString label = (m.pid == -1) ? "IDLE" : ("P" + QString::number(m.pid));
        QGraphicsTextItem* text = scene->addText(label);
        int textWidth = text->boundingRect().width();
        text->setPos(drawX + (rectWidth - textWidth) / 2, 15);

        // Draw Start Time at the left edge
        QGraphicsTextItem* tStart = scene->addText(QString::number(m.start));
        tStart->setPos(drawX - 5, blockH + 2);

        drawX += rectWidth;
    }

    // Draw the final End Time at the far right edge
    QGraphicsTextItem* tEnd = scene->addText(QString::number(currentTime));
    tEnd->setPos(drawX - 5, blockH + 2);
}

// ── Live Mode ─────────────────────────────────────────────────────────────

void MainWindow::on_btn_RunLive_clicked()
{
    if (processList.empty()) {
        QMessageBox::warning(this, "Warning", "Please add processes before running!");
        return;
    }

    isSimulationRunning = true;

    if (ui->radio_Static->isChecked()) {
        ui->btn_Add->setEnabled(false);
    }

    ui->input_Arrival->setVisible(false);
    ui->label_Arrival->setVisible(false);

    ui->groupBox_Scheduler->setEnabled(false);
    ui->groupBox_Mode->setEnabled(false);
    ui->btn_RunLive->setVisible(false);
    ui->btn_RunInstant->setVisible(false);

    std::deque<process> backendList;
    for (const auto& p : processList)
        backendList.emplace_back(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime);

    std::sort(backendList.begin(), backendList.end(),
              [](const process& a, const process& b){ return a.getArrivalTime() < b.getArrivalTime(); });

    currentTime = 0;
    ui->view_Gantt->scene()->clear();

    if (activeSimulator) { activeSimulator->stop(); delete activeSimulator; activeSimulator = nullptr; }

    const QString algorithm = ui->combo_Scheduler->currentText();
    const int     quantum   = ui->input_Quantum->value();

    activeSimulator = new simulator(algorithmToType(algorithm), quantum);
    activeSimulator->loadProcesses(backendList);

    activeSimulator->on_tick_callback = [this]() {
        QMetaObject::invokeMethod(this, "onLiveTimerTick", Qt::QueuedConnection);
    };

    activeSimulator->start();
}

void MainWindow::onLiveTimerTick()
{
    if (!activeSimulator) return;

    currentTime++; // Time advances
    drawGanttChart(); // Re-render the entire merged chart up to the new time

    ui->label_Time->setText("Current Time: " + QString::number(currentTime) + "s");
    ui->label_Wait->setText("Avg Waiting Time: " + QString::number(activeSimulator->getAvgWaitingTime(), 'f', 2));
    ui->label_Turnaround->setText("Avg Turnaround Time: " + QString::number(activeSimulator->getAvgTurnaroundTime(), 'f', 2));

    if (!activeSimulator->isRunning()) {
        isSimulationRunning = false;
        ui->btn_Add->setEnabled(true);
        ui->input_Arrival->setVisible(true);
        ui->label_Arrival->setVisible(true);
        ui->groupBox_Scheduler->setEnabled(true);
        ui->groupBox_Mode->setEnabled(true);
        ui->btn_RunLive->setVisible(true);
        ui->btn_RunInstant->setVisible(true);

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

    std::deque<process> backendList;
    for (const auto& p : processList)
        backendList.emplace_back(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime);

    std::sort(backendList.begin(), backendList.end(),
              [](const process& a, const process& b){ return a.getArrivalTime() < b.getArrivalTime(); });

    if (activeSimulator) { delete activeSimulator; activeSimulator = nullptr; }

    const QString algorithm = ui->combo_Scheduler->currentText();
    const int     quantum   = ui->input_Quantum->value();

    activeSimulator = new simulator(algorithmToType(algorithm), quantum);
    activeSimulator->loadProcesses(backendList);

    // Process backend logic instantly
    activeSimulator->runBatch();

    // Find the total time needed for the rendering logic
    auto timeline = activeSimulator->getTimeline();
    int maxTime = 0;
    for (const ::event& e : timeline) {
        if (e.getEndTime() > maxTime) maxTime = e.getEndTime();
    }
    currentTime = maxTime;

    // Use our new unified, merged drawing function!
    drawGanttChart();

    ui->label_Time->setText("Total Time: " + QString::number(currentTime) + "s");
    ui->label_Wait->setText("Avg Waiting Time: " + QString::number(activeSimulator->getAvgWaitingTime(), 'f', 2));
    ui->label_Turnaround->setText("Avg Turnaround Time: " + QString::number(activeSimulator->getAvgTurnaroundTime(), 'f', 2));

    QMessageBox::information(this, "Finished", "Simulation Complete!");
}

void MainWindow::on_radio_Dynamic_toggled(bool checked) const {
    ui->btn_RunInstant->setVisible(!checked);
}
