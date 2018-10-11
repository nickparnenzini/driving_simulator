#include "main.h"

/**
@description: function used to compute the max torque depending on the rpm value 
@input: rpm value 
@output: torque value 
**/
float max_torque_computation(float rpm)
{
      /* 2 lines to approximate the rpm/max_torque relation */
	  /* first line: m1 = 0.0257; q1 = 194.286 */
	  /* second line: m2 = -0.0266; q2 = 430.0 */

	   /* compute torque related to the rpm value */
	  float torque = 0.0;

	  /* first line */
	  if(rpm <= RPM_MAX_TORQUE)
	  {
	  		torque = m1*rpm + q1;
	  }

	  /* second line */
	  if(rpm > RPM_MAX_TORQUE)
	  {
	  		torque = m2*rpm + q2;
	  }

	  return torque; 

}


/**
@description: function used to comput the minimum value between two values
@input: two double numbers
@output: minimum value 
*/
double compute_min(double val1, double val2)
{
		/* minimum value */
		double min_val = 0.0; 

		if(val1 > val2)
			min_val = val2;
		else
			min_val = val1;

		return min_val; 

}


/**
@description: function used to compute the braking torque
@inputs: 
Vx: speed value of the car
wheel_radius: radius of the wheels 
@output:
braking_torque: value of the braking torque  
*/
float compute_braking_torque(float brake_input, float Vx, float wheel_radius)
{
		/* pressure applied to the break disk */
		float p_b = 0.0;
		/* braking torque */
		float braking_torque = 0.0;
		/* angular speed of traction wheels*/
		float angular_speed_wheel = Vx / wheel_radius; 
		/* brake value */
		float brake_value = brake_input * 1.2; /* from range 0/100 to range 0/120 */

		/* compute pressure applied to the break disk. We are supposing a range between 0 and 150 Bars */
		p_b = (float)(1.5 * K_c * brake_value)/(1 + tau_bs);

		/* braking torque computation */
		braking_torque = - p_b * K_b * compute_min(1.0, angular_speed_wheel/alpha); 

		return braking_torque; 

}


/**
@description: function used to compute the torque engine
@input: pointer to the shared structure 
@output: torque engine
*/
float compute_torque_engine(struct car_t *c)
{
	/* engine torque */
	float torque = 0.0; 
	/* max torque depending on the rpm value */
	float max_torque = 0.0;
	/* throttle position value */
	float throttle_position = 0.0;
	/* traction torque value */
	float traction_torque = 0.0;
	/* brake torque value */
	float brake_torque = 0.0;

	/*check if the throttle is pressed */
	if(c->throttle > 0)
	{
			/* compute the meax torque depending on the rpm value */
			max_torque = max_torque_computation(c->rpm);

			/* torque position normalization */
			throttle_position = (float)(c->throttle/100.0);

			/* traction torque value is up to the maximum torque */
			traction_torque = throttle_position*max_torque; 

			torque = traction_torque;

	}
	
	/* check if the brake is pressed */
	if(c->brake > 0)
	{
			/* compute the brake torque */
			brake_torque = compute_braking_torque(c->brake, c->speed, c->cartype->wheel_radius); 

			/* check if the speed is positive (otherwise the car would go reverse) */
			if(c->speed > 0.0)
				torque = brake_torque; 

	}

    return torque; 

}


/**
@description: function to compute the aerodynamic drag force 
@input: speed value 
@output: drag force 
*/
float compute_f_drag(float speed_val)
{
		/* drag force */
		float drag_force = 0.0;
		/* drag coefficient */
		float c_drag = A*rho*0.5;  

		/* drag force is only if speed is > 0 */
		if(speed_val > 0.0)
			drag_force =  c_drag*(speed_val*speed_val); 

		return drag_force; 

}


/**
@description: function to compute the rolling resistance 
@inputs:
speed: speed value
mass: mass value
@output: rolling resistance force 
*/
float compute_f_rr(float speed, float mass)
{
		/* rolling resistance coefficient */
		float f_rr_coefficient = 0.0;
		/* rolling resistance value */
		float rolling_resistance = 0.0;

		f_rr_coefficient = 0.01;

		/* there is rolling resistance only if the speed is > 0 */
		if(speed > 0.0)
			rolling_resistance =  f_rr_coefficient*mass*g;
			
		return rolling_resistance; 

}



