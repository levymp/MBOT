/*******************************************************************************
* measure_motor_params.c
*   Template code 
*   Complete this code to automatically measure motor parameters
*   or print out data to be namalyzed in numpy
* 
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <rc/start_stop.h>
#include <rc/encoder_eqep.h>
#include <rc/encoder.h>
#include <rc/adc.h>
#include <rc/time.h>
#include <rc/motor.h>
#include "../common/mb_motor.h"
#include "../common/mb_defs.h"


float enc2meters = (WHEEL_DIAMETER * M_PI) / (GEAR_RATIO * ENCODER_RES);

void test_speed(float du, float dtime_s, FILE *file);

/*******************************************************************************
* int main() 
*
*******************************************************************************/
int main(){

	// make sure another instance isn't running
    // if return value is -3 then a background process is running with
    // higher privaledges and we couldn't kill it, in which case we should
    // not continue or there may be hardware conflicts. If it returned -4
    // then there was an invalid argument that needs to be fixed.
    if(rc_kill_existing_process(2.0)<-2) return -1;

	// start signal handler so we can exit cleanly
    if(rc_enable_signal_handler()==-1){
                fprintf(stderr,"ERROR: failed to start signal handler\n");
                return -1;
    }

#if defined(MRC_VERSION_1v3) || defined(MRC_VERSION_2v1)
    if(mb_motor_init()<0){
        fprintf(stderr,"ERROR: failed to initialze mb_motors\n");
        return -1;
    }
#endif

#if defined(BEAGLEBONE_BLUE)
    if(rc_motor_init()<0){
        fprintf(stderr,"ERROR: failed to initialze motors\n");
        return -1;
    }
#endif

    if(rc_encoder_eqep_init()<0){
        fprintf(stderr,"ERROR: failed to initialze encoders\n");
        return -1;
    }
    
    // make PID file to indicate your project is running
	// due to the check made on the call to rc_kill_existing_process() above
	// we can be fairly confident there is no PID file already and we can
	// make our own safely.
	rc_make_pid_file();

	// done initializing so set state to RUNNING
	rc_set_state(RUNNING);

    FILE *fp = fopen("measure_motor_params.csv","w+");
	if(rc_get_state()==RUNNING){
		rc_nanosleep(1E9); //sleep for 1s
	    int NUM_PWM_STEPS = 20;
		float PWM_INC = 1.0/(NUM_PWM_STEPS);
		float TIME_STEP = 0.50f;

		// Test speed measurements with PWM swoop
        fprintf(fp,"PWM, Left Speed, Right Speed\n");  
		for (int i = 0; i < NUM_PWM_STEPS; i++){
            float duty = (i+1) * PWM_INC;
		    test_speed(duty, TIME_STEP, fp);
		}
	}
	
	// TODO: Plase exit routine here
    rc_set_state(EXITING);
	rc_motor_cleanup();
    rc_encoder_cleanup();
	rc_encoder_eqep_cleanup();

    // remove pid file LAST
	rc_remove_pid_file();   
	return 0;
}

void test_speed(float duty, float dtime_s, FILE *file){
    // set motors to desired speed
    rc_motor_set(LEFT_MOTOR, duty);
    rc_motor_set(RIGHT_MOTOR, duty);
    rc_nanosleep(1E9);

    int left_encoder_count = LEFT_ENCODER_POL*rc_encoder_eqep_read(LEFT_ENCODER);
    int right_encoder_count = RIGHT_ENCODER_POL*rc_encoder_eqep_read(RIGHT_ENCODER);
    
    rc_nanosleep(dtime_s * 1E9);  
    rc_motor_set(0,0);
    
    int left_encoder_count_delta = LEFT_ENCODER_POL*rc_encoder_eqep_read(LEFT_ENCODER) - left_encoder_count;
    int right_encoder_count_delta = RIGHT_ENCODER_POL*rc_encoder_eqep_read(RIGHT_ENCODER) - right_encoder_count;
    
    float left_wheel_speed = left_encoder_count_delta * enc2meters / dtime_s;
    float right_wheel_speed = right_encoder_count_delta * enc2meters / dtime_s;
    fprintf(file,"%.3f, %.3f, %.3f\n", duty, left_wheel_speed, right_wheel_speed);  
    fprintf(stdout,"%.3f, %.3f, %.3f\n", duty, left_wheel_speed, right_wheel_speed);  

}
