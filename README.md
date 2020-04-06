# ndt
Newgrammer Development Tools
============================

A newgrammer is someone who is new to programming.

This repository contains esoteric examples of programmin
in the Windows (Win32) environment.  As of 2020, this 
environment is being sunsetted for more modern approaches.  Although the future is upon us, there are
still plenty of lessons to be learned from how to keep 
simple things simple and make hard things possible.

This is a Windows specific set of code, which makes not attempt at working on any other platform.  It is meant as a teaching tool for those who are relatively new to programming.

The examples directory contains... examples of how to use various bits and pieces of the library.  Programs must be compiled to run.  The programs are meant to work with Visual Studio C++ compiler (cl).  The programs favor the 64-bit environment, and make no attempt to work for 32-bit environments.  

Compiling
=========
Install a relatively recent version of the Visual Studio environment, to get a Visual C++  compiler installed.

Bring up a command prompt for 64-bit development: 

x64 Native Tools Command Prompt

From here, cd into the directory where the code is to be compiled, like the examples directory, and type:

c:> cl /EHsc /I../src filename.cpp

Assuming your program is correct, you will be left with a filed:

filename.exe

Which is your executable.



Bits And Bobs
=============

ndt aims to be as independent as possible.  There are no external dependencies other than what is already provided on every Windows machine it will run on.  There is no dependency on the Windows SDK header files.  This ensures that a bare minimum coding environment will be sufficient to try things out.

Most functions in ndt are encapsultated in classes that are in header files.  That means ndt does not have a 'library' that programs link against.  Each program is individual, and compiles down to a fairly small size.


References
==========
C# .net framework code
https://referencesource.microsoft.com/

https://github.com/Wiladams/NewTOAPIA

C++ compiler intrinsics
https://docs.microsoft.com/en-us/cpp/intrinsics/compiler-intrinsics?view=vs-2019