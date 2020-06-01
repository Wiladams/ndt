#include "binstream.hpp"
#include "Graphics.h"
#include "p5.hpp"

#include <vector>
#include <cstdio>

// A class to connect a stream of Graphics commands
// to a Graphics interface that can execute them.
class GraphicsDecoder
{
    BinStream &fBS;
    IGraphics * fGraphics;

    // Some convenience readers
    uint16_t readCommand()
    {
        return fBS.readUInt16();
    }

    uint32_t readUInt()
    {
        return fBS.readUInt32();
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

    Pixel readColor()
    {
        Pixel c;
        c.value = fBS.readUInt32();

        return c;
    }

    size_t readString(char *buff, size_t buffLen)
    {
        return fBS.readStringZ(buffLen, buff); 
    }

public:
    
    GraphicsDecoder(BinStream &bs, IGraphics *g)
    :fBS(bs),
    fGraphics(g)
    {
    }

    void readNextCommand()
    {
        // First read the command
        uint32_t cmd = readCommand();
        //printf("NEXT: %d\n", cmd);

        // BUGBUG - maybe there should be a chunk
        // size next?

        switch (cmd) {
            case GCMD_NONE: {

            }
            break;

            case GCMD_ANGLEMODE:{
                fGraphics->angleMode(readUInt());
            }
            break;
            case GCMD_ELLIPSEMODE:{
                fGraphics->ellipseMode((ELLIPSEMODE)readUInt());
            }
            break;
            case GCMD_RECTMODE:{
                fGraphics->rectMode((RECTMODE)readUInt());
            }
            break;
            case GCMD_BLENDMODE:{
                fGraphics->blendMode(readUInt());
            }
            break;

            case GCMD_STROKECAPS: {
                fGraphics->strokeCaps(readUInt());
            }
            break;
            case GCMD_STROKEJOIN: {
                fGraphics->strokeJoin(readUInt());
            }
            break;
            case GCMD_STROKEWEIGHT: {
                fGraphics->strokeWeight((double)readFloat());
            }
            break;

            case GCMD_PUSH: {
                fGraphics->push();
            }
            break;
            case GCMD_POP: {
                fGraphics->pop();
            }
            break;

            case GCMD_TRANSLATE: {
                BLPoint xy = readCoord();
                fGraphics->translate(xy.x, xy.y);
            }
            break;
            case GCMD_SCALE: {
                BLPoint xy = readCoord();
                fGraphics->scale(xy.x, xy.y);
            }
            break;
            case GCMD_ROTATE: {
                float angle = readFloat();
                BLPoint xy = readCoord();

                fGraphics->rotate(xy.x, xy.y, angle);
            }
            break;

            case GCMD_FILL_COLOR: {
                BLRgba32 c = readColor();
                fGraphics->fill(c.r, c.g, c.r, c.a);
            }
            break;
            case GCMD_FILL_GRADIENT: {
                // BUGBUG - decode gradient
            }
            break;
            case GCMD_FILL_PATTERN: {
                // BUGBUG - decode gradient
            }
            break;
            case GCMD_FILL_NONE: {
                fGraphics->noFill();
            }
            break;

            case GCMD_STROKE: {
                BLRgba32 c = readColor();
                //printf("GraphicsDecoder.GCMD_STROKE: 0x%08x\n", c.value);

                fGraphics->stroke(c.r, c.g, c.b, c.a);
            }
            break;
            case GCMD_NOSTROKE: {
                fGraphics->noStroke();
            }
            break;

            case GCMD_FLUSH: {
                //fGraphics->flush();
            }
            break;
            case GCMD_LOADPIXELS: {
                // BUGBUG - loadPixels should be on Grpahics API
                //fGraphics->flush();
            }
            break;
            case GCMD_UPDATEPIXELS: {
                // BUGBUG
                // Graphics API should have updatePixels
                // fGraphics->updatePixels()
            }
            break;
            case GCMD_CLEAR: {
                fGraphics->clear();
            }
            break;

            case GCMD_BACKGROUND: {
                Pixel c = readColor();
                fGraphics->background(c);
            }
            break;
            case GCMD_CLIP:{
                BLRect r = readRect();
                fGraphics->clip(r.x, r.y, r.w, r.h);
            }
            break;
            case GCMD_NOCLIP: {
                fGraphics->noClip();
            }
            break;

            case GCMD_SET:{
                BLPoint xy = readCoord();
                Pixel c = readColor();
                fGraphics->set(xy.x, xy.y, c);
            }
            break;
            case GCMD_POINT: {
                BLPoint xy = readCoord();
                fGraphics->point(xy.x, xy.y);
            }
            break;
            case GCMD_LINE: {
                BLPoint xy1 = readCoord();
                BLPoint xy2 = readCoord();
                //printf("(%p) line: %f,%f %f,%f\n", fGraphics, xy1.x, xy1.y, xy2.x, xy2.y);
                fGraphics->line(xy1.x, xy1.y, xy2.x, xy2.y);
            }
            break;
            case GCMD_RECT: {
                BLRect rr = readRect();
                fGraphics->rect(rr.x,rr.y,rr.w,rr.h);
            }
            break;
            case GCMD_ROUNDRECT:{
                BLRect rr = readRect();
                BLPoint radii = readCoord();
                fGraphics->rect(rr.x,rr.y,rr.w,rr.h, radii.x, radii.y);
            }
            break;
            case GCMD_ELLIPSE: {
                BLPoint ab = readCoord();
                BLPoint cd = readCoord();
                fGraphics->ellipse(ab.x, ab.y, cd.x, cd.y);
            }
            break;
            case GCMD_CIRCLE: {
                BLPoint ab = readCoord();
                float dia = readFloat();
                fGraphics->circle(ab.x, ab.y, dia);
            }
            break;
            case GCMD_TRIANGLE: {
                BLPoint xy1 = readCoord();
                BLPoint xy2 = readCoord();
                BLPoint xy3 = readCoord();
                fGraphics->triangle(xy1.x, xy1.y, xy2.x, xy2.y, xy3.x, xy3.y);
            }
            break;
            case GCMD_BEZIER: {
                BLPoint xy1 = readCoord();
                BLPoint xy2 = readCoord();
                BLPoint xy3 = readCoord();
                BLPoint xy4 = readCoord();
                fGraphics->bezier(xy1.x, xy1.y, xy2.x, xy2.y, xy3.x, xy3.y, xy4.x, xy4.y);
            }
            break;
            case GCMD_POLYLINE: {
                uint32_t n = readUInt();
                std::vector<BLPoint> pts;

                for (size_t i=0; i<n; i++){
                    pts.push_back(readCoord());
                }
                fGraphics->polyline(pts.data(), pts.size());
            }
            break;
            case GCMD_POLYGON: {
                uint32_t n = readUInt();
                std::vector<BLPoint> pts;

                for (size_t i=0; i<n; i++){
                    pts.push_back(readCoord());
                }
                fGraphics->polygon(pts.data(), pts.size());
            }
            break;
            case GCMD_QUAD: {
                BLPoint xy1 = readCoord();
                BLPoint xy2 = readCoord();
                BLPoint xy3 = readCoord();
                BLPoint xy4 = readCoord();
                fGraphics->bezier(xy1.x, xy1.y, xy2.x, xy2.y, xy3.x, xy3.y, xy4.x, xy4.y);
            }
            break;

            case GCMD_IMAGE: {
                // BUGBUG - figure out decoding
                BLPoint dstxy = readCoord();
            }
            break;
            case GCMD_IMAGE_SCALE: {
                // BUGBUG - figure out decoding

                BLRect dstrect = readRect();
                BLRect srcArea = readRect();
            }
            break;

            case GCMD_TEXTALIGN: {
                uint32_t horiz = readUInt();
                uint32_t vert = readUInt();
                fGraphics->textAlign((ALIGNMENT)horiz, (ALIGNMENT)vert);
            }
            break;
            case GCMD_TEXTFONT: {
                char buff[1024];
                size_t buffLen = 1024;
                size_t nRead = readString(buff, buffLen);
                fGraphics->textFont(buff);
            }
            break;
            case GCMD_TEXTSIZE:{
                float size = readFloat();
                fGraphics->textSize(size);
            }
            break;
            case GCMD_TEXT: {
                char buff[1024];
                size_t buffLen = 1024;
                BLPoint xy = readCoord();
                size_t nRead = readString(buff, buffLen);
                fGraphics->text(buff, xy.x, xy.y);
            }
            break;

            case GCMD_BEGINSHAPE:{
                uint32_t kind = readUInt();
                fGraphics->beginShape((SHAPEMODE)kind);
            }
            break;
            case GCMD_VERTEX:{
                BLPoint xy = readCoord();
                fGraphics->vertex(xy.x, xy.y);
            }
            break;
            case GCMD_ENDSHAPE:{
                uint32_t endkind = readUInt();
                fGraphics->endShape((SHAPEEND)endkind);
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