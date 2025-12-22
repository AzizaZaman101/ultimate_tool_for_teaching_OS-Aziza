#include "race_condition.h"
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