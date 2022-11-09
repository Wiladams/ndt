#pragma once

#include "blend2d.h"
#include "functor.hpp"

//local errors = require("blerror")


//local fsys = require("filesystem")
//local FileSystem = fsys.FileSystem;
//local FileSystemItem = fsys.FileSystemItem;

// Some fontface enums 
enum fontStyle {
    "NORMAL" = 0,
    "OBLIQUE",
    "ITALIC",
};

enum fontStretch {
    "ULTRA_CONDENSED"=1;
    "EXTRA_CONDENSED";
    "CONDENSED";
    "SEMI_CONDENSED";
    "NORMAL";
    "SEMI_EXPANDED";
    "EXPANDED";
    "EXTRA_EXPANDED";
    "ULTRA_EXPANDED";
};

enum fontWeight = {
    [100] = "THIN" = 100,
    [200] = "EXTRA_LIGHT",
    [300] = "LIGHT",
    [350] = "SEMI_LIGHT",
    [400] = "NORMAL",
    [500] = "MEDIUM",
    [600] = "SEMI_BOLD",
    [700] = "BOLD",
    [800] = "EXTRA_BOLD",
    [900] = "BLACK",
    [950] = "EXTRA_BLACK"
};

local unitFactors = {
    ["in"] = 1;
    ["mm"] = 25.4;
    ["px"] = 96;
    ["pt"] = 72;
}

// simple predicate to determine whether a file is
// truetype or not
string function passTTF(item)
{
    local name = item.Name:lower()
    
	return name:find(".ttf", 1, true);
}

local function getFaceInfo(face, fileName)
    if not face then return nil end

    local dmetrics = face:designMetrics()

    local info = {
        fileName = fileName;
        family = face:familyName();
        subFamilyName = face:subfamilyName();
        fullName = face:fullName();
        postscriptName = face:postScriptName();
        style = fontStyle[face:style()];
        stretch =  fontStretch[face:stretch()];
        weight = fontWeight[face:weight()];
    }

    return info
end

// create a database of font face information
local function loadFontFaces(dir, predicate)
    local facedb = {}
    
    local startat = FileSystemItem({Name=dir})


	for item in startat:itemsRecursive() do
		if predicate then
            if predicate(item) then
                -- load the fontface
                local fullPath = item:getFullPath()
                local face, err = BLFontFace:createFromFile(fullPath)
                local familyname = face:familyName():lower()

                local familyslot = facedb[familyname]
                if not familyslot then
                    familyslot = {}
                    facedb[familyname] = familyslot
                end

                local faceinfo = getFaceInfo(face, item.Name)
                familyslot[face:subfamilyName():lower()] = {face = face, sizes = {}, info = faceinfo};
 			end
		else
			print(item.Name);
    end
        end

        return facedb
        end


        class FontMonger
    {
        const std::string systemFontDirectory("c:\\windows\\fonts\\");

        FontMonger(obj)
        {
            obj = obj or {
                typefacedb = loadFontFaces(FontMonger.systemFontDirectory, passTTF)
            }

            obj.typefacedb = obj.typefacedb or loadFontFaces(FontMonger.systemFontDirectory, passTTF)
                obj.dpi = obj.dpi or 96    --start with dpi == px measurement

                //local stime = runningTime()
                //local duration = runningTime() - stime
                //print("FontMonger, loadFontFaces: ", duration)

                setUnits("px")
        }

        // traverse all the fonts that are in the system
        // removing the ones that were created a long time ago
        void cleansweep()
        {
            printf("cleansweep\n");
        }

        void setDpi(self, dpi)
        {
            self.dpi = dpi;
        }



        /* [[
            setUnits, specifies the natural units that will be
                used when specifying a font size.It can be one of
                the unitFactors.
        ]]*/
            void setUnits(char *units)
                local factor = unitFactors[units]
                if not factor then
                    return false, 'unknown units specified'
                    end

                    self.units = units;
            self.unitsPerInch = factor
                end

                -- Enumerate all the fontfaces that are currently
                -- loaded
                function FontMonger.families(self)
                local function visitor()
                for family, v in pairs(self.typefacedb) do
                    if type(family) == "string" and type(v) == "table" then
                        coroutine.yield(family, v)
                        end
                        end
                        end

                        return coroutine.wrap(visitor)
                        end

                        function FontMonger.faces(self)
                        local function visitor()
                        for family, v in pairs(self.typefacedb) do
                            --print("v:", v)
                            for subfamily, facedata in pairs(v) do
                                coroutine.yield(family, subfamily, facedata)
                                end
                                end
                                end

                                return coroutine.wrap(visitor)
                                end

                                // get a particular font face.If 'nearest' is true, then
                                // it will try to match the criteria, even if an exact match
                                // is not available.At least the family must be the same though
                                function FontMonger.getFace(self, family, subfamily, nearest)
                                subfamily = subfamily or "regular"
                                local famslot = self.typefacedb[family:lower()]
                                if not famslot then
                                    return nil, "unknown family: "..family
                                    end

                                    local subslot = famslot[subfamily:lower()]
                                    if not subslot then
                                        return nil, "unknown subfamily: "..subfamily
                                        end

                                        return subslot
                                        end

                                        -- Get a specific font object at a specific size
                                        function FontMonger.getFont(self, family, subfamily, size)
                                        --print("FontMonger.getFont, size: ", family, subfamily, size)

                                        subfamily = subfamily or "regular"
                                        local fontSize = size / self.unitsPerInch * self.dpi

                                        -- try to find the font face
                                        local famslot = self.typefacedb[family]
                                        if not famslot then
                                            return nil, "font family not found: "..family
                                            end

                                            local subslot = famslot[subfamily]
                                            if not subslot then
                                                return nil, "subfamily not found: "..subfamily
                                                end

                                                -- if we have subslot, it's a fontdata entry
                                                --so look in the sizes to see if the font size
                                                -- has already been created.
                                                local sizeKey = string.format("%3.2f", fontSize)
                                                local fontEntry = subslot.sizes[sizeKey]
                                                if not fontEntry then
                                                    -- create the font of the size
                                                    font = subslot.face:createFont(fontSize)
                                                    --add an entry to the sizes table
                                                    fontEntry = { font = font; }
                                                    subslot.sizes[sizeKey] = fontEntry
                                                    end

                                                    // mark the last time this was accessed for later
                                                    // garbage collection
                                                    //fontEntry.accessed = runningTime();

                                                    // Finally, return the actual font object
                                                    return fontEntry.font;
    }

