//
// Created by imagry on 26/05/18.
//

#include "common.h"
#include "can_commander.h"
#include <canlib.h>
#include <math.h>
#include <stdio.h>

void init_can();
bool send_can_frame(int id, int dlc, char* data);

canHandle handle;

void CANCommander::init() {
    init_can();
    printf("CAN Commander initialized\n");
}

void init_can() {
    canStatus stat;

    int chanCount = 0;
    stat = canGetNumberOfChannels(&chanCount);
    if (stat != canOK || chanCount == 0) {
        printf("Failed to find can channel\n");
        exit(-1);
    }

    handle = canOpenChannel(0, 0);
    if (handle < 0) {
        printf("Failed to open can channel\n");
        exit(-1);
    }

    stat = canSetBusParams(handle, canBITRATE_500K, 0, 0, 0, 0, 0);
    if (stat != canOK) {
        printf("Failed to set can params\n");
        exit(-1);
    }

    stat = canBusOn(handle);
    if (stat != canOK) {
        printf("Failed to set can bus on\n");
        exit(-1);
    }
}

void CANCommander::send_data(float steering, uint16_t throttle, uint16_t brake, int turn_indicator) {
    int id = 0x011;
    int dlc = 7;

    char data[8];
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;

    data[0] = ((int)(steering)) & 0xFF;
    data[1] = ((int)(steering)) >> 8 ;

    *((uint16_t*)&data[2]) = brake;
    *((uint16_t*)&data[4]) = throttle;

    data[6] = (uint8_t)turn_indicator;

    send_can_frame(id, dlc, data);
}

void CANCommander::enable_controls() {
    int id = 0x010;
    int dlc = 1;

    char data[8];
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;

    if (ENABLE_STEERING == 1) {
        data[0] |= 0x1;
    }

    if (ENABLE_THROTTLE == 1) {
        data[0] |= 0x2;
    }

    if (ENABLE_BRAKE == 1) {
        data[0] |= 0x4;
    }

    send_can_frame(id, dlc, data);
}

void CANCommander::disable_controls() {
    int id = 0x010;
    int dlc = 1;

    char data[8];
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;
    send_can_frame(id, dlc, data);
}

bool send_can_frame(int id, int dlc, char* data) {
    canStatus stat = canWriteWait(handle, id, data, dlc, 0, INTERVAL_BETWEEN_PID_REQ_US/1000);
    return stat == canOK;
}






