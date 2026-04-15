#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QColor>
#include <QBrush>
#include <QPen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Initialize State Variables
    nextPid = 1;
    currentTime = 0;
    isSimulationRunning = false;
    activeScheduler = nullptr;
    incomingIndex = 0;
    ganttX = 0;

    // 2. Setup the Live Timer
    liveTimer = new QTimer(this);
    connect(liveTimer, &QTimer::timeout, this, &MainWindow::onLiveTimerTick);

    // 3. Setup the Gantt Chart Scene
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->view_Gantt->setScene(scene);
    ui->view_Gantt->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // 4. Initial UI State (Hide Quantum & Priority since FCFS is default)
    ui->label_Quantum->setVisible(false);
    ui->input_Quantum->setVisible(false);
    ui->label_Priority->setVisible(false);
    ui->input_Priority->setVisible(false);
}

MainWindow::~MainWindow()
{
    if (activeScheduler) {
        delete activeScheduler;
    }
    delete ui;
}

// ==========================================
// UI EVENT HANDLERS
// ==========================================

void MainWindow::on_combo_Scheduler_currentIndexChanged(int index)
{
    QString algorithm = ui->combo_Scheduler->currentText();

    // Hide everything by default
    ui->label_Quantum->setVisible(false);
    ui->input_Quantum->setVisible(false);
    ui->label_Priority->setVisible(false);
    ui->input_Priority->setVisible(false);

    // Show specific inputs based on the algorithm chosen
    if (algorithm == "Round Robin") {
        ui->label_Quantum->setVisible(true);
        ui->input_Quantum->setVisible(true);
    }
    else if (algorithm.contains("Priority")) {
        ui->label_Priority->setVisible(true);
        ui->input_Priority->setVisible(true);
    }
}

void MainWindow::on_btn_Add_clicked()
{
    // Check which mode the user selected in the UI
    bool isStaticMode = ui->radio_Static->isChecked();

    // Only block additions if the simulation is running AND we are in Static mode
    if (isSimulationRunning && isStaticMode) {
        QMessageBox::warning(this, "Warning", "Cannot add processes while simulation is running in Static Mode!");
        return;
    }

    // 1. Create a new Process object
    Process p;
    p.pid = nextPid;

    // 2. Determine Arrival Time based on the state
    if (isSimulationRunning && !isStaticMode) {
        // Dynamic mode during runtime: force arrival time to be the current simulation time
        p.arrivalTime = currentTime;
    } else {
        p.arrivalTime = ui->input_Arrival->value();
    }

    p.burstTime = ui->input_Burst->value();
    p.remainingTime = p.burstTime;

    // 3. Grab priority if the input is currently visible
    if (ui->input_Priority->isVisible()) {
        p.priority = ui->input_Priority->value();
    } else {
        p.priority = 0;
    }

    // 4. Add to our frontend list
    processList.push_back(p);

    // If running dynamically, we also need to push it to the backend list immediately!
    if (isSimulationRunning && !isStaticMode) {
        backendProcessList.push_back(process(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime));
    }

    nextPid++; // Increment for the next process

    // 5. Clear inputs for the user
    if (!isSimulationRunning) {
        ui->input_Arrival->setValue(0);
    }
    ui->input_Burst->setValue(1);
    ui->input_Priority->setValue(0);

    // 6. Update the visual table
    updateProcessTable();
}

void MainWindow::on_btn_Reset_clicked()
{
    // Stop timer if running
    liveTimer->stop();
    isSimulationRunning = false;

    // Clean up backend
    if (activeScheduler) {
        delete activeScheduler;
        activeScheduler = nullptr;
    }

    // Re-enable all inputs for the next setup phase
    ui->btn_Add->setEnabled(true);
    ui->input_Arrival->setEnabled(true);
    ui->input_Arrival->setValue(0);

    // Reset variables
    processList.clear();
    backendProcessList.clear();
    nextPid = 1;
    currentTime = 0;
    incomingIndex = 0;
    ganttX = 0;

    // Clear UI
    ui->label_Time->setText("Current Time: 0s");
    ui->label_Wait->setText("Avg Waiting Time: 0.00");
    ui->label_Turnaround->setText("Avg Turnaround Time: 0.00");

    // Clear Table and Gantt Chart
    ui->table_Processes->setRowCount(0);
    ui->view_Gantt->scene()->clear();
}



