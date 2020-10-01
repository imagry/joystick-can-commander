//
// Created by imagry on 26/05/18.
//

#ifndef JOYSTICK_SIMPLECARCONTROLLER_H
#define JOYSTICK_SIMPLECARCONTROLLER_H

#include "common.h"

class Commander {

private:
    static void *execute(void * _);

public:
    void init();
    void enable_controls();
    void disable_controls();
    void set_values(int steering, int throttle, int brake, int turn_indicator);
};


#endif //JOYSTICK_SIMPLECARCONTROLLER_H
