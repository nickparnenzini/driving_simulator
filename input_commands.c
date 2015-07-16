#include "main.h"

struct car_t car; 

/**
@description: function used to turn the car on
@input: pointer to the shared structure 
*/
void ignition_on(struct car_t *c)
{
		/* gear mode (P,N,R,D) */
		int gear_mode; 
		
		pthread_mutex_lock(&c->mutex_car);

		/* turn the car on only in P gear mode */
		if(c->gear_mode == P)
		{
				/* wait until the brake pedal has been released */
				while(!keyrel(KEY_B));

				/* turn the car on*/
				c->state_car = ON; 
				/* rpm starting value */
				c->rpm =  RPM_START; 

				/* we create the thread to update vehicle dynamics */
				pthread_create(&tid[2], NULL, create_physics_update, (void*) c);

		}

		pthread_mutex_unlock(&c->mutex_car);

}


/**
@description: function used to turn the car off 
@input: shared structure 
*/
void ignition_off(struct car_t *c)
{
		pthread_mutex_lock(&c->mutex_car);

		/* check if the car is on */
		if(c->state_car == ON)
		{
			/* check if the car is in P gear mode */
			if(c->gear_mode == P)
			{
				/* set rpm to 0 */
				c->rpm = 0.0;
				/* set gear to NO_GEAR */
				c->gear = NO_GEAR; 
				/* turn the car off */
				c->state_car = OFF; 
			}
		}

		pthread_mutex_unlock(&c->mutex_car);

}

/**
@description: function used to accelerate 
@input: shared structure 
*/
void accelerate(struct car_t *c)
{
		pthread_mutex_lock(&c->mutex_car);

		/* check if the car is on */
		if(c->state_car == ON)
		{			
			/* check if the car is in D or in R mode (otherwise this command is not valid) */
			if((c->gear_mode == D) || (c->gear_mode == R))
			{
				/* set the brake to 0 */
				c->brake = 0; 

				/* check the throttle value */
				if(c->throttle <= 99)
					c->throttle++; 

			} 					
		}

		pthread_mutex_unlock(&c->mutex_car);

}


/**
@description: function used to decelerate 
@input: shared structure 
*/
void decelerate(struct car_t *c)
{
		pthread_mutex_lock(&c->mutex_car);

		/* check if the car is on */
		if(c->state_car == ON)
		{	
			/* check if the car is in D or in R mode (otherwise this command is not valid) */
			if((c->gear_mode == D) || (c->gear_mode == R))
			{
				/* check the throttle value */
				if(c->throttle >= 1)
					c->throttle--; 
			}
		}

		pthread_mutex_unlock(&c->mutex_car);

}

/**
@description: function used to shift to P 
@input: shared structure 
*/
void shift_park(struct car_t *c)
{		
		pthread_mutex_lock(&c->mutex_car);

		/* check if the car is stopped */
		if(c->speed == 0.0)
		{
			/* shift the gear mode to P */
			c->gear_mode = P;
			/* set gear to NO_GEAR */
			c->gear = NO_GEAR; 
		}			 

		pthread_mutex_unlock(&c->mutex_car);

}

/**
@description: function used to press the brake pedal 
@input: shared structure 
*/
void break_pedal(struct car_t *c)
{
	pthread_mutex_lock(&c->mutex_car);

	 /* check if the car is on */
	 if(c->state_car == ON)
	 {	 	
	 	/* check if the car is in D or in R mode (otherwise this command is not valid) */
	 	if((c->gear_mode == D) || (c->gear_mode == R))
		{
			/* if the cruise control is on, it is set to off by pressing the brake pedal */
		 	if(c->cruise_control == ON)
		 		c->cruise_control = OFF; 

		 	/* check the brake value */
		 	if(c->brake <= 99)
		 	{	
		 		/* throttle pedal is set to 0 */
		 		c->throttle = 0;
		 		/* brake value is up to date */
		 		c->brake++;  	  	
		 	}
	 	}
	 }

	 pthread_mutex_unlock(&c->mutex_car);

}

/**
@description: function used to reduce brake pedal angular position 
@input: shared structure 
*/
void reduce_brake(struct car_t *c)
{
	 pthread_mutex_lock(&c->mutex_car);

	 /* check if the car is on */
	 if(c->state_car == ON)
	 {
	 	/* check if the car is in D or in R mode (otherwise this command is not valid) */
	 	if((c->gear_mode == D) || (c->gear_mode == R))
	 	{
	 		/* check the brake value */
	 		if(c->brake >= 1)
	 			c->brake--; 
	 	}
	 }

	 pthread_mutex_unlock(&c->mutex_car);

}


/**
@description: function used to shift to R 
@input: shared structure 
*/
void shift_reverse(struct car_t *c)
{
		pthread_mutex_lock(&c->mutex_car);

		/* check if the car is on */
		if(c->state_car == ON)
		{
			 /* check if the car is stopped */
			 if(c->speed == 0.0)
			 {
			 		/* set gear mode to R */
			 		c->gear_mode = R; 
			 		/* set gear ratio to GR */
			 		c->cartype->gear_ratio = GR; 
			 		/* shift gear to 1 */
			 		c->gear = 1; 
			 }
		}

		pthread_mutex_unlock(&c->mutex_car);

}


