#include "../mobilebot/mobilebot.h"

/*******************************************************************************
* int mb_initialize()
*
* this initializes all the PID controllers from the configuration file
* you can use this as is or modify it if you want a different format
*
* return 0 on success
*
*******************************************************************************/

int mb_initialize_controller(){
    mb_load_controller_config();
    left_wheel_speed_pid = rc_filter_empty();
    right_wheel_speed_pid = rc_filter_empty();
    fwd_vel_sp_lpf = rc_filter_empty();
    rc_filter_pid(&left_wheel_speed_pid, pid_params.kp, pid_params.ki, pid_params.kd, pid_params.dFilterHz, DT);
    rc_filter_pid(&right_wheel_speed_pid, pid_params.kp, pid_params.ki, pid_params.kd, pid_params.dFilterHz, DT);
    rc_filter_first_order_lowpass(&fwd_vel_sp_lpf, DT, fwd_vel_sp_lpf_params.time_constant);
    return 0;
}

/*******************************************************************************
* int mb_load_controller_config()
*
* this provides a basic configuration load routine
* you can use this as is or modify it if you want a different format
*
* return 0 on success
*
*******************************************************************************/


int mb_load_controller_config(){

    FILE* file = fopen(CFG_PATH_PID, "r");
    if (file == NULL){
        fprintf(stderr,"Error opening pid.cfg\n");
    }

    fscanf(file, "%f %f %f %f", 
        &pid_params.kp,
        &pid_params.ki,
        &pid_params.kd,
        &pid_params.dFilterHz
        );

    fclose(file);

    FILE* file2 = fopen(CFG_PATH_LPF, "r");
    if (file2 == NULL){
        fprintf(stderr,"Error opening lpf.cfg\n");
    }

    fscanf(file2, "%f", 
        &fwd_vel_sp_lpf_params.time_constant
        );

    fclose(file2);

    return 0;
}

/*******************************************************************************
* int mb_controller_update()
* 
* TODO: Write your PID controller here
* take inputs from the global mb_state
* write outputs to the global mb_state
*
* return 0 on success
*
*******************************************************************************/

int mb_controller_update(mb_state_t* mb_state, mb_setpoints_t* mb_setpoints){
    
    // Filter setpoint to avoid high frequency content of step-like inputs
    float fwd_vel_filt = rc_filter_march(&fwd_vel_sp_lpf, mb_setpoints->fwd_velocity);
    fwd_vel_filt = mb_setpoints->fwd_velocity;
    // Determine velocity error for both left and right wheels
    float left_vel_setpoint = fwd_vel_filt;
    float right_vel_setpoint = fwd_vel_filt;

    float left_err = left_vel_setpoint - mb_state->left_velocity;
    float right_err = right_vel_setpoint - mb_state->right_velocity;
    // if (abs(left_err) < 1.0e-10){
    //     rc_filter_reset(&left_wheel_speed_pid);
    //     rc_filter_pid(&left_wheel_speed_pid, pid_params.kp, pid_params.ki, pid_params.kd, pid_params.dFilterHz, DT);
    // }
    // if (abs(right_err) < 1.0e-10){
    //     rc_filter_reset(&right_wheel_speed_pid);
    //     rc_filter_pid(&right_wheel_speed_pid, pid_params.kp, pid_params.ki, pid_params.kd, pid_params.dFilterHz, DT);

    // }

    // Calculate feed-forward term for each wheel
    // Based on free floating wheel
    float left_ff = 0.9839674495766608 * left_vel_setpoint + 0.009105866186956774;
    float right_ff = 0.9847512260577416 * right_vel_setpoint + 0.012830887327368673;

    // Calculate PID output
    float left_pid = rc_filter_march(&left_wheel_speed_pid, left_err);
    float right_pid = rc_filter_march(&right_wheel_speed_pid, right_err);

    // Calculate resulting command to motors
    float left_cmd =  left_pid + left_ff;
    float right_cmd = right_pid + right_ff;

    // Saturate command to the range of possible values
    rc_saturate_float(&left_cmd, -1.0f, 1.0f);
    rc_saturate_float(&right_cmd, -1.0f, 1.0f);


    // "Publish" command
    left_cmd = fabs(left_cmd) < 0.05 ? 0.0 : left_cmd;
    right_cmd = fabs(right_cmd) < 0.05 ? 0.0 : right_cmd;

    mb_state->left_cmd = left_cmd;
    mb_state->right_cmd = right_cmd;

    return 0;
}


/*******************************************************************************
* int mb_destroy_controller()
* 
* TODO: Free all resources associated with your controller
*
* return 0 on success
*
*******************************************************************************/

int mb_destroy_controller(){
    rc_filter_free(&left_wheel_speed_pid);
    rc_filter_free(&right_wheel_speed_pid);
    rc_filter_free(&fwd_vel_sp_lpf);
    return 0;
}