# Mirai Botnet Framework

**WARNING: This code is provided for educational and research purposes only. The use of this software for any illegal activity is strictly prohibited. The authors assume no liability for misuse of this information.**

## Description

This project is a modular botnet framework, inspired by the concepts of the Mirai malware. It is designed as an educational tool for security researchers, students, and developers to study botnet architecture, propagation techniques, and mitigation strategies in a controlled environment.

The framework implements a client-server architecture where bots connect to a Command and Control (C2) server to receive commands.

## Features

- **Modular Architecture**: The code is organized into modules that encapsulate different functionalities (core, attack, communication, infection, stealth, and utils).
- **Multiple Attack Vectors**: Implements various Denial of Service (DoS) attacks, such as TCP and UDP floods.
- **Encrypted Communication**: Communication between the bot and the C2 server is encrypted with AES to prevent command interception.
- **Domain Generation Algorithm (DGA)**: Uses DGA to dynamically generate C2 domains, making it harder to block via DNS.
- **Stealth Capabilities**: Includes basic anti-analysis techniques to detect debuggers and virtual machines.
- **Multi-threaded Propagation**: Uses multiple threads to efficiently scan and propagate to new targets.

## Compilation

The project uses CMake to manage the build process. To compile the bot, follow these steps:

```bash
# 1. Create a build directory
mkdir build
cd build

# 2. Generate the build files with CMake
cmake ..

# 3. Compile the project
make
```

The final executable will be named `Mirai` and will be located in the `build` directory.

## Disclaimer

It is reiterated that this project is for educational purposes only. The creation, distribution, or use of malware is illegal. Use this code responsibly and only on systems you own or for which you have explicit permission to analyze.
