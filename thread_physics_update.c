#include "main.h"

/**
@description: thread used to update vehicle dynamics 
@input: pointer to the shared structure 

**/

void *create_physics_update(void* arg)
{
	/* engine torque value (torque at the output shaft of the engine) */
	float engine_torque = 0.0; 

	/* drive torque value */
	float torque_drive = 0.0; 

	/* drive force value */
	float f_drive = 0.0; 

	/* aerodynamic drag force */
	float f_drag = 0.0;

	/* rolling resistance value */
	float f_rr = 0.0; 

	/* longitudinal force */
	float f_long = 0.0; 

	/* acceleration value */
	float acc = 0.0;

	/* previous gear ratio before shifting gear */
	float old_gear_ratio = 0.0; 

	/* max speed */
	float v_max = compute_v_max(); 

	/* hared structure */
	struct car_t *c = (struct car_t *) arg;

	/* variable used to update the thread period */
	struct timespec T_input = msec_to_timespec(PERIOD_UPDATE_THREAD);

	struct timespec t, t_next; 

	clock_gettime(CLOCK_MONOTONIC, &t);    

	t_next = t;

	while(c->state_car == ON)
	{
			pthread_mutex_lock(&c->mutex_car);            

			if(c->fuel_level > 0.0)
			{
						/*****************************/
						/* compute the engine torque */  
						/*****************************/
						if((c->gear_mode == D) || (c->gear_mode == R))
						{
								/* if the car has just started and brake pedal is not pressed */
								if((c->rpm == RPM_START) && (c->brake == 0))
								{
									/* engine torque is equal to the starting torque */
									engine_torque = TORQUE_START;
								}
								else
								{
									/* compute the torque engine depending on the rpm value */
									engine_torque = compute_torque_engine(c); 
								}

						}
						else
						{
								/* in P and N gear mode, the car mustn't move */
								engine_torque = 0.0; 
						}				

						/* compute the torque transmitted to wheels */  
						torque_drive = (engine_torque)*(c->cartype->gear_ratio)*(c->cartype->differential_ratio);

						/* compute the force transmitted to wheels */			
						f_drive =  (float)(torque_drive/c->cartype->wheel_radius); 

						/* compute friction forces */

						/* drag force */
						f_drag = compute_f_drag(c->speed);

						/* rolling resistance force */
						f_rr = compute_f_rr(c->speed, c->cartype->mass); 

						/* longitudinal force computation */
						f_long = f_drive - f_drag - f_rr;

						/* acceleration computation */
						if(c->cruise_control == OFF)
						{
							acc = (float)(f_long/c->cartype->mass);
						}
						else
						{
							acc = 0.0; 
						}			

						/* speed update */
						c->speed += acc*delta_t; 

						/* check speed value */
						if(c->speed > v_max)
							c->speed = v_max; 

						if(c->speed < 0.0)
							c->speed = 0.0; 

						/* position update */
						c->position += c->speed*delta_t; 

						/* variable used to update fuel consumption */
						c->fuel_path = c->speed*delta_t;

						/* fuel level update */
						if(c->speed > 0.0)
							c->fuel_level = compute_fuel_consumption(c->fuel_path, c->fuel_level, c->fuel_efficiency); 

						if(c->fuel_level < 0.0)
							c->fuel_level = 0.0;

						/* rpm update */
						c->rpm = (c->speed*60.0*c->cartype->gear_ratio*c->cartype->differential_ratio)/(2.0*M_PI*c->cartype->wheel_radius); 

						/* check rpm value */
						if(c->rpm < MIN_RPM)
							c->rpm = MIN_RPM;

						if(c->rpm > MAX_RPM)
							c->rpm = MAX_RPM; 

						/* gear and rpm update */
						if(c->gear_mode == D)
							compute_gear(c); 	
							

			}	
			else
			{		
					/* if the car stops because refill fuel is required, then set rpm to 0 as well as speed and fuel path
					(fuel_path is a variable used to count miles done with the last refill value )
					*/
					c->rpm = 0.0;
					c->speed = 0.0;
					c->fuel_path = 0.0; 

			}
					     			
			//sem_post(&c->mutex_car);
			pthread_mutex_unlock(&c->mutex_car);   

			/*******************************************/
			/* last operations at the end of the cycle */
			/*******************************************/
			t_next = timespec_add(&t_next, &T_input);  

			clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t_next, NULL);


	}

	pthread_exit(NULL);

}