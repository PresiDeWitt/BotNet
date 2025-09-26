#ifndef ATTACK_H
#define ATTACK_H

#include "../structures.h"

// A flag to indicate if an attack is currently running
extern volatile int attack_running;

// Starts an attack
void attack_start(struct attack *attack);

// Stops all running attacks
void attack_stop_all();

#endif // ATTACK_H
