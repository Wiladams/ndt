#include "maths.hpp"
#include <cstdio>

int main()
{
    double r = lerp(255, 0, 0.33);
    double g = lerp(0, 0, 0.33);
    double b = lerp(0, 255, 0.33);

    printf("%3.2f, %3.2f, %3.2f\n", r, g, b);
    printf("%3d, %3d, %3d\n", (int)r, (int)g, (int)b);

    printf("(0.2*255) %d\n", (int)(0.2*255));
    return 0;
}