#include "page_replacement.h"
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