void MainWindow::updateProcessTable()
{
    ui->table_Processes->setRowCount(processList.size());

    for (size_t i = 0; i < processList.size(); ++i) {
        Process p = processList[i];

        QTableWidgetItem *pidItem = new QTableWidgetItem("P" + QString::number(p.pid));
        QTableWidgetItem *arrItem = new QTableWidgetItem(QString::number(p.arrivalTime));
        QTableWidgetItem *burstItem = new QTableWidgetItem(QString::number(p.burstTime));
        QTableWidgetItem *prioItem = new QTableWidgetItem(ui->input_Priority->isVisible() ? QString::number(p.priority) : "-");
        QTableWidgetItem *remItem = new QTableWidgetItem(QString::number(p.remainingTime));

        pidItem->setTextAlignment(Qt::AlignCenter);
        arrItem->setTextAlignment(Qt::AlignCenter);
        burstItem->setTextAlignment(Qt::AlignCenter);
        prioItem->setTextAlignment(Qt::AlignCenter);
        remItem->setTextAlignment(Qt::AlignCenter);

        ui->table_Processes->setItem(i, 0, pidItem);
        ui->table_Processes->setItem(i, 1, arrItem);
        ui->table_Processes->setItem(i, 2, burstItem);
        ui->table_Processes->setItem(i, 3, prioItem);
        ui->table_Processes->setItem(i, 4, remItem);
    }
}

// ==========================================
// EXECUTION LOGIC
// ==========================================

void MainWindow::on_btn_RunLive_clicked()
{
    if (processList.empty() && ui->radio_Static->isChecked()) {
        QMessageBox::warning(this, "Warning", "Please add processes before starting a Static simulation.");
        return;
    }

    isSimulationRunning = true;

    // 1. Adjust UI based on the selected mode
    ui->btn_Add->setEnabled(!ui->radio_Static->isChecked());
    ui->input_Arrival->setEnabled(false);

    // 2. Setup the Backend Data
    backendProcessList.clear();
    for (const auto& p : processList) {
        backendProcessList.push_back(process(p.pid, p.arrivalTime, p.burstTime, p.priority, p.remainingTime));
    }
    incomingIndex = 0;
    ganttX = 0; // Reset Gantt drawing position

    // 3. Initialize the correct scheduler
    if (activeScheduler) {
        delete activeScheduler;
        activeScheduler = nullptr;
    }

    QString algorithm = ui->combo_Scheduler->currentText();
    if (algorithm == "FCFS") {
        activeScheduler = new FCFS();
    }
    // Uncomment these as you implement the backend classes!
    // else if (algorithm == "SJF") {
    //     activeScheduler = new SJF();
    // }
    // else if (algorithm == "Round Robin") {
    //     activeScheduler = new RR(ui->input_Quantum->value());
    // }

    if (!activeScheduler) {
         QMessageBox::critical(this, "Error", "Scheduler algorithm not fully implemented yet!");
         isSimulationRunning = false;
         return;
    }

    // 4. Start the clock
    liveTimer->start(1000);
}

void MainWindow::onLiveTimerTick()
{
    if (!activeScheduler) return;

    // 1. Get the scene
    QGraphicsScene *scene = ui->view_Gantt->scene();
    int blockWidth = 30; // How wide 1 second looks on screen
    int blockHeight = 50;

    // 2. See who is running BEFORE we tick (so we draw the current state)
    int runningId = activeScheduler->getCurrentProcessId();

    // 3. Draw the Gantt Chart Block
    if (runningId != -1) {
        // Create a unique color for each process based on its ID
        QColor processColor = QColor::fromHsv((runningId * 50) % 360, 200, 200);

        scene->addRect(ganttX, 0, blockWidth, blockHeight, QPen(Qt::black), QBrush(processColor));
        QGraphicsTextItem *text = scene->addText("P" + QString::number(runningId));
        text->setPos(ganttX + 5, 15);
    } else {
        // CPU is Idle - Draw a gray box
        scene->addRect(ganttX, 0, blockWidth, blockHeight, QPen(Qt::black), QBrush(Qt::lightGray));
        QGraphicsTextItem *text = scene->addText("IDLE");
        text->setPos(ganttX + 2, 15);
    }

    // Move our drawing pen to the right for the next second
    ganttX += blockWidth;

    // 4. Tick the Backend System
    bool hasMoreWork = activeScheduler->tick(currentTime, backendProcessList, incomingIndex);

    // 5. Increment UI Clock
    currentTime++;
    ui->label_Time->setText("Current Time: " + QString::number(currentTime) + "s");

    // Optional: You could update processList's remainingTime here and call updateProcessTable()
    // to see the table count down live!

    // 6. Stop if finished
    if (!hasMoreWork && (incomingIndex >= (int)backendProcessList.size())) {
        liveTimer->stop();
        isSimulationRunning = false;
        QMessageBox::information(this, "Finished", "Simulation Complete!");

        // Re-enable inputs
        ui->btn_Add->setEnabled(true);
        ui->input_Arrival->setEnabled(true);
    }
}

void MainWindow::on_btn_RunInstant_clicked()
{
    if (processList.empty()) return;
    // To be implemented...
}