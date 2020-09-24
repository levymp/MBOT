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
    FILE* file = fopen(CFG_PATH, "r");
    if (file == NULL){
        printf("Error opening pid.cfg\n");
    }

/******
*
*   Example of loading a line from .cfg file:
*
*    fscanf(file, "%f %f %f %f", 
*        &pid_params.kp,
*        &pid_params.ki,
*        &pid_params.kd,
*        &pid_params.dFilterHz
*        );
*
******/

    pid_parameters_t pid_para_left;
    fscanf(file, "%f %f %f %f %f %f", 
        &pid_para_left.kp,
        &pid_para_left.ki,
        &pid_para_left.kd,
        &pid_para_left.dFilterHz,
        &pid_para_left.Tf,
        &pid_para_left.dt
        );

    pid_parameters_t pid_para_right;
    fscanf(file, "%f %f %f %f %f %f", 
        &pid_para_right.kp,
        &pid_para_right.ki,
        &pid_para_right.kd,
        &pid_para_right.dFilterHz,
        &pid_para_right.Tf,
        &pid_para_right.dt
        );

    // printf("%f %f %f %f %f %f\n", pid_para_left.kp, pid_para_left.ki, pid_para_left.kd, pid_para_left.dFilterHz, pid_para_left.Tf, pid_para_left.dt);
    // printf("%f %f %f %f %f %f\n", pid_para_right.kp, pid_para_right.ki, pid_para_right.kd, pid_para_right.dFilterHz, pid_para_right.Tf, pid_para_right.dt);
    
    // if(rc_filter_pid(&left_wheel_speed_pid, pid_para_left.kp, pid_para_left.ki, pid_para_left.kd, pid_para_left.Tf, pid_para_left.dt)) {
    if(rc_filter_pid(&left_wheel_speed_pid, pid_para_left.kp, pid_para_left.ki, pid_para_left.kd, 4/SAMPLE_RATE_HZ, 1/SAMPLE_RATE_HZ)) {
                fprintf(stderr,"ERROR in mb_controller_update in mb_controller.c, failed to make pid controller\n");
                return -1;
    }    
    if(rc_filter_pid(&right_wheel_speed_pid, pid_para_right.kp, pid_para_right.ki, pid_para_right.kd, 4/SAMPLE_RATE_HZ, 1/SAMPLE_RATE_HZ)) {
                fprintf(stderr,"ERROR in mb_controller_update in mb_controller.c, failed to make pid controller\n");
                return -1;
    }
    // rc_filter_print(left_wheel_speed_pid);
    rc_filter_enable_saturation(&left_wheel_speed_pid, MIN_DUTY, MAX_DUTY);
    rc_filter_enable_saturation(&right_wheel_speed_pid, MIN_DUTY, MAX_DUTY);



    fclose(file);
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


    // float left_velocity_setpoint = (2 * mb_setpoints->fwd_velocity - WHEEL_BASE * mb_setpoints->turn_velocity) / 2;
    // float right_velocity_setpoint = (2 * mb_setpoints->fwd_velocity + WHEEL_BASE * mb_setpoints->turn_velocity) / 2;

    // mb_setpoints->left_velocity = left_velocity_setpoint;
    // mb_setpoints->right_velocity = right_velocity_setpoint;

    mb_setpoints->left_velocity = (2 * mb_setpoints->fwd_velocity - WHEEL_BASE * mb_setpoints->turn_velocity) / 2;
    mb_setpoints->right_velocity = (2 * mb_setpoints->fwd_velocity + WHEEL_BASE * mb_setpoints->turn_velocity) / 2;

    mb_state->left_cmd = rc_filter_march(&left_wheel_speed_pid, mb_setpoints->left_velocity - mb_state->left_velocity);
    mb_state->right_cmd = rc_filter_march(&right_wheel_speed_pid, mb_setpoints->right_velocity - mb_state->right_velocity);

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
    return 0;
}
