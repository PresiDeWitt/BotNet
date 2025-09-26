#ifndef UTILS_H
#define UTILS_H

#include "../structures.h"

// Utilidades generales
uint32_t parse_ip(const char *ip_str);
char* ip_to_string(uint32_t ip);
int create_pid_file();
void daemonize();
int drop_privileges();

// Logging
void log_message(int level, const char *format, ...);
#define LOG_ERROR 0
#define LOG_WARNING 1
#define LOG_INFO 2
#define LOG_DEBUG 3

// Tiempo
uint64_t get_timestamp();
void precise_sleep(int milliseconds);

// Seguridad
void secure_zero(void *ptr, size_t len);
int check_root_privileges();

#endif