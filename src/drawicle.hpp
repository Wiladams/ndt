#include "computicle.hpp"
#include "PixelBuffer.hpp"

/*
    Drawicle
    A unit of drawing
    Implements PixelBuffer so it can be a stand
    in for any PixelBuffer
    Turns drawing API calls into serialized messages

    This can in turn be used by other mechanisms to 
    actually do drawing, or send the serialized commands
    elsewhere.
*/

class Drawicle : public PixelBuffer
{

};