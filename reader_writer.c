#include "reader_writer.h"

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
