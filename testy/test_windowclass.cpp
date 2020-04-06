#include "NativeWindow.hpp"
#include <cstdio>

void main()
{
    char * classname = "mywinclass";

    User32WindowClass c1(classname, 0);

    printf("class name: 0x%p\n", classname);

    printf("c1.isValid(): %d name(%p): %s\n", c1.isValid(), c1.getName(), c1.getName());

    User32WindowClass c2(classname);

    if (!c2.isValid()) {
        printf("c2 NOT VALID: %d\n", c2.getLastError());
    } else {
        printf("c2 VALID,  name(%p): %s\n", c2.getName(), c2.getName());
    }
}