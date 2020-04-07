
#include "computicle.hpp"
#include <cstdio>


void main()
{
    Computicle c1;

    for (int i = CM_USER; i<= CM_USER+100; i++)
    {
        //printf("sending: %d\n", i);
        c1.send(i);
    }

    c1.syncAndExit();
    //c1.sync();
    printf("SYNC\n");
    c1.join();
    printf("JOINED\n");
}