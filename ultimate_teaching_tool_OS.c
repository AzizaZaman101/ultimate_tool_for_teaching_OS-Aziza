#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <math.h>

// --- OS Compatibility ---
#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#define SLEEP_MS(ms) Sleep(ms)
#else
#define CLEAR_SCREEN "clear"
#define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

// For the wait function
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// --- Macros & Colors ---
#define MAX_REQ 100
#define MAX_FRAMES 10
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"

// Global Arrays for Simulations
int requests[MAX_REQ];
int pages[MAX_REQ];
int frames[MAX_FRAMES];

// ==========================================
//      VISUALIZATION DELAY
// ==========================================

// Slows down the output so the student can process the information
void slowPrint(const char *text, int delay_ms)
{
    for (int i = 0; text[i] != '\0'; i++)
    {
        printf("%c", text[i]);
        fflush(stdout); // Force print character immediately
        SLEEP_MS(delay_ms);
    }
}

// Simple pause before returning to menu
void waitForStudent()
{
    printf(YELLOW "\n\n[Analysis Complete. Press ENTER to return to Menu...]" RESET);
    getchar();
}

// ==========================================
//      ROBUST INPUT HANDLING
// ==========================================

void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int getSafeInt()
{
    int value;
    int status;
    char after;

    while (1)
    {
        status = scanf("%d", &value);

        if (status == 1)
        {
            after = getchar();
            if (after == '\n' || after == ' ' || after == '\t')
            {
                return value;
            }
            else if (after == ',')
            {
                return value;
            }
            else
            {
                if (after != '\n')
                    clearBuffer();
                return value;
            }
        }
        else
        {
            printf(RED "  Invalid input! Please enter a number: " RESET);
            clearBuffer();
        }
    }
}

