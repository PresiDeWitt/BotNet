#ifndef DGA_H
#define DGA_H

#define MAX_DGA_DOMAINS 10

// Generates a list of domains for the current day into the provided array.
// Returns the number of domains generated.
int dga_generate_domains(char domains[MAX_DGA_DOMAINS][256]);

#endif // DGA_H
