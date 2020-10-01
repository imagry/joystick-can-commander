#include <thread>
#include "can_monitor.h"
#include <canlib.h>
#include <string.h>

#define STEERING_ID (0x2B0)
#define SPEED_ID (0x371)

const int ITERVAL_BETWEEN_PID_REQ_US = 15000;

bool can_monitor_init_success = true;

float steering = 0;
float speed = 0;
bool control_enabled = false;

float steering_formula(const uint8_t *data) ;
float speed_formula(const uint8_t *data) ;
pthread_t can_thread;

canHandle hnd;

void CanMonitor::init() {
    canStatus stat;
    int chanCount = 0;
    stat = canGetNumberOfChannels(&chanCount);
    if (stat != canOK || chanCount == 0) {
        can_monitor_init_success = false;
        printf("Failed to find can channel\n");
        exit(1);
    }

    hnd = canOpenChannel(0, 0);
    if (hnd < 0) {
        printf("Failed to open can channel\n");
        can_monitor_init_success = false;
        exit(1);
    }

    stat = canSetBusParams(hnd, canBITRATE_500K, 0, 0, 0, 0, 0);
    if (stat != canOK) {
        printf("Failed to set can params\n");
        can_monitor_init_success = false;
        exit(1);
    }

    stat = canBusOn(hnd);
    if (stat != canOK) {
        printf("Failed to set can bus on\n");
        can_monitor_init_success = false;
        exit(1);
    }

    if (pthread_create(&can_thread, NULL, execute, NULL)) {
        can_monitor_init_success = false;
        exit(1);
    }
}


void * CanMonitor::execute(void *_) {
    long id;
    unsigned char msg[8];
    unsigned int dlc;
    unsigned int flag;
    unsigned long time_stamp;

    while (can_monitor_init_success) {

        if(canReadWait (hnd, &id, &msg, &dlc, &flag, &time_stamp, ITERVAL_BETWEEN_PID_REQ_US) == canOK) {

            if (id == STEERING_ID) {
                steering = steering_formula(msg);
            }
            else if (id == SPEED_ID) {
                speed = speed_formula(msg);
            }
            else if (id == 0x010) {
                control_enabled = (msg[0] & 0x07) > 0;
            }
            else if (id == 0x99) {
                control_enabled = false;
            }
        }
        else {
            printf("Failed to read can frame\n");
            break;
        }
    }
}

float CanMonitor::get_speed() {
    return speed;
}

float CanMonitor::get_steering() {
    return steering;
}

bool CanMonitor::is_control_enabled() {
    return control_enabled;
}

float steering_formula(const uint8_t *data) {
    return ((short)((data[1] << 8) + data[0])) / 10.0;
}

float speed_formula(const uint8_t *data) {
    return data[4] * 1.15;
}

