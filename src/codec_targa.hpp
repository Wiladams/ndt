/*
    References
    
    http://www.fileformat.info/format/tga/egff.htm#TGA-DMYID.2
    http://www.dca.fee.unicamp.br/~martino/disciplinas/ea978/tgaffs.pdf
    
-- library of congress
    https://www.loc.gov/preservation/digital/formats/fdd/fdd000180.shtml

-- other implementations
    https://unix4lyfe.org/targa/
    https://github.com/ftrvxmtrx/tga

    Honors the following properties
    vertical orientation
    horizontal orientation
    truecolor (16, 24, 32)
    indexed (16, 24, 32)
    monochrome (8)
    compressed - RLE, (not RAW)
*/


#include "binstream.hpp"
#include "mmap.hpp"
#include "PixelBufferRGBA32.hpp"
#include "enumerable.hpp"


//    Convenience structures
enum ColorMapType : int {
    None   = 0,
    Palette     = 1
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
const char * targaXFileID = "TRUEVISION-XFILE.";

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
    PixRGBA     *ColorMap;          // allocated
    bool        Compressed;         // calculated

    // Image information
    uint16_t    XOffset;
    uint16_t    YOffset;
    uint16_t    Width;
    uint16_t    Height;
    uint8_t     PixelDepth;         // bits per pixel
    uint8_t     BytesPerPixel;      // calculated
    uint8_t     ImageDescriptor;
    uint8_t     * ImageIdentification;  // allocated

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


bool decodeSinglePixel(PixRGBA &pix, uint8_t *databuff, int pixelDepth, int imtype, PixRGBA *colorMap)
{
    //printf("decodeSinglePixel: type: %d bpp:%d\n", imtype, pixelDepth);

    if ((imtype == TrueColor) || (imtype == TrueColorCompressed)) {
        if (pixelDepth == 24) {
            pix.red = databuff[2];
            pix.green = databuff[1];
            pix.blue = databuff[0];
            pix.alpha = 255;
            return true;
        } else if (pixelDepth == 32) {
            pix.red = databuff[2];
            pix.green = databuff[1];
            pix.blue = databuff[0];
            pix.alpha = databuff[3];
            
            // some images use alpha as transparency
            // some images do not
            // when they don't use it, they might set it to '0'
            // which will make the pixels transparent
            // we need to be able to distinguish between these
            // two states
            //printf("pix.alpha: %d\n", pix.alpha);
            //pix.alpha = databuff[3];   // BUGBUG - We should pre-multiply the alpha?
            return true;
        } else if (pixelDepth == 16) {
            uint16_t src16 = ((databuff[1] << 8) | databuff[0]);
            pix.blue = (BITSVALUE(src16,0,4) << 3);
            pix.green = (BITSVALUE(src16,5,9) << 3);
            pix.red = (BITSVALUE(src16,10,14) << 3);
            pix.alpha = 255;
            if (BITSVALUE(src16,15,15) >= 1) {
                pix.alpha = 0;  // 255
            }
        }

        return true;
    } else if ((imtype == Monochrome) || (imtype == MonochromeCompressed)) {
        pix.red = databuff[0];
        pix.green = databuff[0];
        pix.blue = databuff[0];
        pix.alpha = 255;

        return true;
    } else if ((imtype == ColorMapped) || (imtype == ColorMappedCompressed)) {
        // lookup the color using databuff[0] as index
        pix = colorMap[databuff[0]];

        return true;
    }

    return false;
}

PixRGBA * readColorMap(BinStream &bs, tgaHeader &header)
{
    int bytespe = header.CMapDepth / 8;     // bytes per entry
    uint8_t databuff[16];

    PixRGBA *cMap = {new PixRGBA[header.CMapLength]{}};

    for (int i= header.CMapStart; i<header.CMapLength; i++) {
        int nRead = bs.readBytes(bytespe, databuff);
        
        // BUGBUG - this could use decodeSinglePixel()
        decodeSinglePixel(cMap[i], databuff, header.CMapDepth, TrueColor, nullptr);
    }

    return cMap;
}

bool readHeader(BinStream &bs, tgaHeader &res)
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
        res.ImageIdentification = {new uint8_t[res.IDLength]{}};
        bs.readBytes(res.IDLength, res.ImageIdentification);
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

bool readFooter(BinStream &bs, tgaFooter &rs)
{
    //print("targa.readFooter, BEGIN")

    rs.ExtensionAreaOffset = bs.readUInt32();
    rs.DeveloperDirectoryOffset = bs.readUInt32();
    bs.readBytes(18, (uint8_t *)rs.Signature);

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
-- We want to figure out the mapping between positions as we
-- read them and their locations in our pixel buffer in one place
-- This iterator figures that out, return x,y pairs for the positions
-- according to the horizontal and vertical orientation.  Ideally this
-- would do interleaving as well, but we don't have an image to test
-- with
*/
class LocIterator : IEnumerator<Location>
{
    int xSign, xStart, xEnd, xincr;
    int ySign, yStart, yEnd, yincr;

    tgaHeader fHeader;

public:
    LocIterator(tgaHeader &header)
        :fHeader(header)
    {
        // Start with left to right orientation
        xSign = 1;
        xStart = 0;
        xEnd = header.Width-1;
        xincr = -1;

        // switch to right to left of horizontal orientation indicates
        if (header.HorizontalOrientation == RightToLeft) {
            xSign = -1;
            xStart = header.Width-1;;
            xEnd = 0;
        }

        // start top to bottom vertical orientation
        ySign = 1;
        yStart = 0;
        yEnd = header.Height-1;
        yincr = 0;

        // Switch to bottom to top if necessary
        if (header.VerticalOrientation == BottomToTop) {
            ySign = -1;
            yStart = header.Height-1;
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
        return {xStart + (xSign*xincr), yStart + (ySign*yincr)};
    }

    // getCurrent()
    Location operator *() {return getCurrent();}
};

/*
// An iterator which will return the uncompressed
// pixels in row order
*/
class PixelsUncompressed : IEnumerator<PixRGBA>
{
    TargaMeta &fMeta;
    int bytesPerPixel;
    uint8_t databuff[16];     // Maximum number of bytes per pixel
    int xStart, xEnd, xincr;
    int yStart, yEnd, yincr;
    BinStream &bs;
    PixRGBA pix;

public:
    PixelsUncompressed(BinStream &abs, TargaMeta &meta)
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
        int nRead = bs.readBytes(fMeta.header.BytesPerPixel, databuff);

        decodeSinglePixel(pix, databuff, fMeta.header.PixelDepth, fMeta.header.ImageType, fMeta.header.ColorMap);
        
        return true;
    }

    void reset() { xincr = -1; yincr = 0; }

    PixRGBA getCurrent() const {return pix;}
};

// An enumerator of compressed pixels
class PixelsCompressed : IEnumerator<PixRGBA>
{
    TargaMeta &fMeta;
    int bytesPerPixel;
    uint8_t databuff[16];     // Maximum number of bytes per pixel
    BinStream &bs;
    PixRGBA pix;
    int pixelCount;
    bool isRLE;


public:
    PixelsCompressed(BinStream &abs, TargaMeta &meta)
        : fMeta(meta),
        bs(abs)
    {
        bytesPerPixel = meta.header.BytesPerPixel;
        printf("PixelsCompressed, bpp: %d\n", bytesPerPixel);

        reset();
    }

    bool moveNext()
    {
        if (pixelCount == 0) {
            // read packet type to see if it's RLE or RAW
            int packet = bs.readOctet();
            isRLE = (packet & 0x80) > 0; // isset(repCount, 7);
            pixelCount = BITSVALUE(packet,0,6) + 1;
            
            // Read at least one pixel value
            int nRead = bs.readBytes(bytesPerPixel, databuff);
            decodeSinglePixel(pix, databuff, fMeta.header.PixelDepth, fMeta.header.ImageType, fMeta.header.ColorMap);
        } else {
            // We're still in a run
            if (!isRLE) {
                // read another pixel
                int nRead = bs.readBytes(bytesPerPixel, databuff);
                decodeSinglePixel(pix, databuff, fMeta.header.PixelDepth, fMeta.header.ImageType, fMeta.header.ColorMap);
            }
        }
        pixelCount = pixelCount - 1;
        
        return true;
    }

    PixRGBA getCurrent() const {return pix;}

    void reset() {
        pixelCount = 0;
    }
};

PixelBuffer * readBody(BinStream &bs, TargaMeta &meta)
{
    PixelBuffer * lpb = new PixelBufferRGBA32(meta.header.Width, meta.header.Height);
    
    LocIterator li(meta.header);

    if (!meta.header.Compressed) {
        PixelsUncompressed pi(bs, meta);

        while (li.moveNext()) {
            Location loc = li.getCurrent();
            if (!pi.moveNext()) {
                break;
            }
            
            PixRGBA c = pi.getCurrent();
            lpb->setPixel(loc.x, loc.y, c);
        }
    } else {
        PixelsCompressed pi(bs, meta);

        while (li.moveNext()) {
            Location loc = li.getCurrent();
            //printf("loc: %d X %d\n", loc.x, loc.y);

            if (!pi.moveNext()) {
                break;
            }
            
            PixRGBA c = pi.getCurrent();
            lpb->setPixel(loc.x, loc.y, c);
        }
    }

    return lpb;
}

// read a targa image from a stream
PixelBuffer * readFromStream(BinStream &bs, TargaMeta &res)
{
    // position 26 bytes from the end and try 
    // to read the footer
    bs.seek(bs.length()-footerSize);
    bool success = readFooter(bs, res.footer);

    //time to read the header
    bs.seek(0);
    success = readHeader(bs, res.header);

    if (!success) {
        return nullptr;
    }

    // We have the header, so we should be able
    // to read the body
    PixelBuffer *apb = readBody(bs, res);

    return apb;
}


PixelBuffer * readFromFile(const char *filename, TargaMeta &meta)
{
    mmap fmap = mmap(filename);
    if (!fmap.isValid()) {
        printf("Could not map file: %s\n", filename);
        return nullptr;
    }

    BinStream bs(fmap.getPointer(), fmap.length(), 0, true );

    if (!bs.isValid()) {
        printf("BinaryStream not valid.\n");
        return nullptr;
    }

    PixelBuffer * abuff = readFromStream(bs, meta);

    return abuff;
}

