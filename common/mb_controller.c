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

    printf("%f %f %f %f %f %f %f %f", 
        l_wheel_speed_params.kp,
        l_wheel_speed_params.ki,
        l_wheel_speed_params.kd,
        l_wheel_speed_params.dFilterHz,
        r_wheel_speed_params.kp,
        r_wheel_speed_params.ki,
        r_wheel_speed_params.kd,
        r_wheel_speed_params.dFilterHz);

    if(rc_filter_pid(&pid_filt_l, 
        (double) l_wheel_speed_params.kp,
        (double) l_wheel_speed_params.ki,
        (double) l_wheel_speed_params.kd,
        .04,
        .02) == -1){
        fprintf(stderr,"ERROR: failed to start signal handler\n");
        return 0;
    }
    rc_filter_enable_saturation(&pid_filt_l, -1, 1);
    rc_filter_print(pid_filt_l);

    rc_filter_pid(&pid_filt_r, 
        r_wheel_speed_params.kp,
        r_wheel_speed_params.ki,
        r_wheel_speed_params.kd,
        .04,
        .02);
    rc_filter_enable_saturation(&pid_filt_r, -1, 1);

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
    float vl = ((float) mb_state->left_encoder_delta * (WHEEL_DIAMETER * 3.141) / (GEAR_RATIO * ENCODER_RES))/(1.0/SAMPLE_RATE_HZ);
    float vr = ((float) mb_state->right_encoder_delta * (WHEEL_DIAMETER * 3.141) / (GEAR_RATIO * ENCODER_RES))/(1.0/SAMPLE_RATE_HZ);
    mb_state->fwd_velocity = vl;
    mb_state->left_cmd = rc_filter_march(&pid_filt_l, vl - mb_setpoints->fwd_velocity);
    mb_state->right_cmd = rc_filter_march(&pid_filt_r, vr - mb_setpoints->fwd_velocity);

    if(mb_state->left_cmd > 1){
        rc_filter_reset(&pid_filt_l);
    }
    if(mb_state->right_cmd > 1){
        rc_filter_reset(&pid_filt_r);
    }
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
    rc_filter_free(&pid_filt_l);
    rc_filter_free(&pid_filt_r);
    return 0;
}
