#include "p5.hpp"

using namespace p5;

typedef struct keyStruct_t {
    int vkey;
    BLRect frame;
    const char *caption;
} keyStruct;


keyStruct keyValues[] = {
{0x1b, {4, 6, 40, 40}, "Esc"},
{0x70, {84, 6, 40, 40}, "F1"},
{0x71, {124, 6, 40, 40}, "F2"},
{0x72, {164, 6, 40, 40}, "F3"},
{0x73, {204, 6, 40, 40}, "F4"},
{0x74, {264, 6, 40, 40}, "F5"},
{0x75, {304, 6, 40, 40}, "F6"},
{0x76, {344, 6, 40, 40}, "F7"},
{0x77, {384, 6, 40, 40}, "F8"},
{0x78, {444, 6, 40, 40}, "F9"},
{0x79, {484, 6, 40, 40}, "F10"},
{0x7a, {524, 6, 40, 40}, "F11"},
{0x7b, {564, 6, 40, 40}, "F12"},
{0xc0, {4, 82, 40, 40}, "`"},
{0x31, {44, 82, 40, 40}, "1"},
{0x32, {84, 82, 40, 40}, "2"},
{0x33, {124, 82, 40, 40}, "3"},
{0x34, {164, 82, 40, 40}, "4"},
{0x35, {204, 82, 40, 40}, "5"},
{0x36, {244, 82, 40, 40}, "6"},
{0x37, {284, 82, 40, 40}, "7"},
{0x38, {324, 82, 40, 40}, "8"},
{0x39, {364, 82, 40, 40}, "9"},
{0x30, {404, 82, 40, 40}, "0"},
{0xbd, {444, 82, 40, 40}, "-"},
{0xbb, {484, 82, 40, 40}, "="},
{0x08, {524, 82, 80, 40}, "Backspace"},
{0x09, {4, 122, 60, 40}, "Tab"},
{0x51, {64, 122, 40, 40}, "Q"},
{0x57, {104, 122, 40, 40}, "W"},
{0x45, {144, 122, 40, 40}, "E"},
{0x52, {184, 122, 40, 40}, "R"},
{0x54, {224, 122, 40, 40}, "T"},
{0x59, {264, 122, 40, 40}, "Y"},
{0x55, {304, 122, 40, 40}, "U"},
{0x49, {344, 122, 40, 40}, "I"},
{0x4f, {384, 122, 40, 40}, "O"},
{0x50, {424, 122, 40, 40}, "P"},
{0xdb, {464, 122, 40, 40}, "["},
{0xdd, {504, 122, 40, 40}, "]"},
{0xdc, {544, 122, 60, 40}, "\\"},
{0x14, {4, 162, 70, 40}, "Caps"},
{0x41, {74, 162, 40, 40}, "A"},
{0x53, {114, 162, 40, 40}, "S"},
{0x44, {154, 162, 40, 40}, "D"},
{0x46, {194, 162, 40, 40}, "F"},
{0x47, {234, 162, 40, 40}, "G"},
{0x48, {274, 162, 40, 40}, "H"},
{0x4a, {314, 162, 40, 40}, "J"},
{0x4b, {354, 162, 40, 40}, "K"},
{0x4c, {394, 162, 40, 40}, "L"},
{0xba, {434, 162, 40, 40}, ","},
{0xde, {474, 162, 40, 40}, "'"},
{0x0d, {514, 162, 90, 40}, "Enter"},
{0xa0, {4, 202, 90, 40}, "Shift"},
{0x5a, {94, 202, 40, 40}, "Z"},
{0x58, {134, 202, 40, 40}, "X"},
{0x43, {174, 202, 40, 40}, "C"},
{0x56, {214, 202, 40, 40}, "V"},
{0x42, {254, 202, 40, 40}, "B"},
{0x4e, {294, 202, 40, 40}, "N"},
{0x4d, {334, 202, 40, 40}, "M"},
{0xbc, {374, 202, 40, 40}, ","},
{0xbe, {414, 202, 40, 40}, "."},
{0xbf, {454, 202, 40, 40}, "/"},
{0xa1, {494, 202, 110, 40}, "Shift"},
{0xa2, {4, 242, 50, 40}, "Ctrl"},
{0x5b, {54, 242, 50, 40}, "Win"},
{0x12, {104, 242, 50, 40}, "Alt"},
{0x20, {154, 242, 250, 40}, "Space"},
{0x12, {404, 242, 50, 40}, "Alt"},
{0x5c, {454, 242, 50, 40}, "Win"},
{0x5d, {504, 242, 50, 40}, "Apps"},
{0xa3, {554, 242, 50, 40}, "Ctrl"},
};

