//
// Created by imagry on 22/05/18.
//

#ifndef JOYSTICK_COMMANDER_COMMON_H
#define JOYSTICK_COMMANDER_COMMON_H

#if defined (__cplusplus)
extern "C" {
#endif

#define CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define CALC_EXPONENTIAL(avg, setpoint, factor) (( setpoint * factor ) + ( ( 1.0 - factor ) * avg ))
#define RANGE_PERCENTAGE(percentage, min, max) (min + (percentage * (max - min)))

#define BRAKE_MIN (0)
#define BRAKE_MAX (65535)

#define THROTTLE_MIN (0)
#define THROTTLE_MAX (65535)

#define ENABLE_THROTTLE (1)
#define ENABLE_BRAKE (1)
#define ENABLE_STEERING (1)

#define STEERING_FILTER_FACTOR (0.05)
#define BRAKE_FILTER_FACTOR (0.04)
#define THROTTLE_FILTER_FACTOR (0.08)

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>


#if defined (__cplusplus)
}
#endif

#endif //JOYSTICK_COMMANDER_COMMON_H
