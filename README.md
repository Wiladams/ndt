# ndt
Newgrammer Development Tools
============================

A newgrammer is someone who is new to programming.

This repository contains examples of programming using the
C++ language on the Windows platform.  The exposure to Windows specifics is fairly minimal.  There is one combination of files which encapsulate putting a window on the screen and capturing user input:

    * apphost.h
    * appmain.cpp


Graphics are supported by the blend2d library.  Other than that, there are no other external dependencies.

In the directory structure, the '\primary' directory contains
a bunch of very useful stuff that makes the programming super
easy to play with.  The most direct and easy way to get started is to use the p5 combination of files

    * p5.hpp
    * p5.cpp

This will give you an environment wherein you only need to implement a couple of functions to put something on the screen.  It is very similar to the original Processing environment, or p5.js, but done in C++

```C++

void setup()
{
    createCanvas(640,480);
}


void draw()
{
    // draw a rectangle
    rect(10,10,100,100);
}
```




The code here is windows specific, which makes no attempt at being cross platform.  All windows specifics are encapsulated in the apphost/appmain combination though, so if you want to try your hand at doing cross platform things, porting those two files is pretty much all you would have to deal with.

The projects directory contains a Visual Studio solutions file which gives you ready access to all the coded up examples.

Examples
========

### analogclock
![analogclock](docs/images/timeflies.png=240x)
![analogclock](docs/images/timeflies.png?raw=true)
* Project: [analogclock](projects/analogclock)
* Notes: animation, transparency, rotation

### bannersketch
![bannersketch](docs/images/bannersketch.png?raw=true)
* Project: [bannersketch](projects/bannersketch)
* Notes: semi-transparent, full screen overlay

### calendar
![calendar](docs/images/calendar.png?raw=true)
* Project: [calendar](projects/calendar)
* Notes: Text of different sizes, calendar object

### colorama
![colorama](docs/images/colorama.png?raw=true)
* Project: [colorama](projects/colorama)
* Notes: rgb color from frequency, gamma adjust

### compkeyboard
![compkeyboard](docs/images/compkeyboard.png?raw=true)
* Project: [compkeyboard](projects/compkeyboard)
* Notes: transparent window, raw global keyboard input, data structures

### lerpcolor
![lerpcolor](docs/images/lerpcolor.png?raw=true)
* Project: [lerpcolor](projects/lerpcolor)
* Notes: Linear interpolation of color, fast polygon fill

### parimage
![parimage](docs/images/parimage.png?raw=true)</br>
* Project: [parimage](projects/parimage)
* Notes: pixel effects (masking, tinting)

### rainbow
![rainbow](docs/images/rainbow.png?raw=true)</br>
* Project: [rainbow](projects/rainbow)
* Notes: color blending, fullscreen, transparency

### screenplanes
![screenplanes](docs/images/screenplanes.png?raw=true)
* Project: [screenplanes](projects/screenplanes)
* Notes: pixel operations to separate color planes, screen capture

### shapes
![shapes](docs/images/shapes.png?raw=true)
* Project: [shapes](projects/shapes)
* Notes: drawing path based shapes

### sinwave
![sinwave](docs/images/sinewave.png?raw=true)
* Project: [sinwave](projects/sinwave)
* Notes: display sin and cosine functions

### squardle
![squardle](docs/images/squardle.png?raw=true)</br>
* Project: [squardle](projects/squardle)
* Notes: drop shadows, painter's algorithm drawing

### svgcolors
![svgcolors](docs/images/svgcolor.png?raw=true)</br>
* Project: [svgcolors](projects/svgcolors)
* Notes: Display colors from SVG palette

### targaviewer
![targaviewer](docs/images/targaviewer.png?raw=true)</br>
* Project: [targaviewer](projects/targaviewer)
* Notes: Display bitmap from .tga file

### wavemaker
![wavemaker](docs/images/wavemaker.png?raw=true)
* Project: [wavemaker](projects/wavemaker)
* Notes: follow the mouse, using trig functions

Compiling
=========
Install a relatively recent version of the Visual Studio environment, to get a Visual C++  compiler installed.  Currently
using Visual Studio 2019

Open up the projects.sln file in the \projects\projects.sln
Make sure you're in Release mode
Build the entire solution, play with examples

Creating a new project
======================
Need to link in: ws2_32.lib, blend2d.lib
Need to set Additional Lib Directories to ..\..\lib\Release

Need to set C/C++ Additional Include Directories - ..\..\primary
Need to set C++ runtime to /Mt


Bits And Bobs
=============



Most functions in ndt are encapsultated in classes that are in header files.  That means ndt does not have a 'library' that programs link against.  Each program is individual, and compiles down to a fairly small size.


Random References
==========


https://github.com/Wiladams/NewTOAPIA

C++ compiler intrinsics
https://docs.microsoft.com/en-us/cpp/intrinsics/compiler-intrinsics?view=vs-2019

VS Code c++ task configuration
https://code.visualstudio.com/docs/cpp/config-msvc


Things to try
=============
Processing and P5 worlds are full of great things to play with.

* http://haptic-data.com/toxiclibsjs/examples/force-directed-graph-p5
* https://natureofcode.com/book/


