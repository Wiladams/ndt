#include "p5.hpp"
#include "Task.hpp"
#include <cstdio>

/*
    This tests whether a task can be created
    and draw concurrently with other things
    There is no coordination here, so if there
    were multiple threads, you could not 
    guarantee the order of drawing commands.
*/

DWORD liner(void *param)
{
    while(true) {
        background(0xc0);

        int x1 = random(0,width-1);
        int y1 = random(0,height-1);
        stroke(color(random(255), random(255), random(255)));

        // draw a chain of 200 line segments in the
        // random color
        for (int i =1; i<= 200; i++) {
            int x2 = random(0,width-1);
            int y2 = random(0,height-1);

            line(x1, y1, x2, y2);

            x1 = x2;
            y1 = y2;
        }

    }
    line(10,10,200,200);
    return 0;
}


Task *t1 = nullptr;

void setup()
{
    createCanvas(800,600);
    frameRate(30);

    t1 = new Task(liner);
    printf("t1.isValid(): %d\n", t1->isValid());

    t1->resume();
}