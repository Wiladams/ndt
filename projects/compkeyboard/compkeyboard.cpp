#include "p5.hpp"

using namespace p5;

typedef struct keyStruct_t 
{
    int vkey;
    bool isToggle;

    BLRect frame;
    const char *caption;
} keyStruct;


keyStruct keyValues[] = {
{0x1b, false, {4, 6, 40, 40}, "Esc"},
{0x70, false, {84, 6, 40, 40}, "F1"},
{0x71, false, {124, 6, 40, 40}, "F2"},
{0x72, false, {164, 6, 40, 40}, "F3"},
{0x73, false, {204, 6, 40, 40}, "F4"},
{0x74, false, {264, 6, 40, 40}, "F5"},
{0x75, false, {304, 6, 40, 40}, "F6"},
{0x76, false, {344, 6, 40, 40}, "F7"},

{0x77, false, {384, 6, 40, 40}, "F8"},
{0x78, false, {444, 6, 40, 40}, "F9"},
{0x79, false, {484, 6, 40, 40}, "F10"},
{0x7a, false, {524, 6, 40, 40}, "F11"},
{0x7b, false, {564, 6, 40, 40}, "F12"},
{0xc0, false, {4, 82, 40, 40}, "`"},
{0x31, false, {44, 82, 40, 40}, "1"},
{0x32, false, {84, 82, 40, 40}, "2"},

{0x33, false, {124, 82, 40, 40}, "3"},
{0x34, false, {164, 82, 40, 40}, "4"},
{0x35, false, {204, 82, 40, 40}, "5"},
{0x36, false, {244, 82, 40, 40}, "6"},
{0x37, false, {284, 82, 40, 40}, "7"},
{0x38, false, {324, 82, 40, 40}, "8"},
{0x39, false, {364, 82, 40, 40}, "9"},
{0x30, false, {404, 82, 40, 40}, "0"},

{0xbd, false, {444, 82, 40, 40}, "-"},
{0xbb, false, {484, 82, 40, 40}, "="},
{0x08, false, {524, 82, 80, 40}, "Backspace"},
{0x09, false, {4, 122, 60, 40}, "Tab"},
{0x51, false, {64, 122, 40, 40}, "Q"},
{0x57, false, {104, 122, 40, 40}, "W"},
{0x45, false, {144, 122, 40, 40}, "E"},
{0x52, false, {184, 122, 40, 40}, "R"},

{0x54, false, {224, 122, 40, 40}, "T"},
{0x59, false, {264, 122, 40, 40}, "Y"},
{0x55, false, {304, 122, 40, 40}, "U"},
{0x49, false, {344, 122, 40, 40}, "I"},
{0x4f, false, {384, 122, 40, 40}, "O"},
{0x50, false, {424, 122, 40, 40}, "P"},
{0xdb, false, {464, 122, 40, 40}, "["},
{0xdd, false, {504, 122, 40, 40}, "]"},

{0xdc, false, {544, 122, 60, 40}, "\\"},
{0x14, false, {4, 162, 70, 40}, "Caps"},
{0x41, false, {74, 162, 40, 40}, "A"},
{0x53, false, {114, 162, 40, 40}, "S"},
{0x44, false, {154, 162, 40, 40}, "D"},
{0x46, false, {194, 162, 40, 40}, "F"},
{0x47, false, {234, 162, 40, 40}, "G"},
{0x48, false, {274, 162, 40, 40}, "H"},

{0x4a, false, {314, 162, 40, 40}, "J"},
{0x4b, false, {354, 162, 40, 40}, "K"},
{0x4c, false, {394, 162, 40, 40}, "L"},
{0xba, false, {434, 162, 40, 40}, ","},
{0xde, false, {474, 162, 40, 40}, "'"},
{0x0d, false, {514, 162, 90, 40}, "Enter"},
{0xa0, false, {4, 202, 90, 40}, "L-Shift"},
{0x5a, false, {94, 202, 40, 40}, "Z"},

{0x58, false, {134, 202, 40, 40}, "X"},
{0x43, false, {174, 202, 40, 40}, "C"},
{0x56, false, {214, 202, 40, 40}, "V"},
{0x42, false, {254, 202, 40, 40}, "B"},
{0x4e, false, {294, 202, 40, 40}, "N"},
{0x4d, false, {334, 202, 40, 40}, "M"},
{0xbc, false, {374, 202, 40, 40}, ","},
{0xbe, false, {414, 202, 40, 40}, "."},

{0xbf, false, {454, 202, 40, 40}, "/"},
{0xa1, false, {494, 202, 110, 40}, "R-Shift"},
{0xa2, false, {4, 242, 50, 40}, "L-Ctrl"},
{0x5b, false, {54, 242, 50, 40}, "Win"},
{VK_LMENU, false, {104, 242, 50, 40}, "Alt"},
{0x20, false, {154, 242, 250, 40}, "Space"},
{VK_RMENU, false, {404, 242, 50, 40}, "Alt"},
{0x5c, false, {454, 242, 50, 40}, "Win"},

{0x5d, false, {504, 242, 50, 40}, "Apps"},
{0xa3, false, {554, 242, 50, 40}, "R-Ctrl"},
};
// There are 74 keys here

