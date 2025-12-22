
#include "disk_scheduler.h"

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