//
// Created by Ahmed Ayman on 4/15/2026.
//

#ifndef CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H
#define CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H
#pragma once

#include <QMainWindow>
#include <QTimer>
#include <vector>
#include <QString>

// Backend Headers
#include "scheduler.h"
#include "FCFS.h"
// #include "SJF.h"   // Uncomment when ready
// #include "RR.h"    // Uncomment when ready

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
    ~MainWindow();

private slots:
    // ==========================================
    // 3. UI Event Handlers (Auto-connected via name matching)
    // ==========================================
    void on_btn_Add_clicked();
    void on_btn_RunLive_clicked();
    void on_btn_RunInstant_clicked();
    void on_btn_Reset_clicked();
    void on_combo_Scheduler_currentIndexChanged(int index);

    // ==========================================
    // 4. Custom Timer Slot
    // ==========================================
    void onLiveTimerTick();

private:
    Ui::MainWindow *ui;

    // ==========================================
    // 5. Backend State Variables
    // ==========================================
    std::vector<Process> processList; // Stores all processes (Frontend)
    QTimer *liveTimer;                // The 1-second clock

    int currentTime;                  // Tracks global execution time
    int nextPid;                      // Automatically assigns P1, P2, P3...
    bool isSimulationRunning;         // Prevents double-clicks while running

    // --- NEW: Variables for the Execution Engine & Drawing ---
    scheduler* activeScheduler = nullptr;
    std::vector<process> backendProcessList; // Formatted for the backend engine
    int incomingIndex = 0;                   // Tracks which process arrives next
    int ganttX = 0;                          // Tracks the X-coordinate for drawing

    // ==========================================
    // 6. Helper Functions
    // ==========================================
    void updateProcessTable();        // Refreshes the UI table
    void drawGanttChart();            // Handles the QGraphicsView drawing
    void calculateMetrics();          // Updates the Wait/Turnaround labels
};
#endif //CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H