/**
@description: function used to compute the gear ratio depending on the gear value 
@input: gear value
@output: gear ratio value 
*/
float compute_gear_ratio(int gear)
{
		float gear_ratio; 

		/* check the gear value */
		switch(gear)
		{
			case 1:
				gear_ratio = G1;	
				break;

			case 2:
				gear_ratio = G2; 
				break;

			case 3:
				gear_ratio = G3;
				break;

			case 4:
				gear_ratio = G4;
				break;

			case 5:
				gear_ratio = G5;
				break;

			case 6:
				gear_ratio = G6;
				break;

			case NO_GEAR:
				gear_ratio = 0; 
				break; 

			default:
				break; 

		}

		return gear_ratio; 

}



/**
@description: function used to shift the gear 
@input: shared structure 
*/
void compute_gear(struct car_t *c)
{
		/* maximum speed value for the current gear */
		float v_max = 0.0;
		/* maximum speed value for (current_gear - 1) */
		float v_max_lower_gear = 0.0;  
		/* denominator to compute the maximum speed  */
		float div_value = 0.0;  
		/* value of the lower gear */
		int lower_gear = 0; 
		/* gear ratio of the lower gear */
		float gear_ratio_lower_gear = 0.0;  
		/* denominator to compute the maximum speed of the lower gear */
		float div_value_lower_gear = 0.0; 
		/* gear ratio of the lower gear (before updating it) */
		float old_gear_ratio = c->cartype->gear_ratio;

		/* compute the denominator value */
		div_value = 60.0 * c->cartype->gear_ratio * c->cartype->differential_ratio;  

		/* compute the maximum and minimum speed for each gear */
		/* maximum speed value computation */
		v_max = (float)(c->cartype->wheel_radius * 2.0 * M_PI * RPM_SHIFT_GEAR )/div_value; 

		/* lower gear */
		lower_gear = c->gear - 1; 

		/* gear ratio for the lower gear */
		gear_ratio_lower_gear = compute_gear_ratio(lower_gear); 

		/* denominator value for the lower gear */
		div_value_lower_gear = 60.0 * gear_ratio_lower_gear * c->cartype->differential_ratio; 

		/* maximum speed for the lower gear */
		v_max_lower_gear = (float) (c->cartype->wheel_radius * 2.0 * M_PI * RPM_SHIFT_GEAR)/div_value_lower_gear; 

		/* check if upshift is needed */
		if((c->speed > v_max) && (c->gear < GEAR_MAX))
		{
				c->gear++;
		}
		else
		{    
			 /* downshift */
			 if(( c->speed < v_max_lower_gear ) && (c->gear > 1))
				  c->gear --;	
		}
				 
		/* gear ratio update */
		c->cartype->gear_ratio = compute_gear_ratio(c->gear); 

		/* rpm update after gear shift */
		c->rpm = (c->rpm*c->cartype->gear_ratio)/old_gear_ratio; 

}

/**
@description: function used to compute the fuel consumtpion
@inputs:
position: distance covered so far with the current refill value 
fuel_level: current fuel level value
fuel_efficiency: it changes considering highway or city
@output: fuel level update  
*/
float compute_fuel_consumption(float position, float fuel_level, float fuel_efficiency)
{
		/* gallons used */
		float gallons_used = 0.0;
		/* fuel in tank */
		float fuel_in_tank = 0.0;
		/* conversion to miles of the covered distance */
		float miles = (position*0.000621); /* conversion from m to miles */ 

		/* computation of the gallons used */
		gallons_used = miles/fuel_efficiency;   

		/* update of the fuel level */
		fuel_in_tank = fuel_level - gallons_used;

		return fuel_in_tank; 

}

/**
@description: function used to compute the maximum speed value 
@input: void
@output: maximum speed 
*/
float compute_v_max()
{
		/* maximum speed */
		float v_max = 0.0;
		/* numerator to compute the maximum speed */
		float num = 2.0*M_PI*MAX_RPM;
		/* maximum gear ratio */
		float max_gear_ratio = compute_gear_ratio(GEAR_MAX);
		/* denominator value */
		float div_val = 60.0*max_gear_ratio*Gd; 

		/* maximum speed computation */
		v_max = (float)(num/div_val);

		return v_max; 

}
