#ifndef fonthandler_h
#define fonthandler_h

#include "definitions.h"
#include "blend2d/fontmanager.h"

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
    BLFontManager fFontManager;
    std::vector<std::string> fFamilyNames;

    FontHandler()
    {
        fFontManager.create();
    }

    const std::vector<std::string>& familyNames() const { return fFamilyNames; }

    // query the font manager
    BLFontFace queryFontFace(const char* fontname)
    {
        // set the found face as the current face
        BLFontFace face;

        BLResult err = fFontManager.queryFace(fontname, face);

        //printf("FontHandler::queryFont(%s); fontMan.queryFace: 0x%x\n", fontname, err);

        return face;
    }


    // create a single font face by filename
    // Put it into the font manager
    // return it to the user
    BLFontFace loadFont(const char* filename)
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
            printf("FontHandler::loadFont Error: %s (0x%x)\n", filename, err);
        }

        return ff;
    }

    // Load the list of font files into 
    // the font manager
    void loadFonts(std::vector<const char*> fontNames)
    {
        for (const auto& filename : fontNames)
        {
            BLFontFace face = loadFont(filename);
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
                BLFontFace ff;
                ff = loadFont(dir_entry.path().generic_string().c_str());
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

        // try to retrieve one again
        //BLFontFace errFace = queryFont("Tahoma");
        //printf("loadDefaultFonts(), query Tahoma: %d\n", errFace.isValid());
    }

    maths::vec2f textMeasure(const char* txt, const char *familyname, float size)
    {
        //BLFontMetrics fm = fFont.metrics();
        BLTextMetrics tm;
        BLGlyphBuffer gb;


        // Get the fontface
        // if not found, return zero size
        BLFontFace face = queryFontFace(familyname);
        if (!face.isValid())
            return { 0.0 };

        // Create the font of the right size
        BLFont font;
        font.createFromFace(face, size);

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
