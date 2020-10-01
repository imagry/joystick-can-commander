
#ifndef CAN_MONITOR_H
#define CAN_MONITOR_H

class CanMonitor {

private:
    static void *execute(void * _);


public:
    void init();
    float get_steering();
    float get_speed();
    bool is_control_enabled();
};

static CanMonitor canMonitor;

#endif //CAN_MONITOR_H
