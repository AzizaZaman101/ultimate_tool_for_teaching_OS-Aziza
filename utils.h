#ifndef UTILS_H
#define UTILS_H

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

// Global Arrays for Simulations, using extern to declare them here
extern int requests[MAX_REQ];
extern int pages[MAX_REQ];
extern int frames[MAX_FRAMES];

// --- Function Prototypes ---
void slowPrint(const char *text, int delay_ms);
void waitForStudent();
void clearBuffer();
int getSafeInt();
void waitForInput();
void printLine(int width);
void printHeader(const char *title);
int countDigits(int n);

#endif