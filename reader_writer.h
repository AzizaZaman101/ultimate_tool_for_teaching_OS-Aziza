#ifndef READER_WRITER_H
#define READER_WRITER_H

#include "utils.h"

// --- Function Prototypes ---
/**
 * Displays the current state of semaphores, active readers, and writers.
 */
void displayRWState(int readers, int writer_active, int mutex, int wrt_sem);

/**
 * Runs the interactive Reader-Writer simulation (Reader Preference).
 */
void runReaderWriter();

#endif