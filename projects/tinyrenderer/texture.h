#pragma once

#include <vector>
#include <string>

struct Texture
{
    static bool load_texture(const std::string filename, const std::string suffix, TGAImage& img)
    {
        size_t founddot = filename.find_last_of(".");
        if (founddot == std::string::npos)
            return false;

        std::string texfile = filename.substr(0, founddot) + suffix;

        auto res = img.read_tga_file(texfile.c_str());
        if (!res)
            std::cerr << "texture file " << texfile << " loading " << (res ? "ok" : "failed") << std::endl;

        return res;
    }
};
