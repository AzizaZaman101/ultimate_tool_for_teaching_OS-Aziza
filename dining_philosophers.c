#include "dining_philosophers.h"

// ==========================================
//      MODULE 8: DINING PHILOSOPHERS
// ==========================================

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