// #include _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
// #include "/Users/michaellevy/Documents/Michigan/Fall 2020/ROB550/MBOT/common/mb_structs.h"



int main(int argc,char* argv[])
{
    
    char CFG_PATH[] = "/home/levymp/Documents/MBOT/tests/pid.cfg";
    FILE* file;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    file = fopen(CFG_PATH, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: UNABLE TO OPEN CONFIG FILE");
    }
    char* value_ptr = NULL;
    int delim = '=';
    char keys[6][10] = {
        "kp",
        "ki",
        "kd",
        "dfilterhz",
        "out_lim", 
        "int_lim"
    };

    char* result = NULL;
    double value; 
    int i;
    while ((nread = getline(&line, &len, file)) != -1) {
        // print the line
        printf("Retrieved line of length %zu:\n", nread);
        fwrite(line, nread, 1, stdout);

        // pass on the comments or empty lines
        if(nread == 1 || line[0] == '#') {
            continue;
        }
        // lower case the line 
        for(i = 0; i < (int)nread; i++) {
            line[i] = tolower(line[i]);
        }
        fwrite(line, nread, 1, stdout);
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
        for(i = 0; i < 6; i++) {
            result = strstr(line, keys[i]);
            // Check if a key was found
            if(result != NULL) {
                // Write the value to struct
                switch(i) {
                    case 0:
                        // &pid_parameters.kp = value;
                        break;
                    case 1:
                        // &pid_parameters.ki = value;
                        break;
                    case 2:
                        // &pid_parameters.kd = value;
                        break;
                    case 3:
                        // &pid_parameters.dFilterHz = value;
                        break;
                    case 4:
                        // &pid_parameters.out_lim = value;
                        break;
                    case 5:
                        // &pid_parameters.int_lim = value;
                        break;    
                    default:
                        fprintf(stderr, "ERROR: UNEXPECTED VALUE FOUND WHEN READING CONFIG");
                        return -1;
                }
                break;
            }
        }
        value_ptr = NULL;
        result = NULL; 
    }
    free(line);
    free(result);
    free(keys);
    fclose(file);
    exit(EXIT_SUCCESS);
}