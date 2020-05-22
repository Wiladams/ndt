#include "random.hpp"

#include <cstdio>

TausPRNG rng(5);

void test_impliedRange()
{
    for (int i=1; i<=100;i++) {
        printf("%d - %d\n", i, rng.next(100));
    }
}

void test_explicitRange()
{
    for (int i=1; i<=100;i++) {
        printf("%d - %d\n", i, (int)rng.next(0,1));
    }
}

int main(int argc, char **argv)
{
    //test_impliedRange();
    test_explicitRange();

    return 0;
}