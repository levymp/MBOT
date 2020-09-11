/**
 * @file drive_simple_encoder.c
 * This file drives the bot in a square using the encoders
 */

#include <stdio.h>
#include <robotcontrol.h> // includes ALL Robot Control subsystems
#include <signal.h>
#include <math.h>
#include <stdlib.h>


// Define Enums
enum motor {
    RIGHT = 1,
    LEFT = 2,
};

enum direction {
	FWD = 1, 
	BWD = -1
};

enum speed {
	FST = 100, 
	MEDF = 75,
	MED = 50,
	SLOW = 25,
	STP = 0
};



// Global Variables for encoders
int avg_rotation = 1562;
double one_rotation = M_PI * 0.084; // one rotation in M
uint64_t wait_time = 1E6;

// function declarations
double DC(enum direction, enum speed);


/**
 * critical components:
 * - ensure no existing instances are running and make new PID file
 * - start the signal handler
 * - initialize subsystems you wish to use
 * - while loop that checks for EXITING condition
 * - cleanup subsystems at the end
 *
 * @return     0 during normal operation, -1 on error
 */

// Interrupt from RC Library
static void __signal_handler(__attribute__ ((unused)) int dummy)
{
        rc_set_state(EXITING);
        return;
}

int main()
{

	// make sure another instance isn't running
	if(rc_kill_existing_process(2.0)<-2) return -1;

	// initialize motors
	if(rc_motor_init() == -1)
	{
		fprintf(stderr, "ERROR FAILED TO INIT MOTORS\n");
		return -1;
	}
	// initialize encoders
	if(rc_encoder_init() == -1)
	{
		fprintf(stderr, "ERROR: FAILED TO INIT ENCODERS\n");
		return -1;
	}
	
	// SET Signal Handler
	signal(SIGINT, __signal_handler); 
	// make PID file to indicate project is running
	rc_make_pid_file();

	printf("RUNNING.....\n");

	printf("\nENCODER POSITIONS\n");
	printf("RIGHT WHEEL |");
	printf("LEFT WHEEL |");
	printf("DISTANCE |");

	printf(" \n");

	// Keep looping until state changes to EXITING
	rc_set_state(RUNNING);
	int right_encoder, left_encoder;
	int right_rotation = 0;
	int left_rotation = 0;
	double distance = 0;
	int turn_total = 0;
	// bool whether to skip distance when turning
	int dist_calc = 1; 

	// move forward initially
	rc_motor_set(RIGHT, DC(FWD, SLOW));
	rc_motor_set(LEFT , DC(FWD, SLOW));

	while(rc_get_state()!=EXITING){
		// print encoder readings
		printf("\r");
		// Read encoder
		right_encoder = rc_encoder_read(RIGHT);
		left_encoder = rc_encoder_read(LEFT);
	
		// add rotation distance (previous rotations)
		if(dist_calc)
		{
			distance =(double) ((right_rotation + left_rotation) / 2);
			// add encoder distance (< 1 rotation)
			distance +=(double) (abs(right_encoder) + abs(left_encoder)) / (2 * avg_rotation);
			// multiply total by circumference of wheel
			distance *= one_rotation; // Meters

			// check if right encoder has completed a rotation
			if(abs(right_encoder) >= avg_rotation)
			{
				right_rotation++;
				if(rc_encoder_write(RIGHT, 0) == -1)
				{
					fprintf(stderr,"ERROR: FAILED ENCODER WRITE FOR RIGHT ENCODER");
				}
			}
			// check if left encoder has completed a rotation
			if(abs(left_encoder) >= avg_rotation)
			{
				left_rotation++;
				if(rc_encoder_write(LEFT, 0) == -1)
				{
					fprintf(stderr, "ERROR: FAILED ENCODER WRITE FOR LEFT ENCODER");
				}
			}
		}
		
		// print values to terminal
		printf("%10d |%10d | %10f (meters)", right_encoder, left_encoder, distance);


		
		// check if > 1 M fwd traveled or 90 deg turned
		if(distance >= 1.0){
			// stop
			rc_motor_set(RIGHT, (double) STP);
			rc_motor_set(LEFT, (double) STP);
			printf("\nTURNING\n");
			rc_nanosleep(wait_time);
			
			// reset encoders
			rc_encoder_write(LEFT, 0);
			rc_encoder_write(RIGHT, 0);
			
			// turn
			rc_motor_set(RIGHT, DC(FWD, SLOW));
			rc_motor_set(LEFT, DC(BWD, SLOW));
			
			// add a turn
			turn_total++;

			// set distance to turn flag value
			dist_calc = 0;
			distance = 0; 

		}else if(dist_calc == 0 && (right_encoder > (avg_rotation/2)) && (left_encoder > (avg_rotation/2)) ){
			printf("\nMOVING FORWARD\n");
			// stop
			rc_motor_set(RIGHT, (double) STP);
			rc_motor_set(LEFT, (double) STP);
			rc_nanosleep(wait_time);
			
			// reset encoders
			rc_encoder_write(LEFT, 0);
			rc_encoder_write(RIGHT, 0);
			
			// reset distance rotations and flag
			distance = 0;
			left_rotation = 0;
			right_rotation = 0; 
			dist_calc = 1;
			
			// check if we're done
			if(turn_total == 4)
			{
				rc_set_state(EXITING);
			}else{
				// move forward
				rc_motor_set(RIGHT, DC(FWD, SLOW));
				rc_motor_set(LEFT , DC(FWD, SLOW));	
			}
		}

		// flush output
		fflush(stdout);
		// quick sleep
		rc_nanosleep(wait_time);
	}

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

	rc_remove_pid_file();	// remove pid file LAST
	return 0;
}

double DC(enum direction d, enum speed s)
{
	double result =(double) d * s * 0.01;
    return result;
}
