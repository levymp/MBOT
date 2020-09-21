/*******************************************************************************
* @file measure_motor_params.c
*   @author ROB 550 Template w/additions from Michael Levy 
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

void test_speed(float du, float dtime_s);

/*******************************************************************************
* int main() 
*
*******************************************************************************/
int main(int argc, char *argv[]){

	// make sure another instance isn't running
    // if return value is -3 then a background process is running with
    // higher privaledges and we couldn't kill it, in which case we should
    // not continue or there may be hardware conflicts. If it returned -4
    // then there was an invalid argument that needs to be fixed.

    // Parse arguments
    if( argc > 3) {
        fprintf(stderr, "ERROR: TOO MANY ARGUMENTS");
        return -1;
    }
    // get the duty cycle
    float duty = atof(argv[1]);
    float dtime_s = atof(argv[2]);

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

    if(rc_encoder_init()<0){
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

	if(rc_get_state()==RUNNING){
		rc_nanosleep(1E9); //sleep for 1s
	}
    printf("RUNNING.....\n");

	// Setup print statements
	printf("\nRIGHT VEL.\n");
	printf("LEFT VEL. |");
	printf("DUTY CYCLE |");
	printf(" \n");
    // call motor test
    test_speed(duty, dtime_s);
	
	// Exit cleanly
    // CLEANUP 
	if(rc_encoder_cleanup() == -1)
	{
		fprintf(stderr, "ERROR: ENCODER CLEANUP FAILED\n");
		return -1;
	}
	if(rc_motor_cleanup() == -1)
	{
		fprintf(stderr, "ERROR: MOTOR CLEANUP FAILED\n");
		return -1;
	}

    // remove pid file LAST
	rc_remove_pid_file();
	return 0;
}

void test_speed(float duty, float dtime_s){
    // Given a change in time measure the velocity of each wheel

    int v_r, v_l;
    // check duty cycle given
    if(abs(duty) > 1) {
        fprintf(stderr,"ERROR: Incorrect Duty Cycle given!\n");
        return -1;
    }
    
    // Check if time is strictly positive
    if(dtime_s <= 0) {
        fprintf(stderr,"ERROR: delta-t is not strictly positive\n");
        return -1;
    }
    
    // Reset encoders to zero
    if(rc_encoder_write(LEFT_MOTOR, 0) != 0 || rc_encoder_write(RIGHT_MOTOR, 0) != 0) {
        fprintf(stderr, "ERROR: COULD NOT RESET ENCODER VALUES");
        return -1;
    }
    
    // Start motors
    if(rc_motor_set(LEFT_MOTOR, duty) != 0 || rc_motor_set(RIGHT_MOTOR, duty) != 0) {
        fprintf(stderr, "ERROR: COULD NOT Start Motors");
        return -1;
    }

    // sleep for desired time
    rc_nanosleep(dtime_s * 1E9);

    // turn off motors
    if(rc_motor_set(LEFT_MOTOR, 0) != 0 || rc_motor_set(RIGHT_MOTOR, 0) != 0) {
        fprintf(stderr, "ERROR: COULD NOT Start Motors");
        return -1;
    }
    // calculate velocities
    v_r = enc2meters * rc_encoder_read(RIGHT_MOTOR) / dtime_s
    v_l = enc2meters * rc_encoder_read(LEFT_MOTOR) / dtime_s
    printf("%10d | %10d | %10d", v_r, v_l, duty)
}