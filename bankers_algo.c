#include "bankers_algo.h"
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