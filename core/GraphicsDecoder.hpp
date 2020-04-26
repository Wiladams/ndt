#include "binstream.hpp"
#include "Graphics.h"
#include <vector>

// A class to connect a stream of Graphics commands
// to a Graphics interface that can execute them.
class GraphicsDecoder
{
    BinStream &fBS;
    IGraphics &fGraphics;

    // Some convenience readers
    uint32_t readCommand()
    {
        return fBS.readUInt32();
    }

    uint32_t readUInt()
    {
        fBS.readUInt32();
    }

    float readFloat()
    {
        return fBS.readFloat();
    }

    BLPoint readCoord()
    {
        float x = fBS.readFloat();
        float y = fBS.readFloat();
        return {x, y};
    }

    BLRect readRect()
    {
        float x = fBS.readFloat();
        float y = fBS.readFloat();
        float w = fBS.readFloat();
        float h = fBS.readFloat();

        return {x,y,w,h};
    }

    Color readColor()
    {
        return Color(fBS.readUInt32());
    }

    const char * readString()
    {
        char buff[1024];
        size_t buffLen = 1024;

        fBS.readStringZ(buffLen, buff); 
        return buff;
    }
public:
    GraphicsDecoder(BinStream &bs, IGraphics &g)
    :fBS(bs),
    fGraphics(g)
    {
    }

    void readNextCommand()
    {
        // First read the command
        uint32_t cmd = readCommand();
        // BUGBUG - maybe there should be a chunk
        // size next?

        switch (cmd) {
            case GCMD_NONE: {

            }
            break;

            case GCMD_ANGLEMODE:{
                fGraphics.angleMode(readUInt());
            }
            break;
            case GCMD_ELLIPSEMODE:{
                fGraphics.ellipseMode((ELLIPSEMODE)readUInt());
            }
            break;
            case GCMD_RECTMODE:{
                fGraphics.rectMode((RECTMODE)readUInt());
            }
            break;
            case GCMD_BLENDMODE:{
                fGraphics.blendMode(readUInt());
            }
            break;

            case GCMD_STROKECAPS:
                fGraphics.strokeCaps(readUInt());
            break;
            case GCMD_STROKEJOIN:
                fGraphics.strokeJoin(readUInt());
            break;
            case GCMD_STROKEWEIGHT:
                fGraphics.strokeWeight((int)readUInt());
            break;

            case GCMD_PUSH:
                fGraphics.push();
            break;
            case GCMD_POP:
                fGraphics.pop();
            break;

            case GCMD_TRANSLATE: {
                auto xy = readCoord();
                fGraphics.translate(xy.x, xy.y);
            }
            break;
            case GCMD_SCALE: {
                auto xy = readCoord();
                fGraphics.scale(xy.x, xy.y);
            }
            break;
            case GCMD_ROTATE: {
                float angle = readFloat();
                auto xy = readCoord();

                fGraphics.rotate(xy.x, xy.y, angle);
            }
            break;

            case GCMD_FILL_COLOR: {
                BLRgba c = readColor();
                fGraphics.fill(c.r, c.g, c.r, c.a);
            }
            break;
            case GCMD_FILL_GRADIENT:
                // BUGBUG - decode gradient
            break;
            case GCMD_FILL_PATTERN:
                // BUGBUG - decode gradient
            break;
            case GCMD_FILL_NONE:
                fGraphics.noFill();
            break;

            case GCMD_STROKE: {
                BLRgba c = readColor();
                fGraphics.stroke(c.r, c.g, c.r, c.a);
            }
            break;
            case GCMD_NOSTROKE:
                fGraphics.noStroke();
            break;

            case GCMD_FLUSH:
                //fGraphics.flush();
            break;
            case GCMD_LOADPIXELS:
                // BUGBUG - loadPixels should be on Grpahics API
                //fGraphics.flush();
            break;
            case GCMD_UPDATEPIXELS:
                // BUGBUG
                // Graphics API should have updatePixels
                // fGraphics.updatePixels()
            break;
            case GCMD_CLEAR:
                fGraphics.clear();
            break;

            case GCMD_BACKGROUND: {
                Color c = readColor();
                fGraphics.background(c);
            }
            break;
            case GCMD_CLIP:{
                auto r = readRect();
                fGraphics.clip(r.x, r.y, r.w, r.h);
            }
            break;
            case GCMD_NOCLIP:
                fGraphics.noClip();
            break;

            case GCMD_SET:{
                auto xy = readCoord();
                auto c = readColor();
                fGraphics.set(xy.x, xy.y, c);
            }
            break;
            case GCMD_POINT: {
                auto xy = readCoord();
                fGraphics.point(xy.x, xy.y);
            }
            break;
            case GCMD_LINE: {
                auto xy1 = readCoord();
                auto xy2 = readCoord();
                fGraphics.line(xy1.x, xy1.y, xy2.x, xy2.y);
            }
            break;
            case GCMD_RECT: {
                auto rr = readRect();
                fGraphics.rect(rr.x,rr.y,rr.w,rr.h);
            }
            break;
            case GCMD_ROUNDRECT:{
                auto rr = readRect();
                auto radii = readCoord();
                fGraphics.rect(rr.x,rr.y,rr.w,rr.h, radii.x, radii.y);
            }
            break;
            case GCMD_ELLIPSE:
                auto ab = readCoord();
                auto cd = readCoord();
                fGraphics.ellipse(ab.x, ab.y, cd.x,cd.y);
            break;
            case GCMD_CIRCLE: {
                auto ab = readCoord();
                float dia = readFloat();
                fGraphics.circle(ab.x, ab.y, dia);
            }
            break;
            case GCMD_TRIANGLE: {
                auto xy1 = readCoord();
                auto xy2 = readCoord();
                auto xy3 = readCoord();
                fGraphics.triangle(xy1.x, xy1.y, xy2.x, xy2.y, xy3.x, xy3.y);
            }
            break;
            case GCMD_BEZIER:
                auto xy1 = readCoord();
                auto xy2 = readCoord();
                auto xy3 = readCoord();
                auto xy4 = readCoord();
                fGraphics.bezier(xy1.x, xy1.y, xy2.x, xy2.y, xy3.x, xy3.y, xy4.x, xy4.y);
            break;
            case GCMD_POLYLINE: {
                auto n = readUInt();
                std::vector<BLPoint> pts;

                for (size_t i=0; i<n; i++){
                    pts.push_back(readCoord());
                }
                fGraphics.polyline(pts.data(), pts.size());
            }
            break;
            case GCMD_POLYGON: {
                                auto n = readUInt();
                std::vector<BLPoint> pts;

                for (size_t i=0; i<n; i++){
                    pts.push_back(readCoord());
                }
                fGraphics.polygon(pts.data(), pts.size());
            }
            break;
            case GCMD_QUAD: {
                auto xy1 = readCoord();
                auto xy2 = readCoord();
                auto xy3 = readCoord();
                auto xy4 = readCoord();
                fGraphics.bezier(xy1.x, xy1.y, xy2.x, xy2.y, xy3.x, xy3.y, xy4.x, xy4.y);
            }
            break;

            case GCMD_IMAGE: {
                // BUGBUG - figure out decoding
                auto dstxy = readCoord();
            }
            break;
            case GCMD_IMAGE_SCALE: {
                // BUGBUG - figure out decoding

                auto dstrect = readRect();
                auto srcArea = readRect();
            }
            break;

            case GCMD_TEXTALIGN: {
                auto horiz = readUInt();
                auto vert = readUInt();
                fGraphics.textAlign((ALIGNMENT)horiz, (ALIGNMENT)vert);
            }
            break;
            case GCMD_TEXTFONT: {
                auto fontfile = readString();
                fGraphics.textFont(fontfile);
            }
            break;
            case GCMD_TEXTSIZE:{
                float size = readFloat();
                fGraphics.textSize(size);
            }
            break;
            case GCMD_TEXT:
                auto xy = readCoord();
                auto txt = readString();
                fGraphics.text(txt, xy.x, xy.y);
            break;

            case GCMD_BEGINSHAPE:{
                auto kind = readUInt();
                fGraphics.beginShape((SHAPEMODE)kind);
            }
            break;
            case GCMD_VERTEX:{
                auto xy = readCoord();
                fGraphics.vertex(xy.x, xy.y);
            }
            break;
            case GCMD_ENDSHAPE:{
                auto endkind = readUInt();
                fGraphics.endShape((SHAPEEND)endkind);
            }
            break;
        }
    }

    void run()
    {
        while (!fBS.isEOF()) {
            readNextCommand();
        }
    }
};