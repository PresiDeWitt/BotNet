#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ¡LA SOLUCIÓN! Incluimos directamente la definición de la estructura.
#include "../../include/structures.h"

int parse_config_file(const char *filename, botnet_config_t *config) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;

    char line[256];
    char section[32] = "";

    while (fgets(line, sizeof(line), file)) {
        // Eliminar newline
        line[strcspn(line, "\n")] = 0;

        // Saltar líneas vacías y comentarios
        if (line[0] == '#' || line[0] == '\0') continue;

        // Sección
        if (line[0] == '[' && line[strlen(line)-1] == ']') {
            strncpy(section, line + 1, strlen(line) - 2);
            section[strlen(line) - 2] = '\0';
            continue;
        }

        // Clave=Valor
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");

        if (key && value) {
            // Eliminar espacios
            while (*key == ' ') key++;
            while (*value == ' ') value++;

            if (strcmp(section, "network") == 0) {
                if (strcmp(key, "c2_port") == 0) config->c2_port = atoi(value);
                else if (strcmp(key, "p2p_port") == 0) config->p2p_port = atoi(value);
            }
            else if (strcmp(section, "attack") == 0) {
                if (strcmp(key, "max_threads") == 0) config->max_threads = atoi(value);
            }
        }
    }

    fclose(file);
    return 0;
}
