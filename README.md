# ndt
Newgrammer Development Tools
============================

A newgrammer is someone who is new to programming.

This repository contains examples of programming using the
C++ language on the Windows platform.  Although the platform is
Windows, the graphics subsystem is primarily supported by
blend2d for vector graphics.

At the moment, the projects do not leverage much of WinRT, but
it does not preclude its usage either.

For most graphics programming, the easiest thing to do is use the
p5.hpp/.cpp files.  this gives you a ready made environment which 
supports mouse, keyboard, joystick, touch, 2D vector graphics, and 
a stopwatch, out of the box, before you write a single line of code.

This makes for easy exploration.  Most other functions are included
in .hpp files, so there's no additional libraries to drag in to try
out simple things.

This is a Windows specific set of code, which makes not attempt at 
working on any other platform.  It is meant as a teaching tool for 
those who are relatively new to programming.

The projects directory contains a Visual Studio solutions file which
gives you ready access to all the coded up examples.

Compiling
=========
Install a relatively recent version of the Visual Studio environment, to get a Visual C++  compiler installed.

Open up the projects.sln file in the \projects directory
build the entire solution, play with examples

Creating a new project
======================
Need to link in: ws2_32.lib, blend2d.lib
Need to set Additional Lib Directories

Need to set C/C++ Additional Include Directories
Need to include projects\lib\Release in library path
Need to set C++ runtime to /Mt


Bits And Bobs
=============

ndt aims to be as independent as possible.  With the exception of the blend2d vector graphics library, there are no other dependencies.  the intention is to create 
various functions from scratch, even though they typically already exist.
Besides creating things from scratch, the environment is rich enough that playing
with those creations should be relatively easy.


Most functions in ndt are encapsultated in classes that are in header files.  That means ndt does not have a 'library' that programs link against.  Each program is individual, and compiles down to a fairly small size.


Random References
==========

C# .net framework code
https://referencesource.microsoft.com/

https://github.com/Wiladams/NewTOAPIA

C++ compiler intrinsics
https://docs.microsoft.com/en-us/cpp/intrinsics/compiler-intrinsics?view=vs-2019

VS Code c++ task configuration
https://code.visualstudio.com/docs/cpp/config-msvc


Things to try
=============
Processing and P5 worlds are full of great things to play with.

http://haptic-data.com/toxiclibsjs/examples/force-directed-graph-p5


Interesting Books
=================
https://natureofcode.com/book/
