//
// Created by imagry on 26/05/18.
//

#include <thread>
#include "commander.h"
#include "common.h"
#include "can_monitor.h"
#include "can_commander.h"
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <can_commander.h>

#define UPDATE_INTERVAL_MICRO (10000)

static int steering_angle = 0;
static int throttle = 0;
static int brake = 0;
static int turn_indicator = 0;

pthread_t comm_thread;

CANCommander can_commander;

void Commander::init() {
    can_commander.init();

    if (pthread_create(&comm_thread, NULL, execute, NULL)) {
        printf("Failed to init thread\n");
        exit(1);
    }

    printf("Commander initialized\n");
}

void* Commander::execute(void *_) {
    while (true) {

        uint16_t brake_u, throttle_u;

        brake_u = (uint16_t)brake;
        throttle_u = (uint16_t)throttle;

        can_commander.send_data(steering_angle, throttle_u, brake_u, turn_indicator);

        usleep(UPDATE_INTERVAL_MICRO);
    }
}

void Commander::enable_controls() {
    printf("Enabling Control...\n");
    can_commander.enable_controls();
}

void Commander::disable_controls() {
    printf("Disabling Control...\n");
    can_commander.disable_controls();
}

void Commander::set_values(int _steering, int _throttle, int _brake, int _turn_indicator) {
    steering_angle = _steering;
    throttle = _throttle;
    brake = _brake;
    turn_indicator = _turn_indicator;
}
