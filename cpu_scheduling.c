#include "cpu_scheduling.h"
// ==========================================
//      MODULE 1: CPU SCHEDULING
// ==========================================

GanttSegment history[1000];
int historyIndex = 0;

void addToHistory(int pid, int start, int end)
{
    if (historyIndex > 0 && history[historyIndex - 1].pid == pid)
    {
        history[historyIndex - 1].endTime = end;
    }
    else
    {
        history[historyIndex].pid = pid;
        history[historyIndex].startTime = start;
        history[historyIndex].endTime = end;
        historyIndex++;
    }
}

void printGanttChart()
{
    printf("\n" YELLOW "--- GANTT CHART ---\n" RESET);
    int segWidths[1000];

    for (int i = 0; i < historyIndex; i++)
    {
        int duration = history[i].endTime - history[i].startTime;
        segWidths[i] = (duration * 2);
        if (segWidths[i] < 4)
            segWidths[i] = 4;
    }

    printf(" ");
    for (int i = 0; i < historyIndex; i++)
    {
        printf("+");
        for (int j = 0; j < segWidths[i]; j++)
            printf("-");
    }
    printf("+\n");

    printf(" ");
    for (int i = 0; i < historyIndex; i++)
    {
        printf("|");
        int totalSpaces = segWidths[i] - countDigits(history[i].pid) - 1;
        int leftPad = totalSpaces / 2;
        int rightPad = totalSpaces - leftPad;
        for (int j = 0; j < leftPad; j++)
            printf(" ");
        printf(CYAN "P%d" RESET, history[i].pid);
        for (int j = 0; j < rightPad; j++)
            printf(" ");
    }
    printf("|\n");

    printf(" ");
    for (int i = 0; i < historyIndex; i++)
    {
        printf("+");
        for (int j = 0; j < segWidths[i]; j++)
            printf("-");
    }
    printf("+\n");

    printf(" ");
    printf("%d", history[0].startTime);
    int currentPos = countDigits(history[0].startTime);
    int accumWidth = 0;

    for (int i = 0; i < historyIndex; i++)
    {
        accumWidth += segWidths[i] + 1;
        int nextTime = history[i].endTime;
        int spacesNeeded = accumWidth - currentPos;
        if (spacesNeeded < 1)
            spacesNeeded = 1;

        for (int s = 0; s < spacesNeeded; s++)
            printf(" ");
        printf("%d", nextTime);
        currentPos += spacesNeeded + countDigits(nextTime);
    }
    printf("\n");
}

void displaySchedulingTable(Process p[], int n)
{
    float avg_wt = 0, avg_tat = 0;
    printLine(78);
    printf(CYAN "| %-5s | %-10s | %-10s | %-10s | %-15s | %-12s |\n" RESET,
           "ID", "Arrival", "Burst", "Priority", "Turnaround", "Waiting");
    printLine(78);

    for (int i = 0; i < n; i++)
    {
        printf("| P%-4d | %-10d | %-10d | %-10d | %-15d | %-12d |\n",
               p[i].id, p[i].at, p[i].bt, p[i].pr, p[i].tat, p[i].wt);
        avg_wt += p[i].wt;
        avg_tat += p[i].tat;
    }
    printLine(78);
    printf(YELLOW "\nAverage Waiting Time: %.2f\n", avg_wt / (float)n);
    printf("Average Turnaround Time: %.2f\n" RESET, avg_tat / (float)n);
    printGanttChart();
}

void calculateMetrics(Process p[], int n)
{
    for (int i = 0; i < n; i++)
    {
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;
        if (p[i].wt < 0)
            p[i].wt = 0;
    }
}

void runFCFS(Process p[], int n)
{
    historyIndex = 0;
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (p[j].at > p[j + 1].at)
            {
                Process t = p[j];
                p[j] = p[j + 1];
                p[j + 1] = t;
            }

    int time = 0;
    for (int i = 0; i < n; i++)
    {
        if (time < p[i].at)
            time = p[i].at;
        int start = time;
        time += p[i].bt;
        p[i].ct = time;
        addToHistory(p[i].id, start, time);
    }
    calculateMetrics(p, n);
    printHeader("FCFS Results");
    displaySchedulingTable(p, n);
}

void runSJF(Process p[], int n)
{
    historyIndex = 0;
    int completed = 0, time = 0;
    bool isCompleted[100] = {false};

    while (completed != n)
    {
        int idx = -1, minBt = INT_MAX;
        for (int i = 0; i < n; i++)
        {
            if (p[i].at <= time && !isCompleted[i] && p[i].bt < minBt)
            {
                minBt = p[i].bt;
                idx = i;
            }
        }
        if (idx != -1)
        {
            int start = time;
            time += p[idx].bt;
            p[idx].ct = time;
            isCompleted[idx] = true;
            completed++;
            addToHistory(p[idx].id, start, time);
        }
        else
            time++;
    }
    calculateMetrics(p, n);
    printHeader("SJF Results");
    displaySchedulingTable(p, n);
}

