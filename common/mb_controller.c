#define _GNU_SOURCE
#include "../mobilebot/mobilebot.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
* @file mb_controller.c
// @author - Michael Levy + ROB 550 Template
*/


// Declare functions 



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
    // initialize left and right parameters
    if(mb_load_controller_config(&r_pid_params) || mb_load_controller_config(&l_pid_params)) {
        fprintf(stderr, "ERROR: FAILED TO LOAD CONFIG");
        return -1;
    }

    // left/right motors have config loaded
    // now initialize pid controller for both wheels
    if(rc_filter_pid(&pid_filt_r, r_pid_params.kp, r_pid_params.ki, r_pid_params.kd, 4 * DT, DT) ||
            rc_filter_pid(&pid_filt_l, l_pid_params.kp, l_pid_params.ki, l_pid_params.kd, 4 * DT, DT)) 
    {
        fprintf(stderr, "ERRROR: FAILED TO CONFIGURE PID");
        return -1;
    }
    // set saturation values
    if(rc_filter_enable_saturation(&pid_filt_r, r_pid_params.out_lim, r_pid_params.int_lim) ||
            rc_filter_enable_saturation(&pid_filt_l, l_pid_params.out_lim, l_pid_params.int_lim))
    {
        fprintf(stderr, "ERROR: FAILED TO ENABLE SATURATION VALUES");
        return -1;
    }

    // Set up lowpass filters for fwd velocity
    if(rc_filter_first_order_lowpass(&lp_filt_l, DT, .1) || rc_filter_first_order_lowpass(&lp_filt_r, DT, .1)) 
    {
        fprintf(stderr, "ERRROR: FAILED TO CONFIGURE LOWPASS");
        return -1;
    }

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


int mb_load_controller_config(pid_parameters_t* pid_params){
    // Get file open
    FILE* file = fopen(CFG_PATH, "r");
    if (file == NULL){
        printf("Error opening pid.cfg\n");
    }
    // line read variables
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    // parsing variables
    char* value_ptr = NULL;
    int delim = '=';
    char* result = NULL;
    double value; 
    int i;
    int count = 0;

    pid_params->FF_term = .8;

    // Desired Keys for config
    char keys[6][10] = {
        "kp",
        "ki",
        "kd",
        "dfilterhz",
        "out_lim", 
        "int_lim"
    };
    // length of keys
    int keys_length =(int)sizeof(keys)/sizeof(keys[0]);
    // Open file
    file = fopen(CFG_PATH, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: UNABLE TO OPEN CONFIG FILE");
        return -1;
    }
    
    // Go through all lines in file
    while ((nread = getline(&line, &len, file)) != -1) {
        // pass on the comments or empty lines
        if(nread == 1 || line[0] == '#') {
            continue;
        }
        
        // lower case all values
        for(i = 0; i < (int)nread; i++) {
            line[i] = tolower(line[i]);
        }

        // find the where the delimiter is
        value_ptr = strchr(line, delim);

        // march value_ptr to actual value
        // + 2 if space after delimiter
        // + 1 if value is right after delimiter
        if(value_ptr[1] == ' ') {
            value_ptr += 2;
        } else{
            value_ptr += 1;
        }

        // assign float to value (assuming only \0 after value and no addition comments)
        value = atof(value_ptr);
        
        // Check line against keys and if a key is found assign its value
        for(i = 0; i < keys_length; i++) {
            result = strstr(line, keys[i]);
            // Check if a key was found
            if(result != NULL) {
                // Write the value to struct base on i (index of keys array)
                // TO ADD additional parameters just add it to keys array and add another case
                switch(i) {
                    case 0:
                        pid_params->kp = value;
                        count++;
                        break;
                    case 1:
                        pid_params->ki = value;
                        count++;
                        break;
                    case 2:
                        pid_params->kd = value;
                        count++;
                        break;
                    case 3:
                        pid_params->dFilterHz = value;
                        count++;
                        break;
                    case 4:
                        pid_params->out_lim = value;
                        count++;
                        break;
                    case 5:
                        pid_params->int_lim = value;
                        count++;
                        break;    
                    default:
                        fprintf(stderr, "ERROR: UNEXPECTED VALUE FOUND WHEN READING CONFIG");
                        return -1;
                }
                break;
            }
        }
        // cleanup before next iteration
        value_ptr = NULL;
        result = NULL; 
    }
    // Final Check 
    if(count != keys_length) {
            fprintf(stderr, "ERROR: DID NOT WRITE ALL VALUES FROM CONFIG FILE TO PID PARAMS");
            return -1;
    }
    // Clean up and exit
    free(line);
    free(result);
    fclose(file);
    return 0;
}

// Reset all the filters used in the controller
// Return 0 on success
int mb_controller_filter_reset(mb_state_t* mb_state, mb_setpoints_t* mb_setpoints){
    
    if(rc_filter_reset(&pid_filt_l) ||
    rc_filter_reset(&pid_filt_r) ||
    rc_filter_reset(&lp_filt_l) ||
    rc_filter_reset(&lp_filt_r))
    {
        fprintf(stderr, "ERRROR: FAILED TO RESET FILTERS");
        return -1;
    }
    
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

    // Setpoint for each wheel without lowpass filter
    //mb_setpoints->left_velocity = (2 * mb_setpoints->fwd_velocity - WHEEL_BASE * mb_setpoints->turn_velocity) / 2;
    //mb_setpoints->right_velocity = (2 * mb_setpoints->fwd_velocity + WHEEL_BASE * mb_setpoints->turn_velocity) / 2;

    // Setpoint for each wheel with lowpass filter on the fwd velocity to prevent jerking the robot chassis
    mb_setpoints->left_velocity = (2 * rc_filter_march(&lp_filt_r, mb_setpoints->fwd_velocity) - WHEEL_BASE * mb_setpoints->turn_velocity) / 2;
    mb_setpoints->right_velocity = (2 * rc_filter_march(&lp_filt_l, mb_setpoints->fwd_velocity) + WHEEL_BASE * mb_setpoints->turn_velocity) / 2;

    // If there's a change in the fwd setpoint, reset the filters and start the wheel commands with the feed forward term
    if(mb_setpoints->fwd_velocity != mb_setpoints->old_fwd){

        mb_setpoints->old_fwd = mb_setpoints->fwd_velocity;
        mb_controller_filter_reset(mb_state, mb_setpoints);

        // Starting cmd, Feedforward*setpoint
        mb_state->left_cmd = l_pid_params.FF_term*mb_setpoints->left_velocity;
        mb_state->right_cmd = r_pid_params.FF_term*mb_setpoints->right_velocity;
    }else{

        //March the PID wilter acording to the error, add the modified error to the cmd state
        mb_state->left_cmd += rc_filter_march(&pid_filt_l, (double) (mb_setpoints->left_velocity - mb_state->left_velocity));
        mb_state->right_cmd += rc_filter_march(&pid_filt_r, (double) (mb_setpoints->right_velocity - mb_state->right_velocity));
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

    if(rc_filter_free(&pid_filt_l) ||
    rc_filter_free(&pid_filt_r) ||
    rc_filter_free(&lp_filt_l) ||
    rc_filter_free(&lp_filt_r))
    {
        fprintf(stderr, "ERRROR: FAILED TO FREE FILTERS");
        return -1;
    }
    
    return 0;
}
