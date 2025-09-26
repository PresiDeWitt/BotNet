#ifndef ANTI_ANALYSIS_H
#define ANTI_ANALYSIS_H

// Técnicas de detección de análisis
typedef struct {
    int check_uptime;
    int check_memory;
    int check_cpus;
    int check_processes;
    int check_network;
} anti_analysis_config_t;

// Funciones anti-análisis
int check_environment();
int check_debugger();
int check_virtualization();
int check_sandbox_signatures();
int check_analysis_tools();

// Contramedidas
void evade_analysis();
void fake_system_calls();
void generate_decoy_traffic();

#endif