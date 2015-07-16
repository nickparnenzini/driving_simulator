#include "main.h"

CARTYPE cartypes[1];

/**
@description: function used to initialize static parameters of the car 
*/
void init_cartypes()
{
	CARTYPE	*cartype;

	cartype = &cartypes[0];

	cartype->gear_ratio = G1;
	cartype->differential_ratio = Gd;
	cartype->wheel_radius = (float)(TYRE_DIAMETER/2.0);
	cartype->mass = MASS_CAR; 

}

/**
@description: function used to initialize the shared structure 
*/
void initialize_car(struct car_t *c, CARTYPE *cartype)
{
	pthread_mutex_init(&c->mutex_car, NULL);

	c->cartype = cartype;

	c->state_car = OFF; 
	c->speed = 0.0;
	c->position = 0.0;
	c->rpm = 0.0;
	c->gear_mode = P;
	c->gear = NO_GEAR;
	c->cruise_control = OFF;
	c->throttle = 0;
	c->brake = 0; 
	c->fuel_level = FUEL_TANK_CAPACITY; 
	c->fuel_efficiency = CITY_FUEL_EFFICIENCY;  
	c->fuel_path = 0.0; 

}