#include "memory_Allocation.h"
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