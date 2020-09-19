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

#define PI 3.14159265358979323846

float enc2meters = (WHEEL_DIAMETER * M_PI) / (GEAR_RATIO * ENCODER_RES);

void test_speed(float du, float dtime_s);

/*******************************************************************************
* int main() 
*
*******************************************************************************/
int main(int argc, char** argv){

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

    test_speed((float)atof(argv[1]), (float)atof(argv[2]));
	
	// TODO: Plase exit routine here

    // remove pid file LAST
	rc_remove_pid_file();   
	return 0;
}

void test_speed(float duty, float dtime_s){
    rc_encoder_eqep_init();
    rc_motor_init();

    int enc_init_left = rc_encoder_eqep_read(LEFT_MOTOR);
    int enc_init_right = rc_encoder_eqep_read(RIGHT_MOTOR);

    rc_motor_set(LEFT_MOTOR, duty);
	rc_motor_set(RIGHT_MOTOR, duty);

	rc_nanosleep(dtime_s * 1e9);

	int enc_end_left = rc_encoder_eqep_read(LEFT_MOTOR);
	int enc_end_right = rc_encoder_eqep_read(RIGHT_MOTOR);

	rc_motor_set (LEFT_MOTOR, 0);
	rc_motor_set (RIGHT_MOTOR, 0);

    rc_motor_cleanup();
    rc_encoder_eqep_cleanup();

    int enc_diff_left = enc_end_left - enc_init_left;
    int enc_diff_right = enc_end_right - enc_init_right;

    double wheel_speed_left = (double)enc_diff_left * enc2meters / dtime_s;
    double wheel_speed_right = (double)enc_diff_right * enc2meters / dtime_s;

    // printf("initial encoder count: %d %d\n", enc_init_left, enc_init_right);
    // printf("end encoder count: %d %d\n", enc_end_left, enc_end_right);
    printf("%lf %lf\n", wheel_speed_left, wheel_speed_right);

    return;
}