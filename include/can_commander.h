//
// Created by imagry on 26/05/18.
//

#ifndef CAN_COMMANDER_H
#define CAN_COMMANDER_H

#include "common.h"
const int INTERVAL_BETWEEN_PID_REQ_US = 15000;

class CANCommander {

public:
    void init();
    void enable_controls();
    void disable_controls();
    void send_data(float steering, uint16_t throttle, uint16_t brake, int turn_indicator);
};


#endif // CAN_COMMANDER_H
