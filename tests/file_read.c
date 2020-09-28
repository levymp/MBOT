// #include _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mb_structs.h>



int main(int argc,char* argv[])
{
    
    char CFG_PATH[] = "/Users/michaellevy/Documents/Michigan/Fall 2020/ROB550/MBOT/tests/pid.cfg";
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
        // pass on the comments or empty lines
        if(nread == 1 || line[0] == '#') {
            continue;
        }
        // lower case the line 
        for(i = 0; i < (int)nread; i++) {
            line[i] = tolower(line[i]);
        }
        // print the line
        printf("LINE LENGTH: %zu:\n", nread);
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
        value_ptr = NULL;
        result = NULL; 
    }
    // Clean up and exit
    free(line);
    free(result);
    fclose(file);
    return 0;
}