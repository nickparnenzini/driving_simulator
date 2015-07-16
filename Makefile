
vehicle_simulator: time_functions.c keyrel_fun.c input_commands.c car_initialization.c vehicle_dynamics.c thread_input.c thread_view.c thread_physics_update.c main.c 
	gcc -o vehicle_simulator time_functions.c keyrel_fun.c input_commands.c car_initialization.c vehicle_dynamics.c thread_input.c thread_view.c thread_physics_update.c main.c `allegro-config --libs` -lpthread -lrt 

 
