// ==========================================
//      ULTIMATE OS SIMULATOR: MAIN
// ==========================================

#include "utils.h"
#include "cpu_scheduling.h"
#include "memory_Allocation.h"
#include "bankers_algo.h"
#include "disk_scheduler.h"
#include "page_replacement.h"
#include "race_condition.h"
#include "reader_writer.h"
#include "dining_philosophers.h"

int main()
{
    int choice;
    while (1)
    {
        system(CLEAR_SCREEN);
        printHeader("ULTIMATE OS SIMULATOR");

        printf(YELLOW "1." RESET " CPU Scheduling (FCFS, SJF, SRTF, RR)\n");
        printf(YELLOW "2." RESET " Memory Allocation (Best, First, Worst Fit)\n");
        printf(YELLOW "3." RESET " Deadlock Avoidance (Banker's Algorithm)\n");
        printf(YELLOW "4." RESET " Disk Scheduling Visualizer (FCFS)\n");
        printf(YELLOW "5." RESET " Page Replacement (FIFO Virtual Memory)\n");
        printf(YELLOW "6." RESET " Race Condition Demo (Concurrency Error)\n");
        printf(YELLOW "7." RESET " Process Sync (Reader-Writer Problem)\n");
        printf(YELLOW "8." RESET " Deadlock Simulation (Dining Philosophers)\n");
        printf(YELLOW "9." RESET " Exit Simulator\n");

        printf(CYAN "\nSelect Module: " RESET);
        choice = getSafeInt();

        if (choice == 9)
        {
            printf(GREEN "\nShutting down simulator... Goodbye!\n" RESET);
            break;
        }

        switch (choice)
        {
        case 1:
        {
            // --- CPU Scheduling Sub-Menu ---
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
                        printf("Enter Time Quantum: ");
                        int q = getSafeInt();
                        runRoundRobin(working, n, q);
                    }
                }
                printf(GREEN "\nRun another algorithm with same data? (1=Yes, 0=No): " RESET);
                if (!getSafeInt())
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
            break;
        case 7:
            runReaderWriter();
            break;
        case 8:
            runDiningPhilosophers();
            break;
        default:
            printf(RED "Invalid Choice. Try again.\n" RESET);
            SLEEP_MS(1000);
        }
    }
    return 0;
}