#pragma once

/*
    This file is a general memory stream interface.
    The primary objective is to satisfy the needs of the 
    truetype parser, but it can be used in general cases.

    It differs from the MemoryStream object in that it can't
    write, and it has more specific data type reading 
    convenience calls.

    More specifically, all of the numeric reading assumes the
    data in the stream is formatted in 'big-endian' order.  That
    is, the high order bytes come first.
*/

#include <stdint.h>
#include <string.h>

// Define this here instead of using MIN to
// reduce dependencies
#define BINMIN(a,b) (((a)<(b))?(a):(b))

typedef union {
    uint32_t u32;
    float f;
} U32float;

/*
    binstream(data, size, position)
*/
class BinStream
{
    bool fbigend;
    uint8_t *fdata;
    size_t fcursor;
    size_t fsize;

    BinStream() : fdata(nullptr){}

public:

    BinStream(void *data, const size_t size, size_t position=0, bool littleendian = true)
        : fbigend(!littleendian),
        fcursor(position),
        fdata((uint8_t *)data),
        fsize(size)
    {}
 
    bool isValid() {return fdata != nullptr;}

    // report whether we've reached the end of the stream yet
    bool isEOF() {return (remaining() < 1);}

    uint8_t * data() {return fdata;}
    size_t size() {return fsize;}
    
    // report how many bytes remain to be read
    // from stream
    int64_t remaining() {return fsize - fcursor;}

    // Report the current cursor position.
    int64_t tell() {return fcursor;}



    // get a subrange of the memory stream
    // returning a new memory stream
    BinStream range(int64_t pos, int64_t size)
    {
        if (pos < 0 || (fsize < 0)) {
            // BUGBUG - throw exception
            return BinStream();
        }

        if (pos > fsize) {
            // BUGBUG - throw exception
            return BinStream();
        }

        if ((size > (fsize - pos))) { 
            // BUGBUG - throw exception
            return BinStream();
        }

        return BinStream(fdata+pos, size, 0 , !fbigend);
    }

    BinStream range(size_t size) {return range(fcursor, size); }


    // move to an absolute position
    bool seek(const int64_t pos)
    {
        // if position specified outside of range
        // just set it past end of stream
        if ((pos > fsize)  || (pos < 0)) {
            fcursor = fsize;
            return false;   // , self.cursor;
        } else {
            fcursor = pos;
        }
 
        return true;
    }



    /*
        Moving the cursor around
    */
    // move the cursor ahead by the amount
    // specified in the offset
    // seek, relative to current position
    bool skip(int64_t offset) {return seek(fcursor + offset);}

    // Seek forwad to a boundary of the specified
    // number of bytes.
    bool alignTo(size_t num) {return skip(fcursor % num);}
    
    bool skipToEven() {return alignTo(2);}

    void * getPositionPointer() {return fdata + fcursor;}


    // get 8 bits, and don't advance the cursor
    int peekOctet(int offset=0)
    {
        if (fcursor+offset >= fsize) {
            return -1;
        }

        return fdata[fcursor+offset];
    }


    // get 8 bits, and advance the cursor
    uint8_t readOctet()
    {
        //print("self.cursor: ", self.cursor, self.size)
        if (fcursor >= fsize) {
            // BUGBUG - throw exception
            return 0;
        }

        fcursor = fcursor + 1;
    
        return fdata[fcursor-1];
    }

    size_t readBytes(uint8_t * buff, const size_t n)
    {
        if (n < 1) { 
            // throw exception
            return 0;
        }

        // see how many bytes are remaining to be read
        size_t nActual = BINMIN(n, remaining());
    
        // read the minimum between remaining and 'n'
        uint8_t * ptr = fdata+fcursor;
        memcpy_s(buff, n, ptr,nActual);

        skip(nActual);

        // return the number of bytes actually read
        return nActual;
    }

    // read in a null terminated string
    // n - tells us the size of the buffer
    size_t readStringZ(const size_t n, char *buff)
    {
        // determine maximum number of bytes we can read
        // we leave one space for the null terminator
        size_t nActual = BINMIN(n-1, remaining());
        size_t idx = 0;
        while (idx < nActual) {
            buff[idx] = readOctet();
            if (buff[idx] == 0) {
                break;
            }

            idx = idx + 1;
        }

        // ensure null termination
        buff[idx] = 0;

        return idx;
    }

static const int CR = '\r';
static const int LF = '\n';

// Read a single line up to a maximum specified buffer size
// A line is designated with one of the following sequences
// CR-LF
// LF
//
// In either case, the line ending will not be included in the buffer
//
// The buffer is null terminated
size_t readLine(char* buff, const size_t bufflen)
{
    if (isEOF()) {
        return 0;
    }

    int64_t starting = tell();
    int64_t ending = starting;
    void *startPtr = getPositionPointer();

    while (!isEOF()) {
        uint8_t c = peekOctet();
        if (c < 0) {
            break;
        }

        if (c == CR) {
            c = peekOctet(1);
            if (c == LF) {
                ending = tell();
                skip(2);
                break;
            }
        } else if (c == LF) {
            src:skip(1);
            break;
        }

        skip(1);
        ending = tell();
    }

    int64_t len = ending - starting;
    len = BINMIN(len, bufflen-1);

    memcpy(buff, startPtr, len);
    buff[len] = 0;

    return len;
}

    // Read an integer value
    // The parameter 'n' determines how many bytes to read.
    // 'n' can be up to 8 
    // The routine will deal with big or little endian
    uint64_t readInt(int n)
    {
        uint64_t v = 0;
        int i = 0;

        if (remaining() < n) {
            // BUGBUG - throw exception
            return 0;
        }

        if (fbigend) {
            while  (i < n) {
                v = ((v<< 8) | readOctet());
                i = i + 1;
            }
        } else {
            while  (i < n) {
                v = (v | (readOctet() << 8*i));
                i = i + 1;
            }
        }

        return v;
    }

    // Read 8-bit signed integer
    int8_t readInt8()
    {
        return (int8_t)readInt(1);
    }

    // Read 8-bit unsigned integer
    uint8_t readUInt8()
    {
        return (uint8_t)readInt(1);
    }

    // Read 16-bit signed integer
    int16_t readInt16()
    {
        return (int16_t)readInt(2);
    }

    // Read 16-bit unsigned integer
    uint16_t readUInt16()
    {
        return (uint16_t)readInt(2);
    }

    // Read Signed 32-bit integer
    int32_t readInt32()
    {
        return (int32_t)readInt(4);
    }

    // Read unsigned 32-bit integer
    uint32_t readUInt32()
    {
        return (uint32_t)readInt(4);
    }

    // Read signed 64-bit integer
    int64_t readInt64()
    {
        return (int64_t)readInt(8);
    }

    uint64_t readUInt64()
    {
        return (uint64_t)readInt(8);
    }

    float readFloat()
    {
        U32float f1;
        f1.u32 = readUInt32();
        return f1.f;
    }



    /*
        Writing to a binary stream
    */
    bool writeOctet(const uint8_t octet)
    {
        if (fcursor >= fsize) {
            return false;
        }

        fdata[fcursor] = octet;
        fcursor = fcursor + 1;

        return true;
    }

    bool writeBytes(const uint8_t *bytes, const size_t n)
    {
        if (bytes == nullptr) {
            return false;
        }

        if (n > remaining()) {
            // BUGBUG - throw exception
            return false;   //, "Not enough space"
        }

        memcpy_s(fdata+fcursor, n, bytes, n);
        skip(n);

        return true;
    }

    bool writeStringZ(const char * str)
    {
        if (str == nullptr) {
            return 0;
        }

        size_t n = strlen(str);
        bool success = writeBytes((const uint8_t *)str, n);
        
        // write a null terminating byte
        writeOctet(0);

        return success;
    }

    size_t writeInt(const uint64_t value, size_t n)
    {
        if (remaining() < n) {
            // BUGBUG - throw exception
        }

        int i = n-1;
        if (fbigend) {
            while  (i >= 0) {
                writeOctet(((value >> i*8) & 0xff));
                i = i - 1;
            }
        } else {
            size_t i = 0;
            while  (i < n) {
                writeOctet((value >> i*8) & 0xff);
                i = i + 1;
            }
        }

        return i+1;
    }

    size_t writeInt8(const int8_t n)
    {
        return writeInt(n, 1);
    }

    size_t writeUInt8(const uint8_t n)
    {
        return writeInt(n, 1);
    }

    size_t writeInt16(int16_t n)
    {
        return writeInt(n, 2);
    }

    size_t writeUInt16(uint16_t n)
    {
        return writeInt(n, 2);
    }

    size_t writeInt32(int32_t n)
    {
        return writeInt(n, 4);
    }

    size_t writeUInt32(uint32_t n)
    {
        return writeInt(n, 4);
    }

    size_t writeInt64(int64_t n)
    {
        return writeInt(n, 8);
    }

    size_t writeUInt64(uint64_t n)
    {
        return writeInt(n, 8);
    }

    void writeFloat(float value)
    {
        U32float f1;
        f1.f = value;
        writeUInt32(f1.u32);
    }
    
    // various useful fixed formats
    // fixed 2_14 numbers
    // 16-bit
    float readF2Dot14()
    {
        return (float)readUInt16() / 0x4000;
    }

    // fixed 16_16 numbers
    // 32-bit
    float readFixed()
    {
        uint16_t decimal = readInt16();
        uint16_t fraction = readUInt16();

        return decimal + (fraction / 0xffff);
    }

    // fixed 2_30 numbers
    // 32-bit
    float readF2Dot30()
    {
        return (float)readUInt32() / 0x4000000;
    }

    // Convenient names used in various documentation
    uint8_t readBYTE() {return readOctet();}
    uint16_t readWORD() {return readUInt16();}
    uint32_t readDWORD() {return  readUInt32();}
    int32_t readLONG() {return readInt32();}





/*
readFWord = readInt16;
readUFWord = readUInt16;
readOffset16 = readUInt16;
readOffset32 = readUInt32;
*/
};