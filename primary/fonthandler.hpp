#ifndef fonthandler_h
#define fonthandler_h

#include "definitions.h"
#include "blend2d/fontmanager.h"
#include <vector>
#include <memory>

class FontHandler
{
public:
    // Typography
    BLFontManager fFontManager;

    FontHandler()
    {
        fFontManager.create();
        loadDefaultFonts();
    }

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
        BLResult err = ff.createFromFile(filename, BL_FILE_READ_MMAP_AVOID_SMALL);
        
        if (!err)
        {
            //printf("loadFont() adding: %s\n", ff.familyName().data());
            fFontManager.addFace(ff);
            
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
            "c:\\Windows\\Fonts\\times.ttf",
            "c:\\Windows\\Fonts\\verdana.ttf",
            "c:\\Windows\\Fonts\\wingding.ttf"
        };
        loadFonts(fontNames);

        // try to retrieve one again
        //BLFontFace errFace = queryFont("Tahoma");
        //printf("loadDefaultFonts(), query Tahoma: %d\n", errFace.isValid());
    }


};

APP_EXPORT extern std::shared_ptr<FontHandler> gFontHandler;

#endif // fonthandler_h
