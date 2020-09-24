#ifndef MB_CONTROLLER_H
#define MB_CONTROLLER_H


#include "../mobilebot/mobilebot.h"
#define CFG_PATH_PID "/home/debian/mobilebot/bin/config/pid.cfg"
#define CFG_PATH_LPF "/home/debian/mobilebot/bin/config/lpf.cfg"

int mb_initialize_controller();
int mb_load_controller_config();
int mb_controller_update(mb_state_t* mb_state, mb_setpoints_t* mb_setpoints);
int mb_destroy_controller();
float mb_get_wheel_speed(float encoder_delta, float dt);
/************
* Add your PID and other SISO filters here.
* examples:
* rc_filter_t left_wheel_speed_pid;
* rc_filter_t fwd_vel_sp_lpf;
*************/
rc_filter_t left_wheel_speed_pid;
rc_filter_t right_wheel_speed_pid;
rc_filter_t fwd_vel_sp_lpf;
/***********
* For each PID filter you want to load from settings
* add a pid_parameter_t or filter_parameter_t
* example:
* pid_parameters_t left_wheel_speed_params;
* filter_parameters_t fwd_vel_sp_lpf_params;
************/
pid_parameters_t pid_params;
filter_parameters_t fwd_vel_sp_lpf_params;
#endif