static constexpr int nKeys = sizeof(keyValues) / sizeof(keyStruct);
static constexpr int unit = 40;

BLGradient gradient(BLLinearGradientValues(unit/2, unit/2, unit/2, unit/2));
BLVar gradientVar;

struct VisualKeyboard : public GraphicElement
{
    static BLRoundRect insetRoundRect(const BLRoundRect& rrect, float cx, float cy)
    {
        double dx = cx / 2;
        double dy = cy / 2;

        return BLRoundRect(rrect.x + dx, rrect.y + dy, rrect.w - cx, rrect.h - cy, rrect.rx, rrect.ry);
    }

    VisualKeyboard()
        : GraphicElement(0,0,610,290)
    {}

    void drawKeyStates(IGraphics& ctx)
    {
        for (int i = 0; i < nKeys; i++)
        {
            keyStruct & key = keyValues[i];
            //int state = (::GetAsyncKeyState(key.vkey) & 0xffff);
            
            //if (state) 
            if ((keyStates[key.vkey]&0x80))
            {
                BLRoundRect rrect(key.frame.x, key.frame.y, key.frame.w, key.frame.h, 3, 3);
                ctx.fill(0x30, 0x6f);
                ctx.rect(rrect.x, rrect.y, rrect.w, rrect.h, rrect.rx, rrect.ry);
            }
        }
    }

    void drawNeutral(IGraphics& ctx)
    {
        ctx.fill(127);
        ctx.stroke(10);
        ctx.strokeWeight(1);

        ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
        ctx.textFont("Consolas");
        ctx.textSize(14);

        for (int i = 0; i < nKeys; i++)
        {
            auto key = keyValues[i];
            BLRoundRect rrect{ key.frame.x, key.frame.y, key.frame.w,key.frame.h,3,3 };
            auto crect = insetRoundRect(rrect, unit * 0.15, unit * 0.30);

            // need to adjust values of linear gradient
            double cx = key.frame.x + key.frame.w / 2;
            double cy = key.frame.y + key.frame.h / 2;
            auto r = key.frame.h;

            auto values = BLLinearGradientValues(cx, key.frame.y + key.frame.h, cx, cy);
            gradient.setValues(values);

            ctx.noStroke();
            ctx.fill(gradientVar);


            ctx.rect(rrect.x, rrect.y, rrect.w, rrect.h, rrect.rx, rrect.ry);

            ctx.noFill();
            ctx.stroke(0);
            ctx.rect(rrect.x, rrect.y, rrect.w, rrect.h, rrect.rx, rrect.ry);

            // do the inset rectangle
            ctx.noStroke();
            ctx.fill(255, 0x6f);
            ctx.rect(crect.x, crect.y, crect.w, crect.h, crect.rx, crect.ry);

            // Now do the text
            ctx.fill(0);
            ctx.text(key.caption, key.frame.x + (key.frame.w / 2), key.frame.y + (key.frame.h / 2));
        }
    }

    void drawSelf(IGraphics& ctx)
    {
        drawNeutral(ctx);
        drawKeyStates(ctx);
    }
};

void setup()
{
    setUnitsPerInch(96);

    createCanvas(800, 600);
    //fullscreen();

    // slightly bluish
    gradientVar = gradient;
    gradient.addStop(0.0, BLRgba32(0xFF4f4f4f));
    gradient.addStop(1.0, BLRgba32(0xFF9f9fff));

    auto page = std::make_shared<VisualKeyboard>();
    auto win = window(0, 0, 640, 480);
    win->setBackgroundColor(Pixel(0, 0, 0,10));

    win->addGraphic(page);
}

void draw()
{
    if (!isLayered()) {
        background(0xc0);
    }
    else {
        clear();
    }
}

void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE) {
        halt();
    }
}
