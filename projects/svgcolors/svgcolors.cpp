#include "p5.hpp"

#include <memory>

using namespace p5;

int fontSize = 12;
int maxColumns = 8;
int rowSize = 60;
int columnSize = 120;
int scrollSize = fontSize*3;

struct ColorEntry {
    const char* name=nullptr;
    BLRgba32 value{};
};

struct SVGGraphic {
    ColorEntry svgcolor{};
    int column{};
    int row{};


    bool operator==(const SVGGraphic& rhs) { return svgcolor.name == rhs.svgcolor.name; }

};

//const SVGGraphic SVGGraphic::empty;
constexpr auto emptySVGGraphic = SVGGraphic{};

// Database of colors
ColorEntry svgcolors[] = {
{"white", {255, 255, 255}},
{"ivory", {255, 255, 240}},
{"lightyellow", {255, 255, 224}},
{"mintcream", {245, 255, 250}},
{"azure", {240, 255, 255}},
{"snow", {255, 250, 250}},
{"honeydew", {240, 255, 240}},
{"floralwhite", {255, 250, 240}},
{"ghostwhite", {248, 248, 255}},
{"lightcyan", {224, 255, 255}},
{"lemonchiffon", {255, 250, 205}},
{"cornsilk", {255, 248, 220}},
{"lightgoldenrodyellow", {250, 250, 210}},
{"aliceblue", {240, 248, 255}},
{"seashell", {255, 245, 238}},
{"oldlace", {253, 245, 230}},
{"whitesmoke", {245, 245, 245}},
{"lavenderblush", {255, 240, 245}},
{"beige", {245, 245, 220}},
{"linen", {250, 240, 230}},
{"papayawhip", {255, 239, 213}},
{"blanchedalmond", {255, 235, 205}},
{"antiquewhite", {250, 235, 215}},
{"yellow", {255, 255, 0}},
{"mistyrose", {255, 228, 225}},
{"lavender", {230, 230, 250}},
{"bisque", {255, 228, 196}},
{"moccasin", {255, 228, 181}},
{"palegoldenrod", {238, 232, 170}},
{"khaki", {240, 230, 140}},
{"navajowhite", {255, 222, 173}},
{"aquamarine", {127, 255, 212}},
{"paleturquoise", {175, 238, 238}},
{"wheat", {245, 222, 179}},
{"peachpuff", {255, 218, 185}},
{"palegreen", {152, 251, 152}},
{"greenyellow", {173, 255, 47}},
{"gainsboro", {220, 220, 220}},
{"powderblue", {176, 224, 230}},
{"lightgreen", {144, 238, 144}},
{"lightgray", {211, 211, 211}},
{"chartreuse", {127, 255, 0}},
{"gold", {255, 215, 0}},
{"lightblue", {173, 216, 230}},
{"lawngreen", {124, 252, 0}},
{"pink", {255, 192, 203}},
{"aqua", {0, 255, 255}},
{"cyan", {0, 255, 255}},
{"lightpink", {255, 182, 193}},
{"thistle", {216, 191, 216}},
{"lightskyblue", {135, 206, 250}},
{"lightsteelblue", {176, 196, 222}},
{"skyblue", {135, 206, 235}},
{"silver", {192, 192, 192}},
{"springgreen", {0, 255, 127}},
{"mediumspringgreen", {0, 250, 154}},
{"turquoise", {64, 224, 208}},
{"burlywood", {222, 184, 135}},
{"tan", {210, 180, 140}},
{"yellowgreen", {154, 205, 50}},
{"lime", {0, 255, 0}},
{"mediumaquamarine", {102, 205, 170}},
{"mediumturquoise", {72, 209, 204}},
{"darkkhaki", {189, 183, 107}},
{"lightsalmon", {255, 160, 122}},
{"plum", {221, 160, 221}},
{"sandybrown", {244, 164, 96}},
{"darkseagreen", {143, 188, 143}},
{"orange", {255, 165, 0}},
{"darkgray", {169, 169, 169}},
{"goldenrod", {218, 165, 32}},
{"darksalmon", {233, 150, 122}},
{"darkturquoise", {0, 206, 209}},
{"limegreen", {50, 205, 50}},
{"violet", {238, 130, 238}},
{"deepskyblue", {0, 191, 255}},
{"darkorange", {255, 140, 0}},
{"salmon", {250, 128, 114}},
{"rosybrown", {188, 143, 143}},
{"lightcoral", {240, 128, 128}},
{"coral", {255, 127, 80}},
{"mediumseagreen", {60, 179, 113}},
{"lightseagreen", {32, 178, 170}},
{"cornflowerblue", {100, 149, 237}},
{"cadetblue", {95, 158, 160}},
{"peru", {205, 133, 63}},
{"hotpink", {255, 105, 180}},
{"orchid", {218, 112, 214}},
{"palevioletred", {219, 112, 147}},
{"darkgoldenrod", {184, 134, 11}},
{"lightslategray", {119, 136, 153}},
{"tomato", {255, 99, 71}},
{"gray", {128, 128, 128}},
{"dodgerblue", {30, 144, 255}},
{"mediumpurple", {147, 112, 219}},
{"olivedrab", {107, 142, 35}},
{"slategray", {112, 128, 144}},
{"chocolate", {210, 105, 30}},
{"steelblue", {70, 130, 180}},
{"olive", {128, 128, 0}},
{"mediumslateblue", {123, 104, 238}},
{"indianred", {205, 92, 92}},
{"mediumorchid", {186, 85, 211}},
{"seagreen", {46, 139, 87}},
{"darkcyan", {0, 139, 139}},
{"forestgreen", {34, 139, 34}},
{"royalblue", {65, 105, 225}},
{"dimgray", {105, 105, 105}},
{"orangered", {255, 69, 0}},
{"slateblue", {106, 90, 205}},
{"teal", {0, 128, 128}},
{"darkolivegreen", {85, 107, 47}},
{"sienna", {160, 82, 45}},
{"green", {0, 128, 0}},
{"darkorchid", {153, 50, 204}},
{"saddlebrown", {139, 69, 19}},
{"deeppink", {255, 20, 147}},
{"blueviolet", {138, 43, 226}},
{"magenta", {255, 0, 255}},
{"fuchsia", {255, 0, 255}},
{"darkslategray", {47, 79, 79}},
{"darkgreen", {0, 100, 0}},
{"darkslateblue", {72, 61, 139}},
{"brown", {165, 42, 42}},
{"mediumvioletred", {199, 21, 133}},
{"crimson", {220, 20, 60}},
{"firebrick", {178, 34, 34}},
{"red", {255, 0, 0}},
{"darkviolet", {148, 0, 211}},
{"darkmagenta", {139, 0, 139}},
{"purple", {128, 0, 128}},
{"midnightblue", {25, 25, 112}},
{"darkred", {139, 0, 0}},
{"maroon", {128, 0, 0}},
{"indigo", {75, 0, 130}},
{"blue", {0, 0, 255}},
{"mediumblue", {0, 0, 205}},
{"darkblue", {0, 0, 139}},
{"navy", {0, 0, 128}},
{"black", {0, 0, 0}},
};

