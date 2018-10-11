#include "main.h"

/*
@description: function used to print the correct value depending on the gear mode
@input: gear_mode (it could be P,R,D,N)
@output: char value expressing the gear mode
*/

char compute_gear_mode(int gear_mode)
{
		/* char to save the gear mode of the car */
 		char gear_val;

 		/* check the gear mode */
 		switch(gear_mode)
		{
    		case P:
    			gear_val = 'P';
    			break;

    		case R:
    			gear_val = 'R';
    			break;

    		case N: 
    			gear_val = 'N';
    			break;

    		case D:
    			gear_val = 'D';
    			break;

    		default:
    			break;

		}

		return gear_val; 

}

// Thread view
void *create_view(void *arg)
{

	/* pointer to the shared structure */
	struct car_t *c = (struct car_t *) arg;

	/* time variable used to save the time period */
	struct timespec T_input = msec_to_timespec(PERIOD_VIEW_THREAD);

	struct timespec t, t_next; 

	/* variable used for a loop cycle */
	int i; 

	/* variable used to save the gear mode */
	char val_gear_mode; 

	/* string used to print some on/off values (about the car status and cruise control) */
	char on_off[4];

	strcpy(update_val[0], "VEHICLE STATE");
	strcpy(update_val[SPEED], "Speed: 0 Km/h");
	strcpy(update_val[GEAR_MODE], "Gear mode: P");
	strcpy(update_val[ODOMETER], "Odometer: 0 Km");
	strcpy(update_val[CRUISE_CONTROL], "Cruise control: off");
	strcpy(update_val[RPM], "RPM: 0");
	strcpy(update_val[CURRENT_GEAR], "Current gear: no gear");
	strcpy(update_val[FUEL_LEVEL], "Fuel level: 14 gallons");
	strcpy(update_val[STATE_CAR], "State car: OFF");
	strcpy(update_val[THROTTLE], "THROTTLE: 0");
	strcpy(update_val[BRAKE], "BRAKE: 0");

	/* save the current time */
	clock_gettime(CLOCK_MONOTONIC, &t);

	/* t_next is used to compute thread sleep time */
	t_next = t;

	/* until the simulation is active */
	while(!end_simulation)
	{		
			/* mutex used to have access to the shared structure */
			pthread_mutex_lock(&c->mutex_car);

			/* print speed value */
		    sprintf(update_val[SPEED], "Speed: %f mph", c->speed*2.237);  /* conversion from m/s to mph */

			/* compute the gear mode of the car */
		    val_gear_mode = compute_gear_mode(c->gear_mode);

		    /* print gear mode value */
			sprintf(update_val[GEAR_MODE], "Gear mode: %c", val_gear_mode);

			/* print odometer value */
			sprintf(update_val[ODOMETER], "Odometer: %f miles", c->position*0.000621);	/* conversion from m to miles: 1m = 0.000621 miles */

			/* check cruise control value */
			if(c->cruise_control == ON)
			{
				on_off[0] = 'O';
				on_off[1] = 'N';
				on_off[2] = '\0';
			}
			else
			{
				on_off[0] = 'O';
				on_off[1] = 'F';
				on_off[2] = 'F'; 
				on_off[3] = '\0';
			}

			/* print cruise control value */
			sprintf(update_val[CRUISE_CONTROL], "Cruise control: %s", on_off);		

			/* print rpm value */
			sprintf(update_val[RPM], "RPM: %f", c->rpm);			

			/* check gear value (if gear mode is P, its value is NO_GEAR )*/
			if(c->gear == NO_GEAR)
			{
				sprintf(update_val[CURRENT_GEAR], "Current gear: NO GEAR");	
			}
			else
			{
				sprintf(update_val[CURRENT_GEAR], "Current gear: %d", c->gear);	
			}
				
			/* print fuel level */
			sprintf(update_val[FUEL_LEVEL], "Fuel level: %f", c->fuel_level);

			/* check the state of the car */
			if(c->state_car == ON)
			{
				on_off[0] = 'O';
				on_off[1] = 'N';
				on_off[2] = '\0';
			}
			else
			{
				on_off[0] = 'O';
				on_off[1] = 'F';
				on_off[2] = 'F'; 
				on_off[3] = '\0';
			}

			/* print the state of the car */
			sprintf(update_val[STATE_CAR], "State car: %s", on_off);

			/* print throttle pedal value (from 0 to 100) */
			sprintf(update_val[THROTTLE], "THROTTLE: %d", c->throttle);

			/* print brake pedal value (from 0 to 100) */
			sprintf(update_val[BRAKE], "BRAKE: %d", c->brake);

			pthread_mutex_unlock(&c->mutex_car);

			/* loop to update output values */
			for(i = 0; i < MAX_OUTPUT; i++)
			{
				textout_ex(buf, font, update_val[i], x_pos, y_pos[i], makecol(255,0,0), -1);
			    blit(buf, screen, x_pos, y_pos[i], x_pos, y_pos[i], MAX_LENGTH, MAX_WIDTH); 
			   
			}				

			clear(buf);

			/* last operations at the end of the cycle */
			t_next = timespec_add(&t_next, &T_input);

			clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t_next, NULL);

	}

	pthread_exit(NULL);

}