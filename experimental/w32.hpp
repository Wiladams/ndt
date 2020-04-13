#pragma once

/*
    This single file is meant to be enough win32 to put a 
    window up on the screen and draw a pixel buffer into it.

    It contains most of the common int and pointer types, 
    and is aware of 32 and 64-bit versions.

    All manner of input; mouse, keyboard, touch, gesture, joystick, are supported.

    What's not in here are things like DirectX, Winmm, file system, 
    and the like.  The intention would be that you would take this 
    basic w32.hpp, include it in your project, then add the
    additional definitions and library linkages that you want for your 
    specific application needs.
*/

/*
// These pragma comments are here to ensure the linker will look
// at the specified libraries.  This makes it easier to compile/link
// single file '.cpp' applications without much trouble.
// Without these, the programmer will need to name the libraries
// specifically on the command line when compiling.
*/
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// The ordering of inclusion here should not change
// we MUST have w32_types defined before anything else
// and the funcs should be last.
#include "w32_types.hpp"
#include "w32_const.hpp"
#include "w32_funcs.hpp"
#include "w32_video.hpp"
#include "w32_socket.hpp"
#include "w32_procthread.hpp"


