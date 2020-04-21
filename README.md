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

There is "P5" like API available, and that makes it very easy to 
create fairly involved programs without much effort, so exploration
is relatively easy.

This is a Windows specific set of code, which makes not attempt at working on any other platform.  It is meant as a teaching tool for those who are relatively new to programming.

The projects directory contains a Visual Studio solutions file which
gives you ready access to all the coded up examples.

Compiling
=========
Install a relatively recent version of the Visual Studio environment, to get a Visual C++  compiler installed.

Open up the projects.sln file in the \projects directory
build the entire solution, play with examples



Bits And Bobs
=============

ndt aims to be as independent as possible.  There are no external dependencies other than the blend2d library, which is included.


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