void waitForInput()
{
    printf(YELLOW "\n[Press ENTER to continue...]" RESET);
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// ==========================================
//      VISUALIZATION HELPERS
// ==========================================

void printLine(int width)
{
    printf(CYAN "+");
    for (int i = 0; i < width; i++)
        printf("-");
    printf("+\n" RESET);
}

void printHeader(const char *title)
{
    printf("\n" MAGENTA "==================================================\n");
    printf("   %s\n", title);
    printf("==================================================\n" RESET);
}

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

// ==========================================
//      MODULE 1: CPU SCHEDULING
// ==========================================

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

// ==========================================
//      MODULE 2: MEMORY ALLOCATION
// ==========================================

void displayMemoryAnalysis(int processes, int processSize[], int allocation[], int blockSize[])
{
    int totalInternalFrag = 0, unallocatedCount = 0;
    printHeader("MEMORY ANALYSIS");
    printf(CYAN "| %-10s | %-12s | %-10s | %-15s |\n" RESET, "Process", "Size", "Block", "Internal Frag");
    printLine(60);

    for (int i = 0; i < processes; i++)
    {
        printf("| P%-8d | %-12d | ", i + 1, processSize[i]);
        if (allocation[i] != -1)
        {
            int frag = blockSize[allocation[i]] - processSize[i];
            totalInternalFrag += frag;
            printf("%-10d | " GREEN "%-15d" RESET " |\n", allocation[i] + 1, frag);
        }
        else
        {
            unallocatedCount++;
            printf(RED "%-10s" RESET " | %-15s |\n", "N/A", "N/A");
        }
    }
    printLine(60);
    printf(YELLOW "Total Internal Fragmentation: %d KB\n", totalInternalFrag);
    printf("Unallocated Processes: %d\n" RESET, unallocatedCount);
}

void runMemoryAllocation()
{
    int blocks, processes, type;
    int bSize[100], pSize[100];

    printf("Enter number of memory blocks: ");
    blocks = getSafeInt();
    printf("Enter sizes of blocks (separated by space or enter): ");
    for (int i = 0; i < blocks; i++)
        bSize[i] = getSafeInt();

    printf("Enter number of processes: ");
    processes = getSafeInt();
    printf("Enter sizes of processes: ");
    for (int i = 0; i < processes; i++)
        pSize[i] = getSafeInt();

    while (1)
    {
        printf("\n" BLUE "Choose Allocation Strategy:\n" RESET);
        printf("1. Best Fit\n2. First Fit\n3. Worst Fit\n4. Back\nSelection: ");
        type = getSafeInt();
        if (type == 4)
            break;

        int allocation[processes], occupied[blocks];
        for (int i = 0; i < processes; i++)
            allocation[i] = -1;
        for (int i = 0; i < blocks; i++)
            occupied[i] = 0;

        for (int i = 0; i < processes; i++)
        {
            int idx = -1;
            for (int j = 0; j < blocks; j++)
            {
                if (!occupied[j] && bSize[j] >= pSize[i])
                {
                    if (type == 1)
                    {
                        if (idx == -1 || bSize[j] < bSize[idx])
                            idx = j;
                    }
                    else if (type == 2)
                    {
                        idx = j;
                        break;
                    }
                    else if (type == 3)
                    {
                        if (idx == -1 || bSize[j] > bSize[idx])
                            idx = j;
                    }
                }
            }
            if (idx != -1)
            {
                allocation[i] = idx;
                occupied[idx] = 1;
            }
        }
        displayMemoryAnalysis(processes, pSize, allocation, bSize);
    }
}

// ==========================================
//      MODULE 3: BANKER'S ALGORITHM
// ==========================================

void runBankersAlgorithm()
{
    int n, m;
    printHeader("BANKER'S ALGORITHM");
    printf("Enter number of processes: ");
    n = getSafeInt();
    printf("Enter number of resource types: ");
    m = getSafeInt();

    int alloc[n][m], max[n][m], avail[m], need[n][m], safeSeq[n];
    int work[m];
    bool finish[n];

    printf("\nEnter Allocation Matrix:\n");
    for (int i = 0; i < n; i++)
    {
        printf("P%d: ", i);
        for (int j = 0; j < m; j++)
            alloc[i][j] = getSafeInt();
    }

    printf("\nEnter Max Matrix:\n");
    for (int i = 0; i < n; i++)
    {
        printf("P%d: ", i);
        for (int j = 0; j < m; j++)
        {
            max[i][j] = getSafeInt();
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }

    printf("\nEnter Available Resources: ");
    for (int i = 0; i < m; i++)
    {
        avail[i] = getSafeInt();
        work[i] = avail[i];
    }

    for (int i = 0; i < n; i++)
        finish[i] = false;
    int count = 0;
    while (count < n)
    {
        bool found = false;
        for (int p = 0; p < n; p++)
        {
            if (!finish[p])
            {
                int j;
                for (j = 0; j < m; j++)
                    if (need[p][j] > work[j])
                        break;
                if (j == m)
                {
                    for (int k = 0; k < m; k++)
                        work[k] += alloc[p][k];
                    safeSeq[count++] = p;
                    finish[p] = true;
                    found = true;
                }
            }
        }
        if (!found)
        {
            printf(RED "\nSystem is in an UNSAFE state! (Deadlock Risk)\n" RESET);
            return;
        }
    }
    printf(GREEN "\nSystem is in a SAFE state.\nSafe Sequence: " RESET);
    for (int i = 0; i < n; i++)
        printf("P%d%s", safeSeq[i], (i == n - 1) ? "" : " -> ");
    printf("\n");
}

// ==========================================
//      MODULE 4: DISK SCHEDULING
// ==========================================

void runDiskScheduler()
{
    int head, n, i, seek = 0;
    int req[MAX_REQ];
    printHeader("DISK SCHEDULING (FCFS/VISUALIZER)");

    printf("Enter current head position: ");
    head = getSafeInt();
    printf("Enter number of requests: ");
    n = getSafeInt();
    printf("Enter the requests: ");
    for (i = 0; i < n; i++)
        scanf("%d", &req[i]);
    clearBuffer();

    printf("\n" YELLOW "Simulating Disk Arm Movement..." RESET "\n");
    SLEEP_MS(800);

    printf(CYAN "%d" RESET, head);
    int curr = head;

    for (i = 0; i < n; i++)
    {
        SLEEP_MS(1000); // Wait 1 second between each move
        int move = abs(req[i] - curr);
        seek += move;
        curr = req[i];
        printf(" --(" RED "%d" RESET ")--> " CYAN "%d" RESET, move, curr);
        fflush(stdout);
    }

    SLEEP_MS(500);
    printf("\n\n" GREEN "Calculation: Sum of all head displacements = %d units." RESET, seek);
    waitForStudent();
}

// ==========================================
//      MODULE 5: PAGE REPLACEMENT
// ==========================================

void runPageReplacement()
{
    int f_size, p_count, i, j, faults = 0, top = 0;
    int f[MAX_FRAMES], p[MAX_REQ];

    printHeader("VIRTUAL MEMORY: FIFO PAGE REPLACEMENT");
    printf("Frame Count: ");
    f_size = getSafeInt();
    printf("Sequence Size: ");
    p_count = getSafeInt();
    printf("Sequence: ");
    for (i = 0; i < p_count; i++)
        scanf("%d", &p[i]);
    clearBuffer();

    for (i = 0; i < f_size; i++)
        f[i] = -1; // Initialize frames as empty

    printf("\nRef | Frame Contents\t\tStatus\n----|-------------------------");
    for (i = 0; i < p_count; i++)
    {
        SLEEP_MS(1200); // Delay for student to predict if it's a Hit or Miss
        bool hit = false;
        for (j = 0; j < f_size; j++)
            if (f[j] == p[i])
                hit = true;

        printf("\n %d  | ", p[i]);
        if (!hit)
        {
            f[top] = p[i];
            top = (top + 1) % f_size;
            faults++;
            for (j = 0; j < f_size; j++)
            {
                if (f[j] != -1)
                    printf("[%d] ", f[j]);
                else
                    printf("[ ] ");
            }
            printf("\t" RED "MISS (Page Fault)" RESET);
        }
        else
        {
            for (j = 0; j < f_size; j++)
            {
                if (f[j] != -1)
                    printf("[%d] ", f[j]);
                else
                    printf("[ ] ");
            }
            printf("\t" GREEN "HIT (Found in RAM)" RESET);
        }
        fflush(stdout);
    }
    waitForStudent();
}

// ==========================================
//      MODULE 6: RACE CONDITION
// ==========================================

void runRaceCondition()
{
    printHeader("CONCURRENCY: RACE CONDITION SIMULATOR");
    printf("Scenario: Two threads incrementing a shared variable.\n");
    printf("Problem: Without locks, a context switch can cause lost updates.\n\n");

    int shared = 100;
    printf(BLUE "Initial Shared Variable Value: %d\n" RESET, shared);

    printf(YELLOW "\n1. Thread A reads value (%d)\n" RESET, shared);
    printf(RED "--- CONTEXT SWITCH (Thread A Interrupted) ---\n" RESET);
    printf(CYAN "2. Thread B reads value (%d)\n" RESET, shared);
    printf("3. Thread B increments value to 101 and writes it back.\n");
    shared = 101;
    printf(YELLOW "4. Thread A resumes. It still thinks the value is 100!\n" RESET);
    printf("5. Thread A increments 100 to 101 and writes it back.\n");
    shared = 101;

    printf("\n" RED "FINAL VALUE: %d" RESET " (Should have been 102)\n", shared);
    printf("Explanation: The update from Thread B was overwritten by Thread A.\n");
}

// ==========================================
//      MODULE 7: READER-WRITER PROBLEM
// ==========================================

void displayRWState(int readers, int writer_active, int mutex, int wrt_sem)
{
    system(CLEAR_SCREEN);
    printHeader("READER - WRITER PROBLEM");
    printf(YELLOW "This simulation uses 'Reader Preference' logic.\n" RESET);

    printf("\n" WHITE "   [ SEMAPHORES STATUS ]\n" RESET);
    printf("   Mutex (protects reader count) : %s\n", mutex == 1 ? GREEN "UNLOCKED (1)" RESET : RED "LOCKED (0)" RESET);
    printf("   Wrt (protects shared file)    : %s\n", wrt_sem == 1 ? GREEN "UNLOCKED (1)" RESET : RED "LOCKED (0)" RESET);

    printf("\n" WHITE "   [ CURRENT ACTIVITY ]\n" RESET);
    printf("   Active Readers: " CYAN "%d" RESET "\n", readers);
    printf("   Writer Status : %s\n", writer_active ? RED "WRITING INSIDE CRITICAL SECTION" RESET : GREEN "IDLE" RESET);

    printf("\n" WHITE "   [ VISUALIZATION ]\n" RESET);
    printf("   File/Resource: [ ");
    if (writer_active)
        printf(RED "WRITER WRITING" RESET);
    else if (readers > 0)
    {
        for (int i = 0; i < readers; i++)
            printf(CYAN "R " RESET);
    }
    else
    {
        printf(GREEN "EMPTY" RESET);
    }
    printf(" ]\n");
    printLine(60);
}

void runReaderWriter()
{
    int read_count = 0;
    int mutex = 1;
    int wrt = 1;

    int choice;
    char msg[256] = "System Ready.";

    while (1)
    {
        displayRWState(read_count, (wrt == 0 && read_count == 0), mutex, wrt);
        printf(MAGENTA "LOG: " RESET "%s\n", msg);
        printf("\n" BLUE "Actions:" RESET "\n");
        printf("1. New Reader Tries to Enter\n");
        printf("2. Active Reader Leaves\n");
        printf("3. Writer Tries to Enter\n");
        printf("4. Active Writer Leaves\n");
        printf("5. Back to Main Menu\n");
        printf("Selection: ");
        choice = getSafeInt();

        if (choice == 5)
            break;
        strcpy(msg, "");

        switch (choice)
        {
        case 1:
            if (wrt == 0 && read_count == 0)
            {
                sprintf(msg, RED "BLOCKED:" RESET " Reader cannot enter. Writer holds 'wrt' semaphore.");
            }
            else
            {
                mutex = 0;
                read_count++;
                if (read_count == 1)
                {
                    wrt = 0;
                    sprintf(msg, GREEN "SUCCESS:" RESET " First Reader entered. 'wrt' semaphore LOCKED.");
                }
                else
                {
                    sprintf(msg, GREEN "SUCCESS:" RESET " Reader %d entered. Shared lock held.", read_count);
                }
                mutex = 1;
            }
            break;

        case 2:
            if (read_count == 0)
            {
                sprintf(msg, YELLOW "WARNING:" RESET " No readers are currently inside.");
            }
            else
            {
                mutex = 0;
                read_count--;
                if (read_count == 0)
                {
                    wrt = 1;
                    sprintf(msg, GREEN "LEFT:" RESET " Last Reader left. 'wrt' semaphore UNLOCKED.");
                }
                else
                {
                    sprintf(msg, GREEN "LEFT:" RESET " Reader left. %d remain.", read_count);
                }
                mutex = 1;
            }
            break;

        case 3:
            if (wrt == 0)
            {
                if (read_count > 0)
                    sprintf(msg, RED "BLOCKED:" RESET " Writer cannot enter. Readers hold the lock.");
                else
                    sprintf(msg, RED "BLOCKED:" RESET " Writer cannot enter. Another Writer is active.");
            }
            else
            {
                wrt = 0;
                sprintf(msg, GREEN "SUCCESS:" RESET " Writer Entered. 'wrt' LOCKED.");
            }
            break;

        case 4:
            if (wrt == 1)
            {
                sprintf(msg, YELLOW "WARNING:" RESET " No writer is currently writing.");
            }
            else if (read_count > 0)
            {
                sprintf(msg, RED "ERROR:" RESET " Invalid State (Readers present while WRT held).");
            }
            else
            {
                wrt = 1;
                sprintf(msg, GREEN "LEFT:" RESET " Writer left. 'wrt' semaphore RELEASED.");
            }
            break;
        default:
            sprintf(msg, "Invalid Selection.");
        }
    }
}

// ==========================================
//      MODULE 8: DINING PHILOSOPHERS
// ==========================================

#define THINKING 0
#define HUNGRY 1
#define EATING 2

int chopstick[5];
int p_state[5];
int held_sticks[5];

void initDining()
{
    for (int i = 0; i < 5; i++)
    {
        chopstick[i] = 1;
        p_state[i] = THINKING;
        held_sticks[i] = 0;
    }
}

void displayDiningTable()
{
    system(CLEAR_SCREEN);
    printHeader("DINING PHILOSOPHERS & DEADLOCK VISUALIZER");

    printf(YELLOW "Instructions:" RESET " Manually control philosophers to understand Deadlock.\n");
    printf("Create Deadlock: Make EVERY philosopher pick up their LEFT chopstick.\n\n");

    printf("      (P0)       \n");
    printf("     /    \\     \n");
    printf("  (P4)    (P1)   \n");
    printf("    \\      /    \n");
    printf("   (P3)--(P2)    \n\n");

    printLine(60);
    printf("| %-3s | %-10s | %-12s | %-15s |\n", "ID", "State", "Left Stick", "Right Stick");
    printLine(60);

    for (int i = 0; i < 5; i++)
    {
        int left = i;
        int right = (i + 1) % 5;

        // FIXED BUFFER SIZES TO 50 TO PREVENT OVERFLOW
        char lStatus[50], rStatus[50];
        char stateStr[50];

        if (chopstick[left] == 1)
            strcpy(lStatus, GREEN "Free" RESET);
        else if (held_sticks[i] & 1)
            strcpy(lStatus, GREEN "HELD (By You)" RESET);
        else
            strcpy(lStatus, RED "BUSY" RESET);

        if (chopstick[right] == 1)
            strcpy(rStatus, GREEN "Free" RESET);
        else if (held_sticks[i] & 2)
            strcpy(rStatus, GREEN "HELD (By You)" RESET);
        else
            strcpy(rStatus, RED "BUSY" RESET);

        if (p_state[i] == THINKING)
            strcpy(stateStr, "THINKING");
        else if (p_state[i] == HUNGRY)
            strcpy(stateStr, YELLOW "HUNGRY" RESET);
        else
            strcpy(stateStr, GREEN "EATING" RESET);

        printf("| P%d  | %-18s | %-21s | %-24s |\n", i, stateStr, lStatus, rStatus);
    }
    printLine(60);
}

void runDiningPhilosophers()
{
    initDining();
    int choice, p_id;
    char msg[256] = "Welcome to the Dining Hall.";

    while (1)
    {
        displayDiningTable();
        printf(MAGENTA "LOG: " RESET "%s\n", msg);

        printf("\n" BLUE "Select Philosopher (0-4) or 5 to Exit: " RESET);
        p_id = getSafeInt();
        if (p_id == 5)
            break;
        if (p_id < 0 || p_id > 4)
        {
            strcpy(msg, "Invalid Philosopher ID");
            continue;
        }

        printf("\n" CYAN "Action for P%d:" RESET "\n", p_id);
        printf("1. Pick Up LEFT Chopstick (%d)\n", p_id);
        printf("2. Pick Up RIGHT Chopstick (%d)\n", (p_id + 1) % 5);
        printf("3. Put Down BOTH (Stop Eating)\n");
        printf("4. Cancel\n");
        printf("Selection: ");
        choice = getSafeInt();

        int left = p_id;
        int right = (p_id + 1) % 5;

        switch (choice)
        {
        case 1:
            if (chopstick[left] == 0)
            {
                sprintf(msg, RED "FAILURE:" RESET " Left stick is held by P%d.", (p_id + 4) % 5);
            }
            else if (held_sticks[p_id] & 1)
            {
                sprintf(msg, YELLOW "INFO:" RESET " You already hold the left stick.");
            }
            else
            {
                chopstick[left] = 0;
                held_sticks[p_id] |= 1;
                p_state[p_id] = HUNGRY;
                sprintf(msg, GREEN "SUCCESS:" RESET " P%d picked up Left stick.", p_id);
                if (held_sticks[p_id] == 3)
                    p_state[p_id] = EATING;
            }
            break;

        case 2:
            if (chopstick[right] == 0)
            {
                sprintf(msg, RED "FAILURE:" RESET " Right stick is held by P%d.", (p_id + 1) % 5);
            }
            else if (held_sticks[p_id] & 2)
            {
                sprintf(msg, YELLOW "INFO:" RESET " You already hold the right stick.");
            }
            else
            {
                chopstick[right] = 0;
                held_sticks[p_id] |= 2;
                p_state[p_id] = HUNGRY;
                sprintf(msg, GREEN "SUCCESS:" RESET " P%d picked up Right stick.", p_id);
                if (held_sticks[p_id] == 3)
                    p_state[p_id] = EATING;
            }
            break;

        case 3:
            if (held_sticks[p_id] == 0)
            {
                sprintf(msg, YELLOW "INFO:" RESET " P%d holds nothing.", p_id);
            }
            else
            {
                if (held_sticks[p_id] & 1)
                    chopstick[left] = 1;
                if (held_sticks[p_id] & 2)
                    chopstick[right] = 1;
                held_sticks[p_id] = 0;
                p_state[p_id] = THINKING;
                sprintf(msg, GREEN "SUCCESS:" RESET " P%d dropped sticks and is Thinking.", p_id);
            }
            break;
        case 4:
            strcpy(msg, "Action Cancelled.");
            break;
        default:
            strcpy(msg, "Invalid Action.");
        }

        // Logic to detect DEADLOCK (All sticks held, no one eating)
        int sticks_held = 0;
        for (int i = 0; i < 5; i++)
            if (chopstick[i] == 0)
                sticks_held++;

        if (sticks_held == 5)
        {
            bool anyone_eating = false;
            for (int i = 0; i < 5; i++)
                if (p_state[i] == EATING)
                    anyone_eating = true;

            if (!anyone_eating)
            {
                strcat(msg, RED " [DEADLOCK DETECTED! Everyone waiting]" RESET);
            }
        }
    }
}

// ==========================================
//      MAIN MENU
// ==========================================

int main()
{
    int choice;
    while (1)
    {
        system(CLEAR_SCREEN);
        printHeader("ULTIMATE OS SIMULATOR");
        printf(YELLOW "1." RESET " CPU Scheduling\n");
        printf(YELLOW "2." RESET " Memory Allocation\n");
        printf(YELLOW "3." RESET " Deadlock Avoidance (Banker's)\n");
        printf(YELLOW "4." RESET " Disk Scheduling Visualizer\n");
        printf(YELLOW "5." RESET " Page Replacement (Virtual Memory)\n");
        printf(YELLOW "6." RESET " Race Condition Demo (Concurrency)\n");
        printf(YELLOW "7." RESET " Process Sync (Reader-Writer)\n");
        printf(YELLOW "8." RESET " Deadlock Sim (Dining Philosophers)\n");
        printf(YELLOW "9." RESET " Exit\n");
        printf(CYAN "\nSelect Option: " RESET);
        choice = getSafeInt();

        switch (choice)
        {
        case 1:
        {
            int n;
            Process original[100], working[100];
            printHeader("CPU SCHEDULING SETUP");
            printf("Enter number of processes: ");
            n = getSafeInt();
            for (int i = 0; i < n; i++)
            {
                original[i].id = i + 1;
                printf("P%d (Burst, Arrival, Priority): ", i + 1);
                original[i].bt = getSafeInt();
                original[i].at = getSafeInt();
                original[i].pr = getSafeInt();
            }

            while (1)
            {
                for (int i = 0; i < n; i++)
                    working[i] = original[i];
                int algo, mode = 1;

                printf("\n" BLUE "Select Algorithm:" RESET "\n");
                printf("1. FCFS\n2. SJF\n3. SRTF\n4. Round Robin\n5. Back\n");
                printf("Selection: ");
                algo = getSafeInt();

                if (algo == 5)
                    break;

                if (algo == 3)
                {
                    printf("\nMode: 1. Standard  2. " MAGENTA "Interactive" RESET "\nAction: ");
                    mode = getSafeInt();
                }

                if (mode == 2 && algo == 3)
                    runInteractiveSRTF(working, n);
                else
                {
                    if (algo == 1)
                        runFCFS(working, n);
                    else if (algo == 2)
                        runSJF(working, n);
                    else if (algo == 3)
                        runStandardSRTF(working, n);
                    else if (algo == 4)
                    {
                        int q;
                        printf("Enter Time Quantum: ");
                        q = getSafeInt();
                        runRoundRobin(working, n, q);
                    }
                }
                printf(GREEN "\nRun another? (1=Yes, 0=No): " RESET);
                int c = getSafeInt();
                if (!c)
                    break;
            }
            break;
        }
        case 2:
            runMemoryAllocation();
            break;
        case 3:
            runBankersAlgorithm();
            break;
        case 4:
            runDiskScheduler();
            break;
        case 5:
            runPageReplacement();
            break;
        case 6:
            slowPrint(YELLOW "Initializing Race Condition Arena...\n" RESET, 50);
            SLEEP_MS(1000);
            runRaceCondition();
            waitForStudent();
            break;
        case 7:
            runReaderWriter();
            break;
        case 8:
            runDiningPhilosophers();
            break;
        default:
            printf("Module under construction...\n");
            SLEEP_MS(1000);
            printf(RED "Invalid Choice.\n" RESET);
        }
    }
    return 0;
}