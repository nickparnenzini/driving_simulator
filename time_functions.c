#include "main.h"

/*****
@description: function used to convert from milliseconds to timespec structure 
(used to handle timers )
*/
struct timespec msec_to_timespec(unsigned int msec)
{
	struct timespec ts;

	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;

	return ts;
}


/*****
@description: function used to add milliseconds 
*/
struct timespec timespec_add(struct timespec *t1, struct timespec *t2)
{
	struct timespec ts;

	ts.tv_sec = t1->tv_sec + + t2->tv_sec;
	ts.tv_nsec = t1->tv_nsec + t2->tv_nsec;

	while (ts.tv_nsec >= 1E9) {
		ts.tv_nsec -= 1E9;
		ts.tv_sec++;
	}

	return ts;
}


/*****
@description: function used to convert from timespec to milliseconds 
*/
unsigned int timespec_to_msec(struct timespec *ts)
{
	return (ts->tv_sec * 1E9 + ts->tv_nsec) / 1000000;
}


/*****
@description: function used to compute a difference between two time values
*/
struct timespec timespec_diff(struct timespec *old_time, struct timespec *new_time)
{
	struct timespec ts; 

	if (new_time->tv_nsec < old_time->tv_nsec)
    {
            ts.tv_sec =  new_time->tv_sec - 1 - old_time->tv_sec; 
            ts.tv_nsec = 1E9 + new_time->tv_nsec - old_time->tv_nsec; 
    }
    else
    {
            ts.tv_sec =  new_time->tv_sec - old_time->tv_sec;
            ts.tv_nsec =  new_time->tv_nsec - old_time->tv_nsec; 
    }

    return ts; 

}