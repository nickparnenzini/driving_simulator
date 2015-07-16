#include "main.h"

/* shared structure for car parameters */
struct car_t car; 
/* variable used to end the simulation */
int end_simulation; 
/* variable x_pos, y_pos used to print the vehicle state */
int x_pos = 20;
int y_pos[MAX_OUTPUT] = {10, 40, 70, 100, 130, 160, 190, 220, 250, 280, 310};
/* struct used to describe static parameters of the car */
CARTYPE cartypes[1];

//**************************************************************************//
//                           create threads                                 //
//**************************************************************************//

void create_threads(struct car_t *c)
{

	/* create view thread (to plot output variables) */
	pthread_create(&tid[0], NULL, create_view, (void*) c);

	/* create input thread (to listen to keyboard events) */
	pthread_create(&tid[1], NULL, create_input, (void*) c);

}

//************************************************************************//
//                               MAIN                                     //
//************************************************************************//

int main()
{

	end_simulation = 0; 

	/* allegro library initialization */
	allegro_init();
	/* keyboard initialization */
	install_keyboard();

	/* 32 bit color depth definition */
	set_color_depth(32); 
	/* palette definition */
	set_palette(colors); 
	
	/* graphic mode initialization (MAX_LENGTH*MAX_WIDTH) window definition */
	if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, MAX_LENGTH, MAX_WIDTH, 0, 0) < 0) 
		exit(-1);

	/* definition of bitmap buffer dimensions */
	buf = create_bitmap(MAX_LENGTH, MAX_WIDTH); 
	/* clear the buffer */
	clear(buf); 

	/* structures initialization */
	init_cartypes();
	initialize_car(&car, &cartypes[0]);

	/* threads creation */
	create_threads(&car);	

	while (!key[KEY_ESC]); 

  	destroy_bitmap(buf);

	return 0; 

}