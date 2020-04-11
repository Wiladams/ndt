#include "blend2d.h"
#include <cstdio>

void main()
{
    BLImage img(640,480, BL_FORMAT_PRGB32);

    printf("img empty: %d\n", img.empty());
}