/**
 * @file drive_simple_encoder.c
 * This file drives the bot in a square using the encoders
 */

#include <stdio.h>
#include <robotcontrol.h> // includes ALL Robot Control subsystems
#include <signal.h>

// function declarations


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

static int RUNNING = 0;

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

	printf("RUNNING.....\n")

	printf("\nENCODER POSITIONS\n");
	printf("ENCODER 1 |");
	printf("ENCODER 2 |");
	printf(" \n");

	// Keep looping until state changes to EXITING
	rc_set_state(RUNNING);
	int i;
	while(rc_get_state()!=EXITING){
		// print encoder readings
		printf("\r");
		for(i = 1; i <=2; i++)
		{
			printf("%10d |", rc_encoder_read(i));
		}
		fflush("stdout");
		// quick sleep
		rc_nanosleep(100000);
	}

	// CLEANUP 
	if(rc_encoder_cleanup() == -1)
	{
		frpintf(stderr, "ERROR: ENCODER CLEANUP FAILED\n");
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

