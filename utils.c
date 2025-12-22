#include "utils.h"

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
