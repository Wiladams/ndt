#include "p5.hpp"

typedef struct keyFrame_t {
    int x, y;
    int width, height;
} keyFrame;

typedef struct keyStruct_t {
    int vkey;
    keyFrame frame;
    char *caption;
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
{0x00, {4, 82, 40, 40}, "`"},
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
{0xa5, {504, 242, 50, 40}, "Menu"},
{0xa3, {554, 242, 50, 40}, "Ctrl"},
};

static const int nKeys = sizeof(keyValues)/ sizeof(keyStruct);

keyFrame insetRect(keyFrame rrect, float cx, float cy)
{
    int dx = cx/2;
    int dy = cy/2;

    return {rrect.x+dx, rrect.y+dy, rrect.width-(int)cx, rrect.height-(int)cy};
}

int T_SP = ' ';

void keyTyped(const KeyEvent &event)
{
    // If the user types a '<sp>' reset
    // the chain to 1 node
    if (keyCode == T_SP) {
        if (gIsLayered)
            noLayered();
        else
            layered();

    }
}

static const int unit = 40;

void drawNeutral()
{

    fill(127);
    stroke(10);

    int rowoffset = 4;
    int ygap = 2;

    //ctx:textAlign(MIDDLE, MIDDLE)
    //ctx:textFont("segoe ui")
    //ctx:textSize(8);
    for (int i=0; i<nKeys; i++)
    {
        keyFrame rrect = keyValues[i].frame;
        keyFrame crect = insetRect(rrect, unit*0.30, unit*0.30);

        fill(127);
        stroke(10);
        rect(rrect.x, rrect.y, rrect.width, rrect.height);

        fill(255, 0x6f);
        rect(crect.x, crect.y, crect.width, crect.height);

        // draw the caption
        fill(0);
        //ctx:text(key.caption, key.frame.x+key.frame.width/2, key.frame.y+key.frame.height/2)

    }

}

void draw()
{
    background(0xc0);
    clear();
    drawNeutral();
}

void setup()
{
    createCanvas(800, 600);
}