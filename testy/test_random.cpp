#include "random.hpp"

#include <cstdio>

void main()
{
    TausPRNG rng(5);

    for (int i=1; i<=100;i++) {
        printf("%d - %d\n", i, rng.next(100));
    }
}