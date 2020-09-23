#define _GNU_SOURCE
#include "../mobilebot/mobilebot.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
* @file mb_controller.c
// @author - Michael Levy + ROB 550 Template
*/
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
