#ifndef REPORTING_H
#define REPORTING_H

// Initializes the status reporting module and starts the reporting thread.
void reporting_init();

// Increments the counter for successful infections.
void increment_infection_counter();

#endif // REPORTING_H
