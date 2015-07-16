#include "main.h"

int keyrel(int k)
{
    static int initialized = 0;
    static int keyp[KEY_MAX];
 
    if(!initialized)
    {
        /* Set the keyp (key pressed) flags to false */
        int i;
        for(i = 0; i < KEY_MAX; i++) keyp[i] = 0;
        initialized = 1;
    }
 
    /* Now for the checking
       Check if the key was pressed
    */
    if(key[k] && !keyp[k])
    {
        /* Set the flag and return */
        keyp[k] = 1;
        return 0;
    }
    else if(!key[k] && keyp[k])
    {
        /* The key was released */
        keyp[k] = 0;
        return 1;
    }
    /* Nothing happened? */
    return 0;
    
}