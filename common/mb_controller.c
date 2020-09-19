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

    fscanf(file, "%f %f %f %f %f %f %f %f", 
        &l_wheel_speed_params.kp,
        &l_wheel_speed_params.ki,
        &l_wheel_speed_params.kd,
        &l_wheel_speed_params.dFilterHz,
        &r_wheel_speed_params.kp,
        &r_wheel_speed_params.ki,
        &r_wheel_speed_params.kd,
        &r_wheel_speed_params.dFilterHz);

    rc_filter_pid(&pid_fil_l, 
        l_wheel_speed_params.kp,
        l_wheel_speed_params.ki,
        l_wheel_speed_params.kd,
        l_wheel_speed_params.dFilterHz/2,
        l_wheel_speed_params.dFilterHz);

    rc_filter_pid(&pid_fil_r, 
        r_wheel_speed_params.kp,
        r_wheel_speed_params.ki,
        r_wheel_speed_params.kd,
        r_wheel_speed_params.dFilterHz/2,
        r_wheel_speed_params.dFilterHz);

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
    mb_state->left_cmd = rc_filter_march(&pid_fil_l, mb_state.left_encoder_delta/SAMPLE_RATE_HZ);
    mb_state->right_cmd = rc_filter_march(&pid_fil_l, mb_state.left_encoder_delta/SAMPLE_RATE_HZ);
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
    return 0;
}
