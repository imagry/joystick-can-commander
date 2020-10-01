#include <iostream>
#include "joystick.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_gamecontroller.h>
#include "common.h"
#include "can_monitor.h"
#include "commander.h"

#define JOYSTICK_UPDATE_INTERVAL_MICRO (10000)
#define JOYSTICK_AXIS_THROTTLE (SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
#define JOYSTICK_AXIS_BRAKE (SDL_CONTROLLER_AXIS_TRIGGERLEFT)
#define JOYSTICK_AXIS_STEER (SDL_CONTROLLER_AXIS_LEFTX)
#define JOYSTICK_BUTTON_EXIT (SDL_CONTROLLER_BUTTON_X)
#define JOYSTICK_BUTTON_TURN_RIGHT (SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
#define JOYSTICK_BUTTON_TURN_LEFT (SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
#define JOYSTICK_BUTTON_DISABLE_CONTROLS (SDL_CONTROLLER_BUTTON_BACK)
#define JOYSTICK_BUTTON_ENABLE_CONTROLS (SDL_CONTROLLER_BUTTON_START)

using namespace std;

static int steering_angel = 0;
static int throttle = 0;
static int brake = 0;
static int turn_indicator = 0;

void capture_joystick();

bool get_button(unsigned long button_id);
double get_axis_normalized_position(unsigned long axis_index);
void update_steer_angle();
void update_brake();
void update_throttle();
void update_turn_indicator();
void enable_control();
void disable_control();
void send_updates();
void init();

Commander commander;

int main() {
    init();
    capture_joystick();

    return 0;
}

void init() {
    if (joystick_init( ) != 0) {
        printf("Failed to init joystick\n");
        exit(1);
    }

    commander.init();
    canMonitor.init();

    printf("init succeeded\n");
}

void capture_joystick()
{
    printf("Capturing joystick...\n");
    while (true)
    {
        int return_code = -1;

        return_code = joystick_update( );
        if (return_code == -1)
        {
            printf( "Failed to capture joystick\n");
            exit(1);
        }

        if (get_button(JOYSTICK_BUTTON_EXIT) == 1) {
            exit(0);
        }

        if (get_button(JOYSTICK_BUTTON_DISABLE_CONTROLS) == 1) {
            disable_control();
        }

        if (get_button(JOYSTICK_BUTTON_ENABLE_CONTROLS) == 1) {
            enable_control();
        }

        update_steer_angle();
	    update_turn_indicator();
        update_brake();
        update_throttle();
        send_updates();

        // Restrict rate
        usleep(JOYSTICK_UPDATE_INTERVAL_MICRO);
    }
}

static double steering_average = 0.0;

void update_steer_angle() {
    double normalized_position = get_axis_normalized_position(JOYSTICK_AXIS_STEER);

    steering_average = CALC_EXPONENTIAL(
            steering_average,
            normalized_position * -1,
            STEERING_FILTER_FACTOR);

    steering_angel = (int) (steering_average * 3500);
}

static double throttle_average = 0.0;

void update_throttle() {
    double normalized_position = get_axis_normalized_position(JOYSTICK_AXIS_THROTTLE);

    throttle_average = CALC_EXPONENTIAL(
            throttle_average,
            normalized_position,
            THROTTLE_FILTER_FACTOR);

    throttle = (int) RANGE_PERCENTAGE(throttle_average, THROTTLE_MIN, THROTTLE_MAX);
}

static double brake_average = 0.0;

void update_brake() {
    double normalized_position = get_axis_normalized_position(JOYSTICK_AXIS_BRAKE);

    brake_average = CALC_EXPONENTIAL(
            brake_average,
            normalized_position,
            BRAKE_FILTER_FACTOR);

    brake = (int) RANGE_PERCENTAGE(brake_average, BRAKE_MIN, BRAKE_MAX);
}

void update_turn_indicator() {
    if (get_button(JOYSTICK_BUTTON_TURN_RIGHT)) {
        if (turn_indicator != 1) {
            turn_indicator = 1;
        }
        else {
            turn_indicator = 0;
        }
    }
    else if (get_button(JOYSTICK_BUTTON_TURN_LEFT)) {
        if (turn_indicator != 2) {
            turn_indicator = 2;
        }
        else {
            turn_indicator = 0;
        }
    }
}

int buttons_state[100];

bool get_button(unsigned long button_id) {
    unsigned int button_state = 0;
    int ret = joystick_get_button( button_id, &button_state );

    if (ret == -1) {
        printf("Failed to capture joystick button");
        exit(1);
    }

    if (button_state == 0 && buttons_state[button_id] == 1) {
        buttons_state[button_id] = button_state;
        return true;
    }

    buttons_state[button_id] = button_state;
    return false;
}

double get_axis_normalized_position(unsigned long axis_index) {
    int axis_position = 0;

    static const float deadzone = 0.3;

    int ret = joystick_get_axis( axis_index, &axis_position );

    if (ret == -1) {
        printf("Failed to capture joystick button");
        exit(1);
    }

    double raw_normalized_position = ((double) axis_position / INT16_MAX);

    if ( axis_index == JOYSTICK_AXIS_STEER )
    {
        // if axis is in deadzone, do nothing
        if ( fabs(raw_normalized_position) < deadzone)
        {
            return  0.0;
        }

        // normalize over non deadzone range
        raw_normalized_position *= (fabs(raw_normalized_position) - deadzone) / (1.0 - deadzone);
        return CONSTRAIN(raw_normalized_position, -1.0, 1.0);
    }

    return CONSTRAIN(raw_normalized_position, 0.0, 1.0);
}

void enable_control() {
    commander.enable_controls();
}

void disable_control() {
    commander.disable_controls();
}

void send_updates() {
    commander.set_values(steering_angel, throttle, brake, turn_indicator);
}