void runStandardSRTF(Process p[], int n)
{
    historyIndex = 0;
    int completed = 0, time = 0, minRem = INT_MAX, shortest = -1;
    bool check = false;
    for (int i = 0; i < n; i++)
        p[i].rem_bt = p[i].bt;

    while (completed != n)
    {
        for (int i = 0; i < n; i++)
        {
            if ((p[i].at <= time) && (p[i].rem_bt < minRem) && (p[i].rem_bt > 0))
            {
                minRem = p[i].rem_bt;
                shortest = i;
                check = true;
            }
        }
        if (!check)
        {
            time++;
            continue;
        }

        p[shortest].rem_bt--;
        addToHistory(p[shortest].id, time, time + 1);
        minRem = p[shortest].rem_bt;
        if (minRem == 0)
            minRem = INT_MAX;
        time++;
        if (p[shortest].rem_bt == 0)
        {
            completed++;
            check = false;
            p[shortest].ct = time;
        }
    }
    calculateMetrics(p, n);
    printHeader("SRTF Results");
    displaySchedulingTable(p, n);
}

void runRoundRobin(Process p[], int n, int quantum)
{
    historyIndex = 0;
    int remProc = n, time = 0;
    for (int i = 0; i < n; i++)
        p[i].rem_bt = p[i].bt;
    // Sort by Arrival first
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (p[j].at > p[j + 1].at)
            {
                Process t = p[j];
                p[j] = p[j + 1];
                p[j + 1] = t;
            }

    while (remProc > 0)
    {
        bool done = false;
        for (int i = 0; i < n; i++)
        {
            if (p[i].rem_bt > 0 && p[i].at <= time)
            {
                done = true;
                int exec = (p[i].rem_bt > quantum) ? quantum : p[i].rem_bt;
                addToHistory(p[i].id, time, time + exec);
                time += exec;
                p[i].rem_bt -= exec;
                if (p[i].rem_bt == 0)
                {
                    p[i].ct = time;
                    remProc--;
                }
            }
        }
        if (!done)
            time++;
    }
    calculateMetrics(p, n);
    printHeader("Round Robin Results");
    displaySchedulingTable(p, n);
}

// --- Interactive Mode for CPU ---
void printDashboard(int time, int runningID, Process p[], int n, char *explanation)
{
    system(CLEAR_SCREEN);
    printHeader("INTERACTIVE TEACHING MODE");
    printf(CYAN " Current Time: " RESET "%d\n\n", time);

    printf("  +-------+        \n");
    if (runningID != -1)
        printf("  |  P%d   | <--- " GREEN "RUNNING" RESET "\n", runningID);
    else
        printf("  | IDLE  | <--- " RED "IDLE" RESET "\n");
    printf("  +-------+        \n\n");

    printf("  " YELLOW "Ready Queue: " RESET "[ ");
    bool anyReady = false;
    for (int i = 0; i < n; i++)
    {
        if (p[i].at <= time && p[i].rem_bt > 0 && p[i].id != runningID)
        {
            printf("P%d(%d) ", p[i].id, p[i].rem_bt);
            anyReady = true;
        }
    }
    if (!anyReady)
        printf("Empty ");
    printf("]\n\n");

    printf(MAGENTA "  LOGIC EXPLAINER:" RESET "\n  %s\n", explanation);
    printLine(60);
}

void runInteractiveSRTF(Process p[], int n)
{
    for (int i = 0; i < n; i++)
        p[i].rem_bt = p[i].bt;
    int completed = 0, currentTime = 0, prevProcess = -1;
    char explanation[256];

    printf(GREEN "\nStarting Interactive Simulation... (Maximize window)\n" RESET);
    waitForInput();

    while (completed != n)
    {
        int idx = -1, minRem = INT_MAX;
        for (int i = 0; i < n; i++)
        {
            if (p[i].at <= currentTime && p[i].rem_bt > 0)
            {
                if (p[i].rem_bt < minRem)
                {
                    minRem = p[i].rem_bt;
                    idx = i;
                }
                if (p[i].rem_bt == minRem && p[i].at < p[idx].at)
                    idx = i;
            }
        }

        if (idx != -1)
        {
            if (prevProcess != -1 && prevProcess != idx)
                sprintf(explanation, "PREEMPTION! P%d is shorter than P%d.", p[idx].id, p[prevProcess].id);
            else if (prevProcess == idx)
                sprintf(explanation, "P%d continues running.", p[idx].id);
            else
                sprintf(explanation, "P%d selected (Shortest Remaining).", p[idx].id);

            printDashboard(currentTime, p[idx].id, p, n, explanation);
            waitForInput();

            p[idx].rem_bt--;
            prevProcess = idx;
            if (p[idx].rem_bt == 0)
            {
                completed++;
                p[idx].ct = currentTime + 1;
                prevProcess = -1;
            }
        }
        else
        {
            sprintf(explanation, "No process arrived. CPU is IDLE.");
            printDashboard(currentTime, -1, p, n, explanation);
            waitForInput();
        }
        currentTime++;
    }
    calculateMetrics(p, n);
    system(CLEAR_SCREEN);
    printHeader("Interactive Session Finished");
    displaySchedulingTable(p, n);
}