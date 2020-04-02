#pragma once

#include "p5.hpp"

/*
* <dt> base
* <dd> The base color is the color that is used as the "dominant" color 
*      for graphical objects. For example, a button's text is drawn on top 
*      of this color when the button is "up".  
* <dt> highlight
* <dd> A lighter rendition of the base color used to create a highlight in 
*      pseudo 3D effects.
* <dt> shadow
* <dd> A darker rendition of the base color used to create a shadow in pseudo
*      3D effects.
* <dt> background
* <dd> The color used for background (or inset) <I>items</I> in a drawing 
*      scheme (rather than as a typical background area per se).  For example,
*      the background of a slider (the "groove" that the thumb slides in)
*      is drawn in this color.  [This name probably needs to be changed.]
* <dt>
* <dd> Note: the colors: base, highlight, shadow, and background are designed 
*      to be related, typically appearing to be the same material but with 
*      different lighting.
* <dt> foreground
* <dd> The color normally drawn over the base color for foreground items such
*      as textual labels.  This color needs to contrast with, but not clash 
*      with the base color.
* <dt> text_background
* <dd> The color that serves as the background for text editing areas.
* <dt> splash
* <dd> A color which is designed to contrast with, and be significantly
*      different from, the base, highlight, shadow, and background color 
*      scheme.  This is used for indicators such as found inside a selected
*      radio button or check box.
* </dl>
*/




#define byte(value) (floor(value+0.5))

#define clamp constrain

PixRGBA brighter(const PixRGBA &value)
{
    uint8_t red = byte(clamp(value.red *(1/0.80), 0, 255));
    uint8_t green = byte(clamp(value.green * (1.0/0.85), 0, 255));
    uint8_t blue = byte(clamp(value.blue * (1.0/0.80), 0,255));

    return PixRGBA(red, green, blue, value.alpha);
}

PixRGBA darker(const PixRGBA &value)
{
    uint8_t red = byte(value.red *0.60);
    uint8_t green = byte(value.green * 0.60);
    uint8_t blue = byte(value.blue * 0.60);

    return PixRGBA(red, green, blue, value.alpha);
}




class GUIStyle
{
public:
    enum FrameStyle {
        Sunken = 0x01,
        Raised = 0x02
    };

protected:
    size_t fBorderWidth;


    PixRGBA fBaseColor;
    PixRGBA fForeground;
    PixRGBA fTextBackground;
    PixRGBA fHighlightColor;
    PixRGBA fShadowColor;
    PixRGBA fBackground;
    PixRGBA fBottomShadow;
    PixRGBA fBottomShadowTopLiner;
    PixRGBA fBottomShadowBottomLiner;
    PixRGBA fTopShadow;



public:
    GUIStyle(const PixRGBA &baseColor, const int borderWidth)
        : fBorderWidth(borderWidth)
    {
        setBaseColor(baseColor);

        fForeground = colors.ltGray;


        fBottomShadow = darker(fForeground); // 0x00616161;
        fBottomShadowTopLiner = brighter(fBottomShadow); //fForeground;
        fBottomShadowBottomLiner = fBottomShadow;
        fTopShadow = brighter(fForeground);  // 0x00cbcbcb;

    }
    
    GUIStyle()
        : fBorderWidth(2)
    {
        setBaseColor(colors.ltGray);


        fForeground = colors.ltGray;


        fBottomShadow = darker(fForeground); // 0x00616161;
        fBottomShadowTopLiner = brighter(fBottomShadow); //fForeground;
        fBottomShadowBottomLiner = fBottomShadow;
        fTopShadow = brighter(fForeground);  // 0x00cbcbcb;
    }

    PixRGBA getSunkenColor() {return fForeground;}
    PixRGBA getRaisedColor() {return fForeground;}
    PixRGBA getBackground() {return fBackground;}

    PixRGBA getBaseColor() { return fBaseColor;}
    void setBaseColor(const PixRGBA &value)
    {
	    fBaseColor = value;
	    fTextBackground =   fBaseColor;
        fHighlightColor = brighter(fBaseColor);
	    fShadowColor = darker(fBaseColor);
	    fBackground = brighter(fHighlightColor);
    }

    PixRGBA getForeground() { return fForeground;}
    void setForeground(const PixRGBA &value){fForeground = value;}

    int getBorderWidth(){return fBorderWidth;}
    void setBorderWidth(const int value) {fBorderWidth = value;}

    PixRGBA getPadding() {return 2; }

    void drawFrame(int x, int y, int w, int h, int style)
    {
        if (style == GUIStyle::Sunken) {
            stroke(fHighlightColor);
            for (int n=0; n<getBorderWidth(); n++) {
                line(x+n, y+h-n, x+w-n, y+h-n);    // bottom shadow
                line(x + w - n, y + n, x + w - n, y + h);	    // right shadow
            }

            stroke(fShadowColor);
            for (int n=0; n < getBorderWidth(); n++) {
                line(x+n, y+n, x+w-n, y+n);     // top edge
                line(x+n, y+n, x+n, y+h-n);     // left edge
            }

        } else if (style == GUIStyle::Raised) {	

            stroke(fShadowColor);
            for (int n=0; n < getBorderWidth(); n++) {
                line(x+n, y+h-n, x+w-n, y+h-n);      // bottom shadow
                line(x+w-n, y+n, x+w-n, y+h);	    // right shadow
            }

            if (getBorderWidth() > 0) {
                stroke(fBottomShadowBottomLiner);
                line(x, y + h, x + w, y + h);				// bottom shadow
                line(x + w, y, x + w, y + h);				// right shadow
            }

            stroke(fHighlightColor);
            for (int n=0; n < getBorderWidth(); n++) {
                line(x+n,y+n, x+w-n, y+n);	    // top edge
                line(x+n, y+n, x+n, y+h-n);	    // left edge
            }
        }
    }

    void drawSunkenRect(int x, int y, int w, int h)
    {
        fill(fBaseColor);
        noStroke();
        rect(x,y,w,h);

        drawFrame(x, y, w, h, GUIStyle::Sunken);
    }

    void drawRaisedRect(int x, int y, int w, int h)
    {
        noStroke();
        fill(fBaseColor);
        rect(x,y,w,h);
        drawFrame(x, y, w, h, GUIStyle::Raised);
    }

};
