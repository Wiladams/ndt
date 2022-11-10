#ifndef fonthandler_h
#define fonthandler_h

#include "definitions.h"
#include "blend2d/fontmanager.h"
#include "maths.hpp"

#include <filesystem>
#include <algorithm>
#include <vector>
#include <memory>
#include <map>
#include <string>

//namespace fs = std::filesystem;

class FontHandler
{
public:
    // Typography
    BLFontManager fFontManager{};
    std::vector<std::string> fFamilyNames{};

    // For size helper
    int fDotsPerInch=1;
    float fUnitsPerInch=1;

    FontHandler()
    {
        fFontManager.create();
    }

/*
    setDpiUnits makes it possible to let the FontHandler to 
    understand the DPI we're rendering to, as well as
    the intended units we're using to specify font sizes.
    
    This combination allows you to say; "my display is 192 dpi,
    and I want to use 'inches' to specify my font sizes"

    setDpiUnits(192, 1);

    Or, if you want to work in millimeters

    setDpiUnits(192, 25.4);

    If you don't know your displays dpi, and you just
    want to work in pixels, then you can use the default
    which is;

    setDpiUnits(1,1);

// Some typical units  
    local unitFactors = {
    ["in"] = 1;
    ["mm"] = 25.4;
    ["px"] = 96;
    ["pt"] = 72;
} 
*/
    void setDpiUnits(int dpi, float unitsPerInch)
    {
        fDotsPerInch = dpi;
        fUnitsPerInch = unitsPerInch;
    }

    const std::vector<std::string>& familyNames() const { return fFamilyNames; }

    // query the font manager
    bool queryFontFace(const char* fontname, BLFontFace& face)
    {
        // set the found face as the current face
        auto bResult = fFontManager.queryFace(fontname, face);

        return bResult == BL_SUCCESS;
    }


    // create a single font face by filename
    // Put it into the font manager
    // return it to the user
    BLFontFace loadFontFace(const char* filename)
    {
        BLFontFace ff;
        //BLResult err = ff.createFromFile(filename, BL_FILE_READ_MMAP_AVOID_SMALL);
        BLResult err = ff.createFromFile(filename);

        if (!err)
        {
            //printf("loadFont() adding: %s\n", ff.familyName().data());
            fFontManager.addFace(ff);
            fFamilyNames.push_back(std::string(ff.familyName().data()));
        }
        else {
            ;
            //printf("FontHandler::loadFont Error: %s (0x%x)\n", filename, err);
        }

        return ff;
    }

    // Load the list of font files into 
    // the font manager
    void loadFonts(std::vector<const char*> fontNames)
    {
        for (const auto& filename : fontNames)
        {
            BLFontFace face = loadFontFace(filename);
            //printf("loadFonts: %s, 0x%x\n", face.familyName().data(), face.isValid());
        }
    }

    void loadDirectoryOfFonts(const char *dir)
    {
        const std::filesystem::path fontPath(dir);

        for (const auto& dir_entry : std::filesystem::directory_iterator(fontPath))
        {
            if (dir_entry.is_regular_file())
            {
                if (dir_entry.path().generic_string().ends_with(".ttf"))
                {
                    BLFontFace ff;
                    ff = loadFontFace(dir_entry.path().generic_string().c_str());
                }
            }
        }
    }

    // Do this only once to load in fonts
    void loadDefaultFonts()
    {
        // Load in some fonts to start
        std::vector<const char*> fontNames{
            "c:\\Windows\\Fonts\\arial.ttf",
            "c:\\Windows\\Fonts\\calibri.ttf",
            "c:\\Windows\\Fonts\\cascadiacode.ttf",
            "c:\\Windows\\Fonts\\consola.ttf",
            "c:\\Windows\\Fonts\\cour.ttf",
            "c:\\Windows\\Fonts\\gothic.ttf",
            "c:\\Windows\\Fonts\\segoui.ttf",
            "c:\\Windows\\Fonts\\tahoma.ttf",
            "c:\\Windows\\Fonts\\terminal.ttf",
            "c:\\Windows\\Fonts\\times.ttf",
            "c:\\Windows\\Fonts\\verdana.ttf",
            "c:\\Windows\\Fonts\\wingding.ttf"
        };
        loadFonts(fontNames);
    }

    float getAdjustedFontSize(float sz)
    {
        float fsize = sz * ((float)fDotsPerInch / fUnitsPerInch);
        return fsize;
    }

    void textFont(const char* fontname, float sz, BLFontCore &font)
    {
        blFontReset(&font);

        // query the font manager
        // set the found face as the current face

        BLFontFace face;
        bool success = queryFontFace(fontname, face);

        if (!success)
            return;

        if (face.isValid()) 
        {
            float fsize = getAdjustedFontSize(sz);
            blFontCreateFromFace(&font, &face, fsize);
        }
    }

    maths::vec2f textMeasure(const char* txt, const char *familyname, float size)
    {
        //BLFontMetrics fm = fFont.metrics();
        BLTextMetrics tm;
        BLGlyphBuffer gb;


        // Get the fontface
        // if not found, return zero size
        BLFontFace face;
        queryFontFace(familyname, face);
        if (!face.isValid())
            return { 0,0 };

        // Create the font of the right size
        BLFont font;
        auto fsize = getAdjustedFontSize(size);
        font.createFromFace(face, fsize);

        gb.setUtf8Text(txt);
        font.shape(gb);
        font.getTextMetrics(gb, tm);

        float cx = tm.boundingBox.x1 - tm.boundingBox.x0;
        float cy = font.size();

        return { cx, cy };
    }
};

EXPORT extern std::shared_ptr<FontHandler> gFontHandler;

#endif // fonthandler_h
