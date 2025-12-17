#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

// --- Visual & Formatting Macros ---
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"

// --- Data Structures ---

typedef struct
{
    int id;
    int bt;     // Burst Time
    int at;     // Arrival Time
    int pr;     // Priority (Lower value = Higher priority)
    int rem_bt; // Remaining Burst Time (for preemptive)
    int ct;     // Completion Time
    int tat;    // Turnaround Time
    int wt;     // Waiting Time
} Process;

// Structure to record the execution timeline for Gantt Chart
typedef struct
{
    int pid;
    int startTime;
    int endTime;
} GanttSegment;

GanttSegment history[1000];
int historyIndex = 0;

// --- Helper Functions ---

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

void printLine(int width)
{
    printf(CYAN "+");
    for (int i = 0; i < width; i++)
        printf("-");
    printf("+\n" RESET);
}

void printHeader(const char *title)
{
    printf("\n" MAGENTA "========================================\n");
    printf("   %s\n", title);
    printf("========================================\n" RESET);
}

// --- IMPROVED VISUALIZATIONS ---

// Helper to count digits in a number
int countDigits(int n)
{
    if (n == 0)
        return 1;
    int count = 0;
    while (n != 0)
    {
        n /= 10;
        count++;
    }
    return count;
}

void printGanttChart()
{
    printf("\n" YELLOW "--- GANTT CHART ---\n" RESET);

    // 1. Calculate Widths
    // To make it look good, we enforce a minimum width for short processes
    // and a scaling factor for longer ones.
    int segWidths[1000];
    int totalWidth = 0;

    for (int i = 0; i < historyIndex; i++)
    {
        int duration = history[i].endTime - history[i].startTime;
        // Scale: duration * 2, but Minimum 4 chars to fit " Px "
        segWidths[i] = (duration * 2);
        if (segWidths[i] < 4)
            segWidths[i] = 4;
    }

    // 2. Print Top Border
    printf(" ");
    for (int i = 0; i < historyIndex; i++)
    {
        printf("+");
        for (int j = 0; j < segWidths[i]; j++)
            printf("-");
    }
    printf("+\n");

    // 3. Print Process IDs (Middle)
    printf(" ");
    for (int i = 0; i < historyIndex; i++)
    {
        printf("|");
        // Center the Px ID
        int totalSpaces = segWidths[i] - countDigits(history[i].pid) - 1; // -1 for 'P'
        int leftPad = totalSpaces / 2;
        int rightPad = totalSpaces - leftPad;

        for (int j = 0; j < leftPad; j++)
            printf(" ");
        printf(CYAN "P%d" RESET, history[i].pid);
        for (int j = 0; j < rightPad; j++)
            printf(" ");
    }
    printf("|\n");

    // 4. Print Bottom Border
    printf(" ");
    for (int i = 0; i < historyIndex; i++)
    {
        printf("+");
        for (int j = 0; j < segWidths[i]; j++)
            printf("-");
    }
    printf("+\n");

    // 5. Print Time Markers (Precisely Aligned)
    printf(" ");
    printf("%d", history[0].startTime);

    int currentPos = countDigits(history[0].startTime); // Cursor position tracker
    int accumWidth = 0;                                 // Where the vertical bar actually is physically

    for (int i = 0; i < historyIndex; i++)
    {
        accumWidth += segWidths[i] + 1; // +1 for the '|' or '+' character width

        int nextTime = history[i].endTime;
        int nextTimeWidth = countDigits(nextTime);

        // Calculate spaces needed to reach the next vertical bar
        // We want to align the number LEFT to the vertical bar position
        int spacesNeeded = accumWidth - currentPos;

        // Safety check if numbers overlap (rare but possible in tiny charts)
        if (spacesNeeded < 1)
            spacesNeeded = 1;

        for (int s = 0; s < spacesNeeded; s++)
            printf(" ");
        printf("%d", nextTime);

        currentPos += spacesNeeded + nextTimeWidth;
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

// --- ALGORITHMS ---

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

// 1. First Come First Serve
void runFCFS(Process p[], int n)
{
    historyIndex = 0;
    // Sort by Arrival Time
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (p[j].at > p[j + 1].at)
            {
                Process temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }

    int currentTime = 0;
    for (int i = 0; i < n; i++)
    {
        if (currentTime < p[i].at)
            currentTime = p[i].at;

        int start = currentTime;
        currentTime += p[i].bt;
        p[i].ct = currentTime;
        addToHistory(p[i].id, start, currentTime);
    }
    calculateMetrics(p, n);
    printHeader("FCFS Scheduling Results");
    displaySchedulingTable(p, n);
}

// 2. SJF (Non-Preemptive)
void runSJF(Process p[], int n)
{
    historyIndex = 0;
    int completed = 0, currentTime = 0;
    bool isCompleted[100] = {false};

    while (completed != n)
    {
        int idx = -1;
        int minBt = INT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (p[i].at <= currentTime && !isCompleted[i])
            {
                if (p[i].bt < minBt)
                {
                    minBt = p[i].bt;
                    idx = i;
                }
            }
        }

        if (idx != -1)
        {
            int start = currentTime;
            currentTime += p[idx].bt;
            p[idx].ct = currentTime;
            isCompleted[idx] = true;
            completed++;
            addToHistory(p[idx].id, start, currentTime);
        }
        else
        {
            currentTime++;
        }
    }
    calculateMetrics(p, n);
    printHeader("SJF (Non-Preemptive) Results");
    displaySchedulingTable(p, n);
}

// 3. SRTF
void runSRTF(Process p[], int n)
{
    historyIndex = 0;
    int completed = 0, currentTime = 0;
    int minRem = INT_MAX;
    int shortest = -1;
    bool check = false;

    for (int i = 0; i < n; i++)
        p[i].rem_bt = p[i].bt;

    while (completed != n)
    {
        for (int i = 0; i < n; i++)
        {
            if ((p[i].at <= currentTime) && (p[i].rem_bt < minRem) && (p[i].rem_bt > 0))
            {
                minRem = p[i].rem_bt;
                shortest = i;
                check = true;
            }
        }

        if (!check)
        {
            currentTime++;
            continue;
        }

        p[shortest].rem_bt--;
        addToHistory(p[shortest].id, currentTime, currentTime + 1);
        minRem = p[shortest].rem_bt;
        if (minRem == 0)
            minRem = INT_MAX;

        currentTime++;

        if (p[shortest].rem_bt == 0)
        {
            completed++;
            check = false;
            p[shortest].ct = currentTime;
        }
    }
    calculateMetrics(p, n);
    printHeader("SRTF (Preemptive SJF) Results");
    displaySchedulingTable(p, n);
}

// 4. Priority (Non-Preemptive)
void runPriorityNP(Process p[], int n)
{
    historyIndex = 0;
    int completed = 0, currentTime = 0;
    bool isCompleted[100] = {false};

    while (completed != n)
    {
        int idx = -1;
        int bestPriority = INT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (p[i].at <= currentTime && !isCompleted[i])
            {
                if (p[i].pr < bestPriority)
                {
                    bestPriority = p[i].pr;
                    idx = i;
                }
                else if (p[i].pr == bestPriority)
                {
                    if (p[i].at < p[idx].at)
                        idx = i;
                }
            }
        }

        if (idx != -1)
        {
            int start = currentTime;
            currentTime += p[idx].bt;
            p[idx].ct = currentTime;
            isCompleted[idx] = true;
            completed++;
            addToHistory(p[idx].id, start, currentTime);
        }
        else
        {
            currentTime++;
        }
    }
    calculateMetrics(p, n);
    printHeader("Priority (Non-Preemptive) Results");
    displaySchedulingTable(p, n);
}

