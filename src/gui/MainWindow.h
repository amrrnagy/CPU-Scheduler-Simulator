//
// Created by Ahmed Ayman on 4/15/2026.
//

#ifndef CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H
#define CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H
#pragma once

#include <QMainWindow>
#include <vector>
#include <QString>

#include "../core/simulator.h"
#include "../core/models/process.h"

// ==========================================
// 1. Core Data Structure
// ==========================================
struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;

    // Variables that change during execution
    int remainingTime;

    // Metrics calculated at the end
    int completionTime = 0;
    int waitingTime = 0;
    int turnaroundTime = 0;
};

// ==========================================
// 2. Qt Namespace & Class Definition
// ==========================================
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    // This MUST be a slot so the background thread can call it safely
    void onLiveTimerTick();

private slots:
    // ==========================================
    // 3. UI Event Handlers
    // ==========================================
    void on_btn_Add_clicked();
    void on_btn_RunLive_clicked();
    void on_btn_RunInstant_clicked();
    void on_btn_Reset_clicked();
    void on_combo_Scheduler_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    // ==========================================
    // 5. State Variables
    // ==========================================
    std::vector<Process> processList; // Stores all frontend processes

    int currentTime;                  // Tracks global execution time
    int nextPid;                      // Automatically assigns P1, P2, P3...
    bool isSimulationRunning;         // Prevents double-clicks while running

    simulator* activeSimulator = nullptr; // Encapsulates the backend logic
    int ganttX;                       // X coordinate for drawing the Gantt chart

    // ==========================================
    // 6. Helper Functions
    // ==========================================
    void updateProcessTable();
    void drawGanttChart();
    void calculateMetrics();
};
#endif //CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H