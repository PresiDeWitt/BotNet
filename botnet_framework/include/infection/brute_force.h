#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include "../structures.h"

// Initializes the brute force module and starts its worker threads
void brute_force_init();

// Adds a new target to the brute force queue (called by the scanner)
void brute_force_add_target(struct target *t);

#endif // BRUTE_FORCE_H
