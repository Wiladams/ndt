Files sitting in here are probably throwaway

w32_* 
The various files starting with w32_* were an initial exploration in the minimal 
amount of Windows headers required to create capable Win32 applications.  As 
more of Windows is used, this becomes impractical, particular if we want to
incorporate DirectX and the like, so they are abandoned.

PixelBuffer*
Creating a PixelBuffer API from the bottom up.  This works well enough, and the 
concreate implementations of PixelBufferRGBA32, and 24 serve the purpose of being
a bridge between bitmap drawing and putting something up on the screen.  This
line of thinking was superceded by the creation of the Surface object, which 
does all the same, but relies on blend2d in the backend.

PixelOps
Little experiments in encapsultating pixel operations in functors.  They work
well enough, but are not as performant as what's already in the blend2d library
