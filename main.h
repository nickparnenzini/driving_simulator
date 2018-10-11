#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <allegro.h>

/* thread periods definition (in milliseconds) */
#define PERIOD_VIEW_THREAD 40  
#define PERIOD_INPUT_THREAD 25
#define PERIOD_UPDATE_THREAD 15  

/* window dimensions */
#define MAX_LENGTH 500
#define MAX_WIDTH 500

/* number of data to plot */
#define MAX_OUTPUT 11

/* indexes of update_val array */
/* this array is used to plot data on the screen */
#define SPEED 1
#define GEAR_MODE 2
#define ODOMETER 3
#define CRUISE_CONTROL 4
#define RPM 5
#define CURRENT_GEAR 6
#define FUEL_LEVEL 7
#define STATE_CAR 8
#define THROTTLE 9
#define BRAKE 10

/* definition of some parameters used to set up the car */
#define NO_GEAR 9
#define ON 11
#define OFF 12
#define P 13
#define R 14
#define N 15
#define D 16

/* max gear value */
#define GEAR_MAX 6

/*rpm and torque static values */
#define RPM_START 1000.0
#define TORQUE_START 220.0
#define MIN_RPM 1000.0
#define MAX_RPM 7000.0
#define RPM_MAX_TORQUE 4500.0
#define RPM_SHIFT_GEAR 4000.0

#define TYRE_DIAMETER 0.6372 // m
#define MASS_CAR 1393.0 //Kg 

/* COSTANT VALUES FOR THE BRAKING MODEL */
#define K_c 1 // pressure gain
#define K_b 6.666 // (N*m)/Bar pressure/torque conversion constant for each brake system
#define alpha 0.01 // constant value 
#define tau_bs 0.2 // s reaction time delay

// Technical data for porsche boxter S

/* Fuel consumption: 
   -) city : 21 mpg 
   -) highway: 30 mpg 
*/

/* CONSTANT VALUES TO COMPUTE FRICTION FORCES */
#define	Cd 0.31	 		/* factor for air resistance (drag) 	*/
#define	Cr	0.03	/* factor for rolling resistance */
#define A 1.94 /* m^2 */
#define rho 1.29 /* Kg/m^3 */ /* air density */
#define g 9.81  /* m/s^2 */

#define FUEL_TANK_CAPACITY 14.0 /* gallons */

#define G1 3.82  /* first gear ratio */
#define G2 2.20
#define G3 1.52
#define G4 1.22
#define G5 1.02
#define G6 0.84
#define GR 2.90 /* reverse gear ratio */
#define Gd 3.44 /* differential gear ratio */

/* coefficients used to compute the maximum torque */
#define m1 0.0257 
#define m2 -0.0266
#define q1 194.286
#define q2 430.0

/* parameters used to compute fuel consumption */
#define CITY 30
#define HIGHWAY 31 

#define CITY_FUEL_EFFICIENCY 21.0 /* mpg */
#define HIGHWAY_FUEL_EFFICIENCY 30.0 /* mpg*/

/* delta value to update vehicle dynamics */
#define delta_t 0.015

pthread_t tid[3];

// Variables used to plot results */
char update_val[MAX_OUTPUT][100];

extern int x_pos;  
extern int y_pos[MAX_OUTPUT];  

/* allegro library variables */
BITMAP  *sprite,*buf; 
PALETTE colors;

/* struct used to save static parameters of the car */
typedef struct CARTYPE
{
	float gear_ratio;
	float differential_ratio;
	float wheel_radius;
	float mass;
} CARTYPE;

/* shared structure */
struct car_t
{
	//sem_t mutex_car; // mutex used for the structure
	pthread_mutex_t mutex_car; 

	CARTYPE *cartype; // pointer to static car data
	int state_car; // it can be ON or OFF
	int status; // it could be: ACC_STATUS, BRAKING_STATUS, CRUISE_STATUS 
	float speed;	// car speed
	float position; // car position
	float rpm;	// RPM value
	int gear_mode;  // it can be P,R,N,D 
	int gear;   // it can be 1,2,3,4,5,6
	int cruise_control;  // it can be ON or OFF 
	int throttle; // amount of throttle (input)
	int brake; // amount of brake (input)
	float fuel_level; // fuel level 
	float fuel_efficiency; // fuel efficiency (depending on CITY/HIGHWAY)
	float fuel_path; /* how many miles with a full tank */

}; 

extern struct car_t car; 

extern CARTYPE cartypes[1];

extern int end_simulation; 


// Time functions
struct timespec msec_to_timespec(unsigned int msec);
struct timespec timespec_add(struct timespec *t1, struct timespec *t2);
unsigned int timespec_to_msec(struct timespec *ts);
struct timespec timespec_diff(struct timespec *old_time, struct timespec *new_time);

// Input functions
void ignition_on(struct car_t *c);
void ignition_off(struct car_t *c);
void accelerate(struct car_t *c);
void decelerate(struct car_t *c);
void shift_park(struct car_t *c);
void break_pedal(struct car_t *c);
void reduce_brake(struct car_t *c);
void shift_reverse(struct car_t *c);
void shift_drive(struct car_t *c);
void cruise_control_on(struct car_t *c);
void cruise_control_off(struct car_t *c);
void change_wheels(struct car_t *c);
void refill_fuel(struct car_t *c);
void input_keyboard(struct car_t *c);

// Function used to check if a key has been released 
int keyrel(int k);

// Functions used to initialize the shared structure
void init_cartypes();
void initialize_car(struct car_t *c, CARTYPE *cartype);

// Functions used for vehicle dynamics 
float max_torque_computation(float rpm);
double compute_min(double val1, double val2);
float compute_braking_torque(float brake_input, float Vx, float wheel_radius);
float compute_torque_engine(struct car_t *c);
float compute_f_drag(float speed_val);
float compute_f_rr(float speed, float mass);
float compute_gear_ratio(int gear);
void compute_gear(struct car_t *c);
float compute_fuel_consumption(float position, float fuel_level, float fuel_efficiency);
float compute_v_max();
char compute_gear_mode(int gear_mode);

// Threads 
void *create_physics_update(void* arg);
void *create_view(void *arg);
void *create_input(void *arg);
void create_threads(struct car_t *c);