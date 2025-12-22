#ifndef CPU_SCHEDULING_H
#define CPU_SCHEDULING_H

#include "utils.h" // Includes all your libraries, colors, and helpers

// --- Structures ---
typedef struct
{
    int id;
    int bt, at, pr, rem_bt, ct, tat, wt;
} Process;

typedef struct
{
    int pid;
    int startTime;
    int endTime;
} GanttSegment;

// --- Shared Variables ---
extern GanttSegment history[1000];
extern int historyIndex;

// --- Function Prototypes ---
void addToHistory(int pid, int start, int end);
void printGanttChart();
void displaySchedulingTable(Process p[], int n);
void calculateMetrics(Process p[], int n);

// Algorithms
void runFCFS(Process p[], int n);
void runSJF(Process p[], int n);
void runStandardSRTF(Process p[], int n);
void runRoundRobin(Process p[], int n, int quantum);

// Interactive Mode
void printDashboard(int time, int runningID, Process p[], int n, char *explanation);
void runInteractiveSRTF(Process p[], int n);

#endif