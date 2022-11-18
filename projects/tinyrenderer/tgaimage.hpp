#pragma once
#include <cstdint>
#include <fstream>
#include <vector>
#include "pixelaccessor.h"
#include "maths.hpp"

using FBPixel = maths::vec4b;

#pragma pack(push,1)
struct TGAHeader {
    std::uint8_t  idlength{};
    std::uint8_t  colormaptype{};
    std::uint8_t  datatypecode{};
    std::uint16_t colormaporigin{};
    std::uint16_t colormaplength{};
    std::uint8_t  colormapdepth{};
    std::uint16_t x_origin{};
    std::uint16_t y_origin{};
    std::uint16_t width{};
    std::uint16_t height{};
    std::uint8_t  bitsperpixel{};
    std::uint8_t  imagedescriptor{};
};
#pragma pack(pop)



struct TGAColor {
    FBPixel bgra = {0,0,0,0};
    std::uint8_t bytespp = {0};

    TGAColor() = default;
    TGAColor(const std::uint8_t R, const std::uint8_t G, const std::uint8_t B, const std::uint8_t A=255) : bgra{B,G,R,A}, bytespp(4) { }
    TGAColor(const std::uint8_t *p, const std::uint8_t bpp) : bytespp(bpp) {
        for (int i=bpp; i--; bgra[i] = p[i]);
    }
    std::uint8_t& operator[](const int i) { return bgra[i]; }
};

struct TGAImage : public PixelAccessor< maths::vec4b>
{
    enum Format { GRAYSCALE=1, RGB=3, RGBA=4 };
    
    int w = 0;
    int h = 0;
    int bpp = 0;
    std::vector<std::uint8_t> data = {};

public:
    TGAImage() = default;
    TGAImage(const int w, const int h, const int bpp) 
        : w(w)
        , h(h)
        , bpp(bpp)
        , data(w* h* bpp, 0) 
    {
        reset((uint8_t *)&data, w, h, w * bpp, PixelOrientation::BottomToTop);
    }

    bool  read_tga_file(const std::string filename);
    bool write_tga_file(const std::string filename, const bool vflip=true, const bool rle=true) const;
    void flip_horizontally();
    void flip_vertically();

    // Get a pixel's value with some boundary checking
    maths::vec4b getPixel(const int x, const int y) const override
    {
        if (!data.size() || x < 0 || y < 0 || x >= w || y >= h)
            return {};
        TGAColor c(data.data() + (x + y * w) * bpp, bpp);

        return c.bgra;
    }

    // Set a pixel's value with some boundary checking
    void setPixel(const int x, const int y, const maths::vec4b & c) override
    {
        if (!data.size() || x < 0 || y < 0 || x >= w || y >= h) return;
        memcpy(data.data() + (x + y * w) * bpp, maths::data(c), bpp);
    }

    int width() const {return w;}
    int height() const {return h;}

private:
    bool   load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out) const;
};

