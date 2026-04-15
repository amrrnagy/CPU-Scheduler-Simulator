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
#include <QGraphicsScene> // ADDED: For the Gantt Chart canvas
#include <QColor>         // ADDED: For process colors

// ==========================================
// 1. Core Data Structures
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

    QColor color; // ADDED: Every process needs a color for the Gantt chart
};

// ADDED: Represents a single colored rectangle on the Gantt Chart
struct ExecutionBlock {
    QString processId;
    int startTime;
    int endTime;
    QColor color;
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
    // 3. UI Event Handlers
    // ==========================================
    void on_btn_Add_clicked();
    void on_btn_RunLive_clicked();
    void on_btn_RunInstant_clicked();
    void on_btn_Reset_clicked();
    void on_combo_Scheduler_currentIndexChanged(int index);

    // ==========================================
    // 4. Custom Timer Slot
    // ==========================================
    void on_liveTimer_tick(); // Runs every 1 second during live simulation

private:
    Ui::MainWindow *ui;

    // ==========================================
    // 5. Backend State Variables
    // ==========================================
    std::vector<Process> processList; // Stores all processes

    int currentTime;                  // Tracks global execution time
    int nextPid;                      // Automatically assigns P1, P2, P3...
    bool isSimulationRunning;         // Prevents double-clicks while running

    // --- Live Simulation Specifics ---
    QTimer *liveTimer;
    int liveProcessIndex;                       // Tracks which process is currently running
    std::vector<ExecutionBlock> liveBlocks;     // The blocks currently drawn on the chart

    // --- Visuals ---
    QGraphicsScene *ganttScene;       // ADDED: The canvas for the Gantt Chart

    // ==========================================
    // 6. Helper Functions
    // ==========================================
    void updateProcessTable();
    void calculateMetrics();

    // UPDATED: Now takes a list of blocks to draw
    void drawGanttChart(const std::vector<ExecutionBlock>& blocks);
};
#endif //CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H