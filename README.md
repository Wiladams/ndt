# ndt
Newgrammer Development Tools
============================

A newgrammer is someone who is new to programming.

Herein lies a set of tools that support newgrammer explorations in doing simple things.

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
