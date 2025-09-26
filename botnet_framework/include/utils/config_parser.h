#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdint.h>

// Estructura para mantener la configuración cargada desde un archivo
typedef struct {
    uint16_t c2_port;
    uint16_t p2p_port;
    int max_threads;
} botnet_config_t;

// Prototipo de la función de análisis
int parse_config_file(const char *filename, botnet_config_t *config);

#endif // CONFIG_PARSER_H