int nColors = sizeof(svgcolors) / sizeof(ColorEntry);

SVGGraphic hoverGraphic = emptySVGGraphic;

std::shared_ptr<GWindow> contentArea = nullptr;

class SVGPage : public Graphic 
{
    static maths::vec2f getPreferredSize()
    {
        int rows = (nColors / maxColumns) + 1;

        return { float((maxColumns * columnSize)), float((rows * rowSize)) };
    }

public:
    SVGPage()
        :Graphic(0,0,getPreferredSize().x, getPreferredSize().y)
    {}

    // return the coordinate frame for a specified cell
    BLRect frameForCell(int column, int row)
    {
        double x = (column - 1) * columnSize;
        double y = (row - 1) * rowSize;
        double w = columnSize;
        double h = rowSize;

        return BLRect(x, y, w, h);
    }

    SVGGraphic graphicAt(int x, int y)
    {
        int column = (int)floor(x / columnSize);
        int row = (int)floor(y / rowSize);

        int index = (int)((row * maxColumns) + column + 1);


        if (index > nColors) {
            //return SVGGraphic::empty;
            return emptySVGGraphic;
        }
        auto entry = svgcolors[index];

        //print("graphicAt: ", x, y, column, row, index, entry)

        return { entry, column + 1, row + 1 };
    }

    void mouseMoved(const MouseEvent& e) override
    {
        //printf("moved: %d,%d\n", e.x, e.y);
        hoverGraphic = graphicAt(e.x, e.y);
    }



    void drawEntry(IGraphics & ctx, ColorEntry acolor, int column, int row)
    {
        double lum = (0.2125 * acolor.value.r()) + (0.7154 * acolor.value.g()) + (0.0721 * acolor.value.b());

        ctx.noStroke();
        ctx.fill(acolor.value);

        auto frame = frameForCell(column, row);
        ctx.rect(frame.x, frame.y, frame.w, frame.h);

        // draw the text value
        // set the filling based on the luminance
        if (lum > 120) {
            ctx.fill(0);
        }
        else {
            ctx.fill(255);
        }

        double cx = frame.x + (frame.w / 2);
        double cy = frame.y + frame.h - 12;
        ctx.noStroke();
        ctx.text(acolor.name, cx, cy);
    }


    void drawForeground(IGraphics & ctx) override
    {
        if (hoverGraphic == emptySVGGraphic)
            return;

        auto frame = frameForCell(hoverGraphic.column, hoverGraphic.row);
        ctx.strokeWeight(3);
        ctx.stroke(255, 255, 0);
        ctx.noFill();
        ctx.rect(frame.x, frame.y, frame.w, frame.h);

    }

    // Draw everything normal
    void drawBackground(IGraphics & ctx) override
    {
        auto column = 1;
        auto row = 1;


        ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
        ctx.textSize(fontSize);

        for (int i = 0; i < nColors; i++)
        {
            drawEntry(ctx, svgcolors[i], column, row);
            column = column + 1;
            if (column > maxColumns) {
                column = 1;
                row = row + 1;
            }
        }
    }
};

std::shared_ptr<SVGPage> page = nullptr;

void draw()
{
    background(245, 246, 247);
}

// Vertical mouse wheel
void mouseWheel(const MouseEvent& e)
{
    //printf("wheel: %d\n", e.delta);
    page->translateBy(0, (e.delta / 120) * scrollSize);
}

// Horizontal mouse wheel
void mouseHWheel(const MouseEvent& e)
{
    //printf("wheel: %d\n", e.delta);
    page->translateBy((e.delta / 120) * scrollSize, 0);
}

// Handling panning
// do translation
void panMoved(const GestureEvent& e)
{
    page->translateBy(panVecX, panVecY);
}

void setup()
{
    createCanvas(800, 600);

    // Create a window to hold stuff
    contentArea = window(4, 4, canvasWidth-8, canvasHeight-8);
    page = std::make_shared<SVGPage>();
    contentArea->addChild(page);
}
