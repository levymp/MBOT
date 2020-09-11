/**
 * @file drive_simple_timed.c
 *
 * This file drives the MBOT in a square using pre-determined times. 
 */

#include <stdio.h>
#include <robotcontrol.h> // includes ALL Robot Control subsystems
#include <rc/motor.h>
#include <rc/time.h>
#include <signal.h>



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

enum move {
	FORWARD = 1, 
	BACKWARD = -1, 
	TURN_RIGHT = 2,
	TURN_LEFT = 3
};

// function declarations
void move(enum move m, enum speed s, int time);
double DC(enum direction, enum speed);

/**
 * This template contains these critical components
 * - ensure no existing instances are running and make new PID file
 * - start the signal handler
 * - initialize subsystems you wish to use
 * - while loop that checks for EXITING condition
 * - cleanup subsystems at the end
 *
 * @return     0 during normal operation, -1 on error
 */


// interrupt handler to catch ctrl-c
static void __signal_handler(__attribute__ ((unused)) int dummy)
{
        printf("INTERRUPTED EXITING\n");
		rc_set_state(EXITING);
        return;
}


int main()
{
	// make sure another instance isn't running
	// if return value is -3 then a background process is running with
	// higher privaledges and we couldn't kill it, in which case we should
	// not continue or there may be hardware conflicts. If it returned -4
	// then there was an invalid argument that needs to be fixed.
	if(rc_kill_existing_process(2.0)<-2) return -1;
	
	// define times to drive in nano seconds
	uint64_t fwd_time = 5E9;
	// define time to turn
	uint64_t turn_time = 1E9;
	// define time to wait
	uint64_t wait_time = 4E8;

	// initialize motors
	if(rc_motor_init() == -1)
	{
		fprintf(stderr, "ERROR FAILED TO INIT MOTORS\n");
		return -1;
	}

	// set Signal Handler
	signal(SIGINT, __signal_handler); 

	// make PID file to indicate project is running
	rc_make_pid_file();

	// time to get the camera ready...
	rc_nanosleep(fwd_time); 
	// set the state
	printf("RUNNING.....\n");
	rc_set_state(RUNNING);

	// turn toal sum
	int turn_total = 0;

	// loop to go around four times until signal recieved or interrupt triggered
	while(rc_get_state() != EXITING){
		
		// forward
		printf("MOVING FORWARD\n");
		rc_motor_set (RIGHT, DC(FWD, SLOW));
		rc_motor_set(LEFT , DC(FWD, SLOW));
		rc_nanosleep(fwd_time);
		
		// stop
		rc_motor_set(RIGHT, (double) STP);
		rc_motor_set(LEFT, (double) STP);
		rc_nanosleep(wait_time);
		
		// turn
		printf("TURNING\n");
		rc_motor_set(RIGHT, DC(FWD, SLOW));
		rc_motor_set(LEFT, DC(BWD, SLOW));
		rc_nanosleep(turn_time);
		
		// stop
		rc_motor_set(RIGHT, (double) STP);
		rc_motor_set(LEFT, (double) STP);
		rc_nanosleep(wait_time);
		
		// Add a turn
		turn_total++;
		
		// Check if turn total is 4 and if so exit
		if(turn_total == 4)
		{
			printf("FOUR TURNS COMPLETED! EXITING!\n");
			rc_set_state(EXITING);
		}
	}

	
	// RC Motor Cleanup
	if(rc_motor_cleanup() == -1)
	{
		fprintf(stderr, "ERROR: MOTOR CLEANUP FAILED\n");
		return -1;
	}

	rc_remove_pid_file();	// remove pid file LAST
	return 0;

	// Past code...
	// move(TURN_RIGHT, SLOW, 7E5);

	// move(FORWARD, SLOW, 14);
	// move(TURN_RIGHT, SLOW, .7);

	// move(FORWARD, SLOW, 14);
	// move(TURN_RIGHT, SLOW, .7);
	
	// move(FORWARD, SLOW, 14);
	// move(TURN_RIGHT, SLOW, .7);
}

// direction, speed, time (s)
void move(enum move m, enum speed s, int time)
{	
	// Assume Motor has been initialized
	rc_motor_init();
	switch (m)
	{
		case FORWARD:
			// Travel Forward @ speed s w/time t
			printf("FORWARD\n");
			rc_motor_set(RIGHT, DC(FWD, s));
			rc_motor_set(LEFT, DC(FWD, s));
			rc_nanosleep(time);
			break; 
		case BACKWARD:
			printf("BACKWARD\n");
			// Travel BACKWARD @ speed s w/time t
			rc_motor_set(RIGHT, DC(BWD, s));
			rc_motor_set(LEFT, DC(BWD, s));
			rc_nanosleep(time);
			break;
		case TURN_RIGHT:
			// Turn Right
			printf("RIGHT\n");
			rc_motor_set(RIGHT, DC(BWD, s));
			rc_motor_set(LEFT, DC(FWD, s));
			rc_nanosleep(time);
			break;
		case TURN_LEFT: 
			printf("LEFT\n");
			rc_motor_set(RIGHT, DC(FWD, s));
			rc_motor_set(LEFT, DC(BWD, s));
			rc_nanosleep(time);
			break;
	// Cleanup
	rc_motor_set(RIGHT, (double) STP);
	rc_motor_set(LEFT, (double) STP);
	rc_motor_cleanup();
	}
	
}

double DC(enum direction d, enum speed s)
{
	double result =(double) d * s * 0.01;
    return result;
}
