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

    // Desired Keys for config
    char keys[6][10] = {
        "kp",
        "ki",
        "kd",
        "dfilterhz",
        "out_lim", 
        "int_lim"
    };

    // Open file
    file = fopen(CFG_PATH, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: UNABLE TO OPEN CONFIG FILE");
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
        for(i = 0; i < (int)sizeof(keys)/sizeof(keys[0]); i++) {
            result = strstr(line, keys[i]);
            // Check if a key was found
            if(result != NULL) {
                // Write the value to struct base on i (index of keys array)
                // TO ADD additional parameters just add it to keys array and add another case
                switch(i) {
                    case 0:
                        &pid_parameters.kp = value;
                        break;
                    case 1:
                        &pid_parameters.ki = value;
                        break;
                    case 2:
                        &pid_parameters.kd = value;
                        break;
                    case 3:
                        &pid_parameters.dFilterHz = value;
                        break;
                    case 4:
                        &pid_parameters.out_lim = value;
                        break;
                    case 5:
                        &pid_parameters.int_lim = value;
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
