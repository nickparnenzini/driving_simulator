#include "main.h"

/*
@description: thread input to listen to input events
@input: pointer to the shared structure (struct car_t car)
@output: void
*/

void *create_input(void *arg)
{ 

	/* pointer to the shared structure */
	struct car_t *c = (struct car_t *) arg;

	/* time variable used to save the time period */
	struct timespec T_input = msec_to_timespec(PERIOD_INPUT_THREAD) ;

	struct timespec t, t_next; 

	/* absolute system clock */
	clock_gettime(CLOCK_MONOTONIC, &t);

	t_next = t; 

	while(1)
	{      
			/* control what button has been pressed */
	 		input_keyboard(c); 
	 			
			/* last operations at the end of the cycle */

			/* compute the new time at which the thread must be awake again */
			t_next = timespec_add(&t_next, &T_input);

			/* let thread sleep */
			clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t_next, NULL);

	}

	pthread_exit(NULL);
	
}