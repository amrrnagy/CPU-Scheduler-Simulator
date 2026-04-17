#ifndef CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H
#define CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H
#pragma once

#include <QMainWindow>
#include <vector>
#include <QString>

#include "../core/simulator.h"

struct Process {
    int pid{};
    int arrivalTime{};
    int burstTime{};
    int priority{};
    int remainingTime{};
    int completionTime = 0;
    int waitingTime    = 0;
    int turnaroundTime = 0;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void onLiveTimerTick();

private slots:
    void on_btn_Add_clicked();
    void on_btn_RunLive_clicked();
    void on_btn_RunInstant_clicked();
    void on_btn_Reset_clicked();
    void on_combo_Scheduler_currentIndexChanged(int index) const;
    void on_radio_Dynamic_toggled(bool checked) const;

private:
    Ui::MainWindow *ui;
    std::vector<Process> processList;
    int  currentTime;
    int  nextPid;
    bool isSimulationRunning;
    simulator* activeSimulator = nullptr;
    int ganttX{};

    void updateProcessTable() const;
    // Live remaining time update from backend snapshot.
    void updateRemainingTimesFromSnapshot(const SimSnapshot& snap) const;
    void drawGanttFromSnapshot(const SimSnapshot& snap) const;
};

#endif //CPU_SCHEDULER_SIMULATOR_MAINWINDOW_H
