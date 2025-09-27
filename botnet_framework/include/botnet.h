#ifndef BOTNET_H
#define BOTNET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>

// Linux-specific include for prctl to fix compilation error
#ifdef __linux__
#include <sys/prctl.h>
#endif

// Core headers
#include "config.h"
#include "structures.h"
#include "core/core.h"
#include "core/controller.h"

// Attack headers
#include "attack/attack.h"
#include "attack/vectors.h"

// Communication headers
#include "communication/communication.h"

// Infection headers
#include "infection/infection.h"

// Stealth headers
#include "stealth/stealth.h"

// Utility headers
#include "utils/utils.h"
#include "utils/network_utils.h"

#endif // BOTNET_H