/**
@description: function used to shift to N
@input: shared structure 
*/
void shift_neutral(struct car_t *c)
{
		pthread_mutex_lock(&c->mutex_car);

		if(c->speed == 0.0)
		{
			c->gear_mode = N;

			c->cartype->gear_ratio = 0;

			c->gear = NO_GEAR; 
		}
		
		pthread_mutex_unlock(&c->mutex_car);
}


/**
@description: function used to shift to D
@input: shared structure 
*/
void shift_drive(struct car_t *c)
{
		pthread_mutex_lock(&c->mutex_car);

		/* check if the car is on */
		if(c->state_car == ON)
		{			
			/* shift gear mode to D */
			c->gear_mode = D;
			/* set gear ratio to G1 */
			c->cartype->gear_ratio = G1;	
			/* shift gear to 1 */
			c->gear = 1;  			
		}

		pthread_mutex_unlock(&c->mutex_car);

}

/**
@description: function used to set the cruise control on 
@input: shared structure 
*/
void cruise_control_on(struct car_t *c)
{
		pthread_mutex_lock(&c->mutex_car);

		/* check if the car is on */
		if(c->state_car == ON)
		{
			/* set the cruise control on */
			c->cruise_control = ON;
			/* throttle pedal position is set to 0 */
			c->throttle = 0;
			/* brake pedal position is set to 0 */
			c->brake = 0; 

		}

		pthread_mutex_unlock(&c->mutex_car);

}

/**
@description: function used to set the cruise control off 
@input: shared structure 
*/
void cruise_control_off(struct car_t *c)
{
		pthread_mutex_lock(&c->mutex_car);

		/* check if the cruise control is on */
		if(c->state_car == ON)
			c->cruise_control = OFF;

		pthread_mutex_unlock(&c->mutex_car);

}

/**
@description: function used to change the wheels 
@input: shared structure 
*/
void change_wheels(struct car_t *c)
{
		/* value of diameters tyre */
		float tyre_diameter; 
		int valid = 0; 

		pthread_mutex_lock(&c->mutex_car);

		/* check if the car is off */
		if(c->state_car == OFF)
		{
			while(!valid)
			{
				printf("Please enter a tyre diameter (in m) : ");

				scanf("%f", &tyre_diameter);

				if(tyre_diameter <= 0.7)
				{
					  c->cartype->wheel_radius = tyre_diameter/2.0; 
					  printf("New value of wheel radius: %f\n", c->cartype->wheel_radius);
					  valid = 1; 
				}
				else
				{
					   printf("Please enter a valid number\n");
				}				

			}
			
		}

		pthread_mutex_unlock(&c->mutex_car);

}

/**
@description: function used to refill fuel
@input: shared structure 
*/
void refill_fuel(struct car_t *c)
{
		/* input value of gallons */
		float gallons = 0.0; 
		/* total fuel amount */
		float fuel_amount = 0.0;
		/* variable used to check if the input value is correct */
		int valid = 0; 

		pthread_mutex_lock(&c->mutex_car);

		/* check if the car is off */
		if(c->state_car == OFF)
		{
			while(!valid)
			{
					printf("Please enter amount of fuel to refill (in gallons): ");

					scanf("%f", &gallons);

					fuel_amount = c->fuel_level + gallons; 

					if(fuel_amount <= FUEL_TANK_CAPACITY)
					{
						c->fuel_level = fuel_amount;
						printf("Fuel level: %f\n", fuel_amount);
						valid = 1; 
					}
					else
					{
						printf("Please enter a valid number\n");
					}

			}
			
		}

		pthread_mutex_unlock(&c->mutex_car);

}

/**
@description: function used for input keyboard 
@input: shared structure 
*/
 void input_keyboard(struct car_t *c)
 { 
 		
		if(key[KEY_S]) 
			ignition_on(c); 

		if(key[KEY_O]) 
	  		ignition_off(c); 
	
		if(key[KEY_UP])
			accelerate(c);

		if(key[KEY_DOWN])
			decelerate(c); 

		if(key[KEY_B])
			break_pedal(c); 

		if(key[KEY_SPACE])
			reduce_brake(c);

		if(key[KEY_P])
			shift_park(c); 

		if(key[KEY_R])
			shift_reverse(c);

		if(key[KEY_N])
			shift_neutral(c);

		if(key[KEY_D])
			shift_drive(c);

		if(key[KEY_C])
			cruise_control_on(c);

		if(key[KEY_X])
			cruise_control_off(c);

		if(key[KEY_W])
			change_wheels(c);

		if(key[KEY_F])
			 refill_fuel(c);

		if(key[KEY_Q])
			end_simulation = 1; 

 }