// 5. Priority (Preemptive)
void runPriorityP(Process p[], int n)
{
    historyIndex = 0;
    int completed = 0, currentTime = 0;

    for (int i = 0; i < n; i++)
        p[i].rem_bt = p[i].bt;

    while (completed != n)
    {
        int idx = -1;
        int bestPriority = INT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (p[i].at <= currentTime && p[i].rem_bt > 0)
            {
                if (p[i].pr < bestPriority)
                {
                    bestPriority = p[i].pr;
                    idx = i;
                }
            }
        }

        if (idx != -1)
        {
            p[idx].rem_bt--;
            addToHistory(p[idx].id, currentTime, currentTime + 1);
            currentTime++;
            if (p[idx].rem_bt == 0)
            {
                p[idx].ct = currentTime;
                completed++;
            }
        }
        else
        {
            currentTime++;
        }
    }
    calculateMetrics(p, n);
    printHeader("Priority (Preemptive) Results");
    displaySchedulingTable(p, n);
}

// 6. Round Robin
void runRoundRobin(Process p[], int n, int quantum)
{
    historyIndex = 0;
    int remainingProcesses = n;
    int currentTime = 0;

    // Sort by arrival first
    for (int x = 0; x < n - 1; x++)
    {
        for (int y = 0; y < n - x - 1; y++)
        {
            if (p[y].at > p[y + 1].at)
            {
                Process temp = p[y];
                p[y] = p[y + 1];
                p[y + 1] = temp;
            }
        }
    }

    for (int j = 0; j < n; j++)
        p[j].rem_bt = p[j].bt;

    while (remainingProcesses > 0)
    {
        bool doneSomething = false;

        for (int i = 0; i < n; i++)
        {
            if (p[i].rem_bt > 0 && p[i].at <= currentTime)
            {
                doneSomething = true;
                int executeTime = (p[i].rem_bt > quantum) ? quantum : p[i].rem_bt;

                addToHistory(p[i].id, currentTime, currentTime + executeTime);
                currentTime += executeTime;
                p[i].rem_bt -= executeTime;

                if (p[i].rem_bt == 0)
                {
                    p[i].ct = currentTime;
                    remainingProcesses--;
                }
            }
        }
        if (!doneSomething)
            currentTime++;
    }
    calculateMetrics(p, n);
    printHeader("Round Robin Results");
    displaySchedulingTable(p, n);
}

// --- MEMORY FUNCTIONS ---
void displayMemoryTable(int processes, int processSize[], int allocation[])
{
    printLine(48);
    printf(CYAN "| %-10s | %-15s | %-15s |\n" RESET, "Process No", "Process Size", "Block No");
    printLine(48);
    for (int i = 0; i < processes; i++)
    {
        printf("| %-10d | %-15d | ", i + 1, processSize[i]);
        if (allocation[i] != -1)
            printf(GREEN "%-15d" RESET " |\n", allocation[i] + 1);
        else
            printf(RED "%-15s" RESET " |\n", "Not Allocated");
    }
    printLine(48);
}
void implementBestFit(int blockSize[], int blocks, int processSize[], int processes)
{
    int allocation[processes], occupied[blocks];
    for (int i = 0; i < processes; i++)
        allocation[i] = -1;
    for (int i = 0; i < blocks; i++)
        occupied[i] = 0;
    for (int i = 0; i < processes; i++)
    {
        int indexPlaced = -1;
        for (int j = 0; j < blocks; j++)
        {
            if (blockSize[j] >= processSize[i] && !occupied[j])
            {
                if (indexPlaced == -1)
                    indexPlaced = j;
                else if (blockSize[j] < blockSize[indexPlaced])
                    indexPlaced = j;
            }
        }
        if (indexPlaced != -1)
        {
            allocation[i] = indexPlaced;
            occupied[indexPlaced] = 1;
        }
    }
    printHeader("Best Fit Allocation Results");
    displayMemoryTable(processes, processSize, allocation);
}
void implementFirstFit(int blockSize[], int blocks, int processSize[], int processes)
{
    int allocation[processes], occupied[blocks];
    for (int i = 0; i < processes; i++)
        allocation[i] = -1;
    for (int i = 0; i < blocks; i++)
        occupied[i] = 0;
    for (int i = 0; i < processes; i++)
    {
        for (int j = 0; j < blocks; j++)
        {
            if (!occupied[j] && blockSize[j] >= processSize[i])
            {
                allocation[i] = j;
                occupied[j] = 1;
                break;
            }
        }
    }
    printHeader("First Fit Allocation Results");
    displayMemoryTable(processes, processSize, allocation);
}
void implementWorstFit(int blockSize[], int blocks, int processSize[], int processes)
{
    int allocation[processes], occupied[blocks];
    for (int i = 0; i < processes; i++)
        allocation[i] = -1;
    for (int i = 0; i < blocks; i++)
        occupied[i] = 0;
    for (int i = 0; i < processes; i++)
    {
        int indexPlaced = -1;
        for (int j = 0; j < blocks; j++)
        {
            if (blockSize[j] >= processSize[i] && !occupied[j])
            {
                if (indexPlaced == -1)
                    indexPlaced = j;
                else if (blockSize[j] > blockSize[indexPlaced])
                    indexPlaced = j;
            }
        }
        if (indexPlaced != -1)
        {
            allocation[i] = indexPlaced;
            occupied[indexPlaced] = 1;
        }
    }
    printHeader("Worst Fit Allocation Results");
    displayMemoryTable(processes, processSize, allocation);
}

// --- MAIN DRIVER ---

int main()
{
    int choice;
    while (1)
    {
        printHeader("OS ALGORITHM SIMULATOR");
        printf(YELLOW "1." RESET " Memory Allocation\n");
        printf(YELLOW "2." RESET " CPU Scheduling\n");
        printf(YELLOW "3." RESET " Exit\n");
        printf(CYAN "\nSelect Option: " RESET);
        scanf("%d", &choice);

        if (choice == 3)
            break;

        if (choice == 1)
        {
            int blocks, blockSize[100], processSize[100], processes, type;
            printf("Enter number of memory blocks: ");
            scanf("%d", &blocks);
            printf("Enter size of each block: ");
            for (int i = 0; i < blocks; i++)
                scanf("%d", &blockSize[i]);
            printf("Enter number of processes: ");
            scanf("%d", &processes);
            printf("Enter size of each process: ");
            for (int i = 0; i < processes; i++)
                scanf("%d", &processSize[i]);

            while (1)
            {
                printf("\n" BLUE "Choose Algorithm:\n" RESET);
                printf("1. Best Fit\n2. First Fit\n3. Worst Fit\n4. Back to Main Menu\n");
                printf(CYAN "Selection: " RESET);
                scanf("%d", &type);
                if (type == 4)
                    break;

                if (type == 1)
                    implementBestFit(blockSize, blocks, processSize, processes);
                else if (type == 2)
                    implementFirstFit(blockSize, blocks, processSize, processes);
                else if (type == 3)
                    implementWorstFit(blockSize, blocks, processSize, processes);
            }
        }
        else if (choice == 2)
        {
            int n;
            Process original_p[100];
            Process working_p[100];

            printHeader("CPU Scheduling Setup");
            printf("Enter number of processes: ");
            scanf("%d", &n);

            for (int i = 0; i < n; i++)
            {
                original_p[i].id = i + 1;
                printf("\nProcess P%d:\n", i + 1);
                printf("  Burst Time: ");
                scanf("%d", &original_p[i].bt);
                printf("  Arrival Time: ");
                scanf("%d", &original_p[i].at);
                printf("  Priority (Lower # = Higher Priority): ");
                scanf("%d", &original_p[i].pr);
            }

            while (1)
            {
                for (int i = 0; i < n; i++)
                    working_p[i] = original_p[i];

                int type, subType;
                printf("\n" BLUE "Choose Scheduling Algorithm:\n" RESET);
                printf("1. FCFS\n2. SJF (Non-Preemptive)\n3. SRTF (Preemptive SJF)\n");
                printf("4. Priority Scheduling\n5. Round Robin\n6. Back to Main Menu\n");
                printf(CYAN "Selection: " RESET);
                scanf("%d", &type);

                if (type == 6)
                    break;

                switch (type)
                {
                case 1:
                    runFCFS(working_p, n);
                    break;
                case 2:
                    runSJF(working_p, n);
                    break;
                case 3:
                    runSRTF(working_p, n);
                    break;
                case 4:
                    printf("\n  1. Non-Preemptive\n  2. Preemptive\n  Select: ");
                    scanf("%d", &subType);
                    if (subType == 1)
                        runPriorityNP(working_p, n);
                    else
                        runPriorityP(working_p, n);
                    break;
                case 5:
                {
                    int quantum;
                    printf("Enter Time Quantum: ");
                    scanf("%d", &quantum);
                    runRoundRobin(working_p, n, quantum);
                    break;
                }
                default:
                    printf(RED "Invalid.\n" RESET);
                }

                printf(GREEN "\nDo you want to try another algorithm with the SAME values? (1=Yes, 0=No): " RESET);
                int cont;
                scanf("%d", &cont);
                if (cont == 0)
                    break;
            }
        }
    }
    return 0;
}