#pragma once

/*
    Commentary:  William Adams
    Targa is a fairly ancient file format.  It was primary in the 1980s - 1990s
    then supplanted by more sophisticated formats in later years.  
    
    I have written various versions of Targa parsers, staring around 2006 with
    one written in C#, from the specifications.  That one was then re-implemented
    in roughly 2015 with a LuaJIT version.

    This current C++ version was taken from the Lua and C# code, and redone to 
    be even more complete than those two versions were.  Of particular note, 
    figuring out orientation (left,right, top/bottom) is handled by the LocIterator
    class.  This vastly simplifies the positional nature of the code, which
    makes it much easier to focus on the decoding of pixel values.

    There are also iterators for reading compressed pixels and uncompressed pixels.

    One function decodeSinglePixel() takes care of decoding a pixel value.  This
    is used for anything having to do with color, from creating a colormap to 
    decoding compressed or uncompressed pixels. 

    All combined, this composable approach makes it a bit easier to decode all
    cases, whether they be oriented one way or another, compressed, un-compressed
    or any color depth.

    As of May 2020, this decoder has been tested with about 100 different .tga images
    taken from across the internet.  All typical test cases work, as well as some
    more obscure pathological ones.  The only case that does not work
    are interleaved images, which I have not been able to find any actual instances
    of.  So, I'm going to call it complete.

    It can benefit from some better error handling when the image is malformed, but
    besides that, it's fairly robust.

    References
    One of these references should be valid for some time

    http://www.fileformat.info/format/tga/egff.htm#TGA-DMYID.2
    http://www.dca.fee.unicamp.br/~martino/disciplinas/ea978/tgaffs.pdf
    
-- library of congress
    https://www.loc.gov/preservation/digital/formats/fdd/fdd000180.shtml

-- other implementations
    https://unix4lyfe.org/targa/
    https://github.com/ftrvxmtrx/tga
    https://github.com/ssloy/tinyrenderer/
*/

/*
    Honors the following properties
    vertical orientation
    horizontal orientation
    truecolor (16, 24, 32)
    indexed (16, 24, 32)
    monochrome (8)
    compressed - RLE, RAW

    We use blend2d, but the only connection is the BLRgba
    type, which holds a color value.
*/


#include "blend2d.h"
#include "bitbang.h"
#include "filestream.h"

#include <memory>

namespace targa {
    //    Convenience structures
    enum ColorMapType : int {
        None = 0,
        Palette = 1
    };

    enum HorizontalOrientation : int {
        LeftToRight = 0,
        RightToLeft = 1
    };

    enum VerticalOrientation : int {
        BottomToTop = 0,
        TopToBottom = 1,
    };

    enum Interleave : int {
        non_interleaved = 0,
        two_way = 1,
        four_way = 2,
        reserved = 3
    };

    enum ImageKind : int {
        NoImageData = 0,
        ColorMapped = 1,
        TrueColor = 2,
        Monochrome = 3,
        ColorMappedCompressed = 9,
        TrueColorCompressed = 10,
        MonochromeCompressed = 11,
    };


    static const int footerSize = 26;
    static const char* targaXFileID = "TRUEVISION-XFILE.";

    // Used for location iteration
    struct Location
    {
        int x, y;
    };
    
    struct tgaFooter {
        uint32_t    ExtensionAreaOffset;
        uint32_t    DeveloperDirectoryOffset;
        char        Signature[18];

        // calculated
        bool isExtended;
    };

    // This is NOT suitable for direct memory
    // reading.
    struct tgaHeader {
        uint8_t     IDLength;
        uint8_t     ColorMapType;
        uint8_t     ImageType;
        uint16_t    CMapStart;
        uint16_t    CMapLength;
        uint8_t     CMapDepth;
        BLRgba32* ColorMap;          // allocated
        bool        Compressed;         // calculated

        // Image information
        uint16_t    XOffset;
        uint16_t    YOffset;
        uint16_t    Width;
        uint16_t    Height;
        uint8_t     PixelDepth;         // bits per pixel
        uint8_t     BytesPerPixel;      // calculated
        uint8_t     ImageDescriptor;
        uint8_t* ImageIdentification;  // allocated

        // Calculated
        int AttrBits;
        int HorizontalOrientation;
        int VerticalOrientation;
        int Interleave;
    };

    struct TargaMeta {
        tgaHeader header;
        tgaFooter footer;
    };


    static bool decodeSinglePixel(BLRgba32& pix, uint8_t* databuff, int pixelDepth, int imtype, BLRgba32* colorMap)
    {
        //printf("decodeSinglePixel: type: %d bpp:%d\n", imtype, pixelDepth);

        if ((imtype == TrueColor) || (imtype == TrueColorCompressed)) {
            if (pixelDepth == 24) {
                pix.setR(databuff[2]);
                pix.setG(databuff[1]);
                pix.setB(databuff[0]);
                pix.setA(255);
                return true;
            }
            else if (pixelDepth == 32) {
                pix.setR(databuff[2]);
                pix.setG(databuff[1]);
                pix.setB(databuff[0]);
                pix.setA(databuff[3]);

                // some images use alpha as transparency
                // some images do not
                // when they don't use it, they might set it to '0'
                // which will make the pixels transparent
                // we need to be able to distinguish between these
                // two states
                //printf("pix.alpha: %d\n", pix.alpha);
                //pix.alpha = databuff[3];   // BUGBUG - We should pre-multiply the alpha?
                return true;
            }
            else if (pixelDepth == 16) {
                uint16_t src16 = ((databuff[1] << 8) | databuff[0]);
                pix.setB((binops::BITSVALUE(src16, 0, 4) << 3));
                pix.setG((binops::BITSVALUE(src16, 5, 9) << 3));
                pix.setR((binops::BITSVALUE(src16, 10, 14) << 3));
                pix.setA(255);
                if (binops::BITSVALUE(src16, 15, 15) >= 1) {
                    pix.setA(0);  // 255
                }
            }

            return true;
        }
        else if ((imtype == Monochrome) || (imtype == MonochromeCompressed)) {
            pix.setR(databuff[0]);
            pix.setG(databuff[0]);
            pix.setB(databuff[0]);
            pix.setA(255);

            return true;
        }
        else if ((imtype == ColorMapped) || (imtype == ColorMappedCompressed)) {
            // lookup the color using databuff[0] as index
            pix = colorMap[databuff[0]];

            return true;
        }

        return false;
    }

    static BLRgba32* readColorMap(BinStream& bs, tgaHeader& header)
    {
        size_t bytespe = header.CMapDepth / 8;     // bytes per entry
        uint8_t databuff[16];

        BLRgba32* cMap = { new BLRgba32[header.CMapLength]{} };

        for (int i = header.CMapStart; i < header.CMapLength; i++) {
            size_t nRead = bs.readBytes(databuff, bytespe);

            decodeSinglePixel(cMap[i], databuff, header.CMapDepth, TrueColor, nullptr);
        }

        return cMap;
    }

    static bool readHeader(BinStream& bs, tgaHeader& res)
    {
        res.IDLength = bs.readOctet();           // 00h  Size of Image ID field
        res.ColorMapType = bs.readOctet();       // 01h  Color map type 
        res.ImageType = bs.readOctet();          // 02h  Image type code
        res.CMapStart = bs.readUInt16();         // 03h  Color map origin
        res.CMapLength = bs.readUInt16();        // 05h  Color map length
        res.CMapDepth = bs.readOctet();          // 07h  Depth of color map entries
        res.Compressed = res.ImageType == ColorMappedCompressed ||
            res.ImageType == TrueColorCompressed ||
            res.ImageType == MonochromeCompressed;

        // Image Description
        res.XOffset = bs.readUInt16();           // 08h  X origin of image
        res.YOffset = bs.readUInt16();           // 0Ah  Y origin of image
        res.Width = bs.readUInt16();             // 0Ch  Width of image - Maximum 512
        res.Height = bs.readUInt16();            // 0Eh  Height of image - Maximum 482
        res.PixelDepth = bs.readOctet();         // 10h  Number of bits per pixel
        res.BytesPerPixel = res.PixelDepth / 8;

        res.ImageDescriptor = bs.readOctet();    // 11h  Image descriptor byte
        res.AttrBits = (res.ImageDescriptor & 0x0F);
        res.HorizontalOrientation = ((res.ImageDescriptor & 0x10) >> 4);
        res.VerticalOrientation = ((res.ImageDescriptor & 0x20) >> 5);
        res.Interleave = ((res.ImageDescriptor & 0xC0) >> 6);


        // If there's an identification section, read that next
        //print("ImageIdentification: ", res.IDLength, string.format("0x%x",bs.tell()))
        if (res.IDLength > 0) {
            res.ImageIdentification = { new uint8_t[res.IDLength]{} };
            bs.readBytes(res.ImageIdentification, res.IDLength);
        }

        // If there's a color map, read that next
        if (res.ColorMapType == Palette) {
            res.ColorMap = readColorMap(bs, res);
        }

        return true;
    }


    /*
         Targa images come in many different formats, and there are
         a couple of different versions of the specification.

        First thing to do is determine if the file is adhereing to version
        2.0 of the spcification.  We do that by reading a 'footer', which
        is the last 26 bytes of the file.

        Return a PixelBuffer if we can read the file successfully.
    */

    static bool readFooter(BinStream& bs, tgaFooter& rs)
    {
        //print("targa.readFooter, BEGIN")

        rs.ExtensionAreaOffset = bs.readUInt32();
        rs.DeveloperDirectoryOffset = bs.readUInt32();
        bs.readBytes((uint8_t*)rs.Signature, 18);

        rs.isExtended = !strncmp(rs.Signature, targaXFileID, strlen(targaXFileID));

        //printf("readFooter(), signature: %s\n", rs.Signature);
        //printf("isExtended: %d\n", rs.isExtended);

        if (!rs.isExtended) {
            return false;
        }

        //print("targa.readFooter, END")
        return true;
    }

    /*
     We want to figure out the mapping between positions as we
     read them and their locations in our pixel buffer in one place
     This iterator figures that out, return x,y pairs for the positions
     according to the horizontal and vertical orientation.  Ideally this
     would do interleaving as well, but we don't have an image to test
     with
    */
    class LocIterator //: IEnumerator<Location>
    {
        tgaHeader fHeader;

        int xSign, xStart, xEnd, xincr;
        int ySign, yStart, yEnd, yincr;

    public:
        LocIterator(const tgaHeader& header)
            :fHeader(header)
        {
            // Start with left to right orientation
            xSign = 1;
            xStart = 0;
            xEnd = header.Width - 1;
            xincr = -1;

            // switch to right to left of horizontal orientation indicates
            if (header.HorizontalOrientation == RightToLeft) {
                xSign = -1;
                xStart = header.Width - 1;;
                xEnd = 0;
            }

            // start top to bottom vertical orientation
            ySign = 1;
            yStart = 0;
            yEnd = header.Height - 1;
            yincr = 0;

            // Switch to bottom to top if necessary
            if (header.VerticalOrientation == BottomToTop) {
                ySign = -1;
                yStart = header.Height - 1;
                yEnd = 0;
            }

            reset();
        }

        virtual bool moveNext()
        {
            xincr = xincr + 1;
            if (xincr == fHeader.Width)
            {
                xincr = 0;
                yincr = yincr + 1;
                if (yincr == fHeader.Height) {
                    return false;
                }
            }

            return true;
        }

        virtual void reset()
        {
            xincr = -1;
            yincr = 0;
        }

        virtual Location getCurrent() const
        {
            return { xStart + (xSign * xincr), yStart + (ySign * yincr) };
        }

        // getCurrent()
        Location operator *() { return getCurrent(); }
    };

    /*
    // An iterator which will return the uncompressed
    // pixels in row order
    */
    class PixelsUncompressed //: IEnumerator<BLRgba32>
    {
        const TargaMeta& fMeta;
        int bytesPerPixel;
        uint8_t databuff[16];     // Maximum number of bytes per pixel
        int xStart, xEnd, xincr;
        int yStart, yEnd, yincr;
        BinStream& bs;
        BLRgba32 pix;

    public:
        PixelsUncompressed(BinStream& abs, const TargaMeta& meta)
            : fMeta(meta),
            bs(abs)
        {
            bytesPerPixel = meta.header.BytesPerPixel;
            reset();
        }

        bool moveNext() {
            // BUGBUG - really we could just count pixels
            // the actual location doesn't matter here
            xincr = xincr + 1;
            if (xincr >= fMeta.header.Width) {
                xincr = 0;
                yincr = yincr + 1;
                if (yincr >= fMeta.header.Height) {
                    return false;
                }
            }

            // Get the pixel data
            size_t nRead = bs.readBytes(databuff, (size_t)fMeta.header.BytesPerPixel);

            decodeSinglePixel(pix, databuff, fMeta.header.PixelDepth, fMeta.header.ImageType, fMeta.header.ColorMap);

            return true;
        }

        void reset() { xincr = -1; yincr = 0; }

        const BLRgba32 & getCurrent() const { return pix; }
    };

    // An enumerator of compressed pixels
    class PixelsCompressed //: IEnumerator<BLRgba32>
    {
        TargaMeta fMeta;
        int bytesPerPixel;
        uint8_t databuff[16];     // Maximum number of bytes per pixel
        BinStream& bs;
        BLRgba32 pix;
        int pixelCount;
        bool isRLE;


    public:
        PixelsCompressed(BinStream& abs, const TargaMeta& meta)
            : fMeta(meta),
            bs(abs)
        {
            bytesPerPixel = meta.header.BytesPerPixel;

            reset();
        }

        bool moveNext()
        {
            if (pixelCount == 0) {
                // read packet type to see if it's RLE or RAW
                int packet = bs.readOctet();
                isRLE = (packet & 0x80) > 0; // isset(repCount, 7);
                pixelCount = (int)binops::BITSVALUE(packet, 0, 6) + 1;
                //printf("pixelCount: %d\n", pixelCount);
                // Read at least one pixel value
                size_t nRead = bs.readBytes(databuff, (size_t)bytesPerPixel);
                decodeSinglePixel(pix, databuff, fMeta.header.PixelDepth, fMeta.header.ImageType, fMeta.header.ColorMap);
            }
            else {
                // We're still in a run
                if (!isRLE) {
                    // read another pixel
                    size_t nRead = bs.readBytes(databuff, bytesPerPixel);
                    decodeSinglePixel(pix, databuff, fMeta.header.PixelDepth, fMeta.header.ImageType, fMeta.header.ColorMap);
                }
            }
            pixelCount = pixelCount - 1;

            return true;
        }

        const BLRgba32 & getCurrent() const { return pix; }

        void reset() {
            pixelCount = 0;
        }
    };


    static bool readBody(BinStream& bs, const TargaMeta& meta, BLImage &img)
    {
        blImageInitAs(&img, meta.header.Width, meta.header.Height, BL_FORMAT_PRGB32);
        BLImageData imageData;
        img.getData(&imageData);

        LocIterator li(meta.header);

        if (!meta.header.Compressed) {
            PixelsUncompressed pi(bs, meta);

            while (li.moveNext()) {
                Location loc = li.getCurrent();
                if (!pi.moveNext()) {
                    break;
                }

                auto & c = pi.getCurrent();
                //lpb->setPixel(loc.x, loc.y, c);
                ((BLRgba32*)(imageData.pixelData))[(int)loc.y * img.width() + (int)loc.x] = c;
            }
        }
        else {
            PixelsCompressed pi(bs, meta);

            while (li.moveNext()) {
                Location loc = li.getCurrent();


                if (!pi.moveNext()) {
                    break;
                }

                BLRgba32 c = pi.getCurrent();
                //lpb->setPixel(loc.x, loc.y, c);
                size_t offset = (int)loc.y* img.width() + (int)loc.x;
                //printf("loc: %d, %d  Offset: %d\n", loc.x, loc.y, offset);

                ((BLRgba32*)(imageData.pixelData))[offset] = c;
            }
        }

        return true;
    }

    static bool readMetaInformation(BinStream& bs, TargaMeta& meta)
    {
        // position 26 bytes from the end and try 
    // to read the footer
        bs.seek(bs.size() - footerSize);
        bool isExtended = readFooter(bs, meta.footer);

        //time to read the header
        bs.seek(0);
        bool success = readHeader(bs, meta.header);

        if (!success)
            return false;
    }

    // read a targa image from a stream
    static bool readFromStream(BinStream& bs, BLImage &img)
    {
        TargaMeta meta{};
        bool success = readMetaInformation(bs, meta);

        if (!success) {
            //printf("targa::readFromStream(), failed to read header\n");
            return false;
        }

        // We have the header, so we should be able
        // to read the body
        success = readBody(bs, meta, img);

        return success;
    }

    static bool readFromFile(const char* filename, BLImage& img)
    {
        auto bs = FileStream(filename);

        if (!bs.isValid()) {
            printf("Could not map file: %s\n", filename);
            return false;
        }

        bool success = readFromStream(bs, img);

        return success;
    }
}   // end of targa namespace