static const int nKeys = sizeof(keyValues) / sizeof(keyStruct);
static const int unit = 40;

BLGradient gradient(BLLinearGradientValues(unit/2, unit/2, unit/2, unit/2));

BLRoundRect insetRoundRect(const BLRoundRect& rrect, double cx, double cy)
{
    double dx = cx / 2;
    double dy = cy / 2;

    return BLRoundRect(rrect.x + dx, rrect.y + dy, rrect.w - cx, rrect.h - cy, rrect.rx, rrect.ry);
}


void drawKeyStates(std::shared_ptr<IGraphics> ctx)
{   
    for (int i = 0; i < nKeys; i++)
    {
        auto key = keyValues[i];
        int state = (::GetAsyncKeyState(key.vkey) & 0xffff);

        if (state) {
            BLRoundRect rrect(key.frame.x, key.frame.y, key.frame.w, key.frame.h, 3, 3);
            ctx->fill(0x30, 0x6f);
            ctx->rect(rrect.x, rrect.y, rrect.w, rrect.h, rrect.rx, rrect.ry);
        }
    }
}

void drawNeutral(std::shared_ptr<IGraphics> ctx)
{
    ctx->fill(127);
    ctx->stroke(10);
    ctx->strokeWeight(1);

    ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
    //ctx->textFont("c:\\windows\\fonts\\segoeui.ttf");
    ctx->textSize(8);

    for (int i=0; i<nKeys; i++)
    {
        auto key = keyValues[i];
        BLRoundRect rrect{ key.frame.x, key.frame.y, key.frame.w,key.frame.h,3,3 };
        auto crect = insetRoundRect(rrect, unit*0.15 , unit*0.30);

        // need to adjust values of linear gradient
        double cx = key.frame.x + key.frame.w / 2;
        double cy = key.frame.y + key.frame.h / 2;
        auto r = key.frame.h;

        auto values = BLLinearGradientValues(cx, key.frame.y + key.frame.h, cx, cy);
        gradient.setValues(values);

        ctx->noStroke();
        ctx->fill(gradient);


        ctx->rect(rrect.x, rrect.y, rrect.w, rrect.h, rrect.rx, rrect.ry);

        ctx->noFill();
        ctx->stroke(0);
        ctx->rect(rrect.x, rrect.y, rrect.w, rrect.h, rrect.rx, rrect.ry);

        // do the inset rectangle
        ctx->noStroke();
        ctx->fill(255, 0x6f);
        ctx->rect(crect.x, crect.y, crect.w, crect.h, crect.rx, crect.ry);

        // Now do the text
        ctx->fill(0);
        ctx->text(key.caption, key.frame.x + (key.frame.w / 2), key.frame.y + (key.frame.h / 2));
    }
}





void setup()
{
    gAppSurface->setPpiUnits(systemDpi, 96);

    createCanvas(800, 600);
    layered();

    // slightly bluish
    gradient.addStop(0.0, BLRgba32(0xFF4f4f4f));
    gradient.addStop(1.0, BLRgba32(0xFF9f9fff));
}

void draw()
{
    if (!isLayered()) {
        background(0xc0);
    }
    else {
        clear();
    }

    drawNeutral(gAppSurface);
    drawKeyStates(gAppSurface);
}

void keyReleased(const KeyboardEvent& e)
{
    // toggle layered
    printf("keyReleased: e.keyCode: 0x%x  scanCode: 0x%x\n", e.keyCode, e.scanCode);
    if (e.keyCode == VK_ESCAPE) {
        halt();
    }
}
