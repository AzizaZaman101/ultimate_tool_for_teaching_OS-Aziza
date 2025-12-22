#ifndef DINING_PHILOSOPHERS_H
#define DINING_PHILOSOPHERS_H

#include "utils.h"

// --- Constants ---
#define THINKING 0
#define HUNGRY 1
#define EATING 2

// --- Function Prototypes ---
void initDining();
void displayDiningTable();
void runDiningPhilosophers();

#endif