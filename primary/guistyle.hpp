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




class GUIStyle
{
public:
    enum FrameStyle {
        Sunken = 0x01,
        Raised = 0x02
    };

    static inline uint8_t tobyte(double v) { return int(floor(v + 0.5)); }


    static inline Pixel brighter(const Pixel& value)
    {
        uint8_t red = tobyte(p5::constrain(value.r() * (1 / 0.80), 0, 255));
        uint8_t green = tobyte(p5::constrain(value.g() * (1.0 / 0.85), 0, 255));
        uint8_t blue = tobyte(p5::constrain(value.b() * (1.0 / 0.80), 0, 255));

        return Pixel(red, green, blue, value.a());
    }

    static inline Pixel darker(const Pixel& value)
    {
        uint8_t red = tobyte(value.r() * 0.60);
        uint8_t green = tobyte(value.g() * 0.60);
        uint8_t blue = tobyte(value.b() * 0.60);

        return Pixel(red, green, blue, value.a());
    }


    static void drawDropShadow(IGraphics& ctx, const BLRect& r, size_t maxOffset, Pixel& c)
    {
        auto shadow = c;

        ctx.push();
        ctx.noStroke();

        for (int i = 1; i <= maxOffset; i++)
        {
            auto alpha = p5::map(i, (double)1, (double)maxOffset, 20, 5);
            shadow.setA((uint32_t)alpha);
            ctx.fill(shadow);
            //ctx.rect(r.x + i, r.y + i, r.w, r.h);
            ctx.rect(r.x + i, r.y + i, r.w, r.h, 4, 4);
        }
        ctx.pop();
    }

protected:
    size_t fBorderWidth;


    Pixel fBaseColor;
    Pixel fForeground;
    Pixel fTextBackground;
    Pixel fHighlightColor;
    Pixel fShadowColor;
    Pixel fBackground;
    Pixel fBottomShadow;
    Pixel fBottomShadowTopLiner;
    Pixel fBottomShadowBottomLiner;
    Pixel fTopShadow;



public:
    GUIStyle(const Pixel&baseColor, const int borderWidth)
        : fBorderWidth(borderWidth)
    {
        setBaseColor(baseColor);

        fForeground = Pixel(0xffc0c0c0);


        fBottomShadow = darker(fForeground); // 0x00616161;
        fBottomShadowTopLiner = brighter(fBottomShadow); //fForeground;
        fBottomShadowBottomLiner = fBottomShadow;
        fTopShadow = brighter(fForeground);  // 0x00cbcbcb;

    }
    
    GUIStyle()
        : fBorderWidth(2)
    {
        setBaseColor(Pixel(0xffc0c0c0));


        fForeground = Pixel(0xffc0c0c0);


        fBottomShadow = darker(fForeground); // 0x00616161;
        fBottomShadowTopLiner = brighter(fBottomShadow); //fForeground;
        fBottomShadowBottomLiner = fBottomShadow;
        fTopShadow = brighter(fForeground);  // 0x00cbcbcb;
    }

    Pixel getSunkenColor() {return fForeground;}
    Pixel getRaisedColor() {return fForeground;}
    Pixel getBackground() {return fBackground;}

    Pixel getBaseColor() { return fBaseColor;}
    void setBaseColor(const Pixel&value)
    {
	    fBaseColor = value;
	    fTextBackground =   fBaseColor;
        fHighlightColor = brighter(fBaseColor);
	    fShadowColor = darker(fBaseColor);
	    fBackground = brighter(fHighlightColor);
    }

    Pixel getForeground() { return fForeground;}
    void setForeground(const Pixel&value){fForeground = value;}

    int getBorderWidth(){return (int)fBorderWidth;}
    void setBorderWidth(const int value) {fBorderWidth = value;}

    int getPadding() {return 2; }

    void drawFrame(IGraphics& ctx, int x, int y, int w, int h, int style)
    {
        if (style == GUIStyle::Sunken) {
            ctx.stroke(fHighlightColor);
            for (int n=0; n<getBorderWidth(); n++) {
                ctx.line(x+n, y+h-n, x+w-n, y+h-n);    // bottom shadow
                ctx.line(x + w - n, y + n, x + w - n, y + h);	    // right shadow
            }

            ctx.stroke(fShadowColor);
            for (int n=0; n < getBorderWidth(); n++) {
                ctx.line(x+n, y+n, x+w-n, y+n);     // top edge
                ctx.line(x+n, y+n, x+n, y+h-n);     // left edge
            }

        } else if (style == GUIStyle::Raised) {	

            ctx.stroke(fShadowColor);
            for (int n=0; n < getBorderWidth(); n++) {
                ctx.line(x+n, y+h-n, x+w-n, y+h-n);      // bottom shadow
                ctx.line(x+w-n, y+n, x+w-n, y+h);	    // right shadow
            }

            if (getBorderWidth() > 0) {
                ctx.stroke(fBottomShadowBottomLiner);
                ctx.line(x, y + h, x + w, y + h);				// bottom shadow
                ctx.line(x + w, y, x + w, y + h);				// right shadow
            }

            ctx.stroke(fHighlightColor);
            for (int n=0; n < getBorderWidth(); n++) {
                ctx.line(x+n,y+n, x+w-n, y+n);	    // top edge
                ctx.line(x+n, y+n, x+n, y+h-n);	    // left edge
            }
        }
    }

    void drawSunkenRect(IGraphics & ctx, int x, int y, int w, int h)
    {
        ctx.fill(fBaseColor);
        ctx.noStroke();
        ctx.rect(x,y,w,h);

        drawFrame(ctx, x, y, w, h, GUIStyle::Sunken);
    }

    void drawRaisedRect(IGraphics & ctx, int x, int y, int w, int h)
    {
        ctx.noStroke();
        ctx.fill(fBaseColor);
        ctx.rect(x,y,w,h);
        drawFrame(ctx, x, y, w, h, GUIStyle::Raised);
    }

};
