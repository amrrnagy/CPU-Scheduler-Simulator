//
// Created by Ahmed Ayman on 4/15/2026.
//
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Initialize State Variables
    nextPid = 1;
    currentTime = 0;
    isSimulationRunning = false;

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
    if (isSimulationRunning) {
        QMessageBox::warning(this, "Warning", "Cannot add processes while simulation is running live!");
        return;
    }

    // 1. Create a new Process object
    Process p;
    p.pid = nextPid;
    p.arrivalTime = ui->input_Arrival->value();
    p.burstTime = ui->input_Burst->value();
    p.remainingTime = p.burstTime; // At the start, remaining = burst

    // Only grab priority if the input is currently visible
    if (ui->input_Priority->isVisible()) {
        p.priority = ui->input_Priority->value();
    } else {
        p.priority = 0;
    }

    // 2. Add to our backend list
    processList.push_back(p);
    nextPid++; // Increment for the next process

    // 3. Clear inputs for the user
    ui->input_Arrival->setValue(0);
    ui->input_Burst->setValue(1);
    ui->input_Priority->setValue(0);

    // 4. Update the visual table
    updateProcessTable();
}

void MainWindow::on_btn_Reset_clicked()
{
    // Stop timer if running
    liveTimer->stop();
    isSimulationRunning = false;

    // Reset variables
    processList.clear();
    nextPid = 1;
    currentTime = 0;

    // Clear UI
    ui->label_Time->setText("Current Time: 0s");
    ui->label_Wait->setText("Avg Waiting Time: 0.00");
    ui->label_Turnaround->setText("Avg Turnaround Time: 0.00");

    // Clear Table and Gantt Chart
    ui->table_Processes->setRowCount(0);
    ui->view_Gantt->scene()->clear();
}

// ==========================================
// HELPER FUNCTIONS
// ==========================================

void MainWindow::updateProcessTable()
{
    // Set the number of rows to match our vector size
    ui->table_Processes->setRowCount(processList.size());

    // Loop through our processes and fill the table
    for (size_t i = 0; i < processList.size(); ++i) {
        Process p = processList[i];

        QTableWidgetItem *pidItem = new QTableWidgetItem("P" + QString::number(p.pid));
        QTableWidgetItem *arrItem = new QTableWidgetItem(QString::number(p.arrivalTime));
        QTableWidgetItem *burstItem = new QTableWidgetItem(QString::number(p.burstTime));
        QTableWidgetItem *prioItem = new QTableWidgetItem(ui->input_Priority->isVisible() ? QString::number(p.priority) : "-");
        QTableWidgetItem *remItem = new QTableWidgetItem(QString::number(p.remainingTime));

        // Center align the text
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
// EXECUTION LOGIC (To Be Implemented Next)
// ==========================================

void MainWindow::on_btn_RunLive_clicked()
{
    if (processList.empty()) return;

    isSimulationRunning = true;
    liveTimer->start(1000); // Ticks every 1000ms (1 second)
}

void MainWindow::on_btn_RunInstant_clicked()
{
    if (processList.empty()) return;

    // We will put the instant calculation logic here later
}

void MainWindow::onLiveTimerTick()
{
    // Increment the clock
    currentTime++;
    ui->label_Time->setText("Current Time: " + QString::number(currentTime) + "s");

    // We will add the logic to pick the next process, decrement its time,
    // and draw the Gantt chart rectangle here!
}