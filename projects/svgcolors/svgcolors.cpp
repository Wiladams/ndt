#include "p5.hpp"
#include "elements/slider.h"
#include "svgparser.h"

#include <memory>

using namespace p5;

int maxColumns = 8;
float fontSize = 12;
float rowSize = 60;
float columnSize = 120;
float scrollSize = fontSize*3;



struct SVGColorGraphic : public GraphicElement
{
    maths::vec4b fColor{};

    SVGColorGraphic(const maths::rectf &fr, const std::string &name, const maths::vec4b &c )
        : GraphicElement(fr)
    {
        fColor = c;
        fColor.a = 255;
        setName(name);
    }


    bool operator==(const SVGColorGraphic& rhs) { return name() == rhs.name(); }


    void drawSelf(IGraphics& ctx)
    {
        double lum = (0.2125 * fColor.r) + (0.7154 * fColor.g) + (0.0721 * fColor.b);

        ctx.noStroke();
        ctx.fill(fColor.r, fColor.g,fColor.b,fColor.a);
        ctx.rect(boundsX(), boundsY(), boundsWidth(), boundsHeight());

        // draw the text value
        // set the filling based on the luminance
        if (lum > 120) {
            ctx.fill(0);
        }
        else {
            ctx.fill(255);
        }

        // use center alignment
        float cx = boundsX() + (boundsWidth() / 2);
        float cy = boundsY() + boundsHeight() /2;
        ctx.noStroke();
        ctx.textSize(12);
        ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
        ctx.text(name().c_str(), cx, cy);
    }
};


//
// A whole page of color graphic chits
//
class SVGColorPage : public GraphicGroup
{
    static maths::vec2f preferredSize()
    {
        int rows = (svg::colors.size() / maxColumns) + 1;

        return { float((maxColumns * columnSize)), float((rows * rowSize)) };
    }

    std::shared_ptr<GraphicElement> fHoverGraphic = nullptr;

public:
    SVGColorPage()
        :GraphicGroup(0,0,preferredSize().x, preferredSize().y)
    {
        int column = 0;
        int row = 0;

        // Construct the children
        for (auto& entry : svg::colors)
        {
            maths::rectf f{ column * columnSize, row * rowSize, columnSize, rowSize };
            auto g = std::make_shared<SVGColorGraphic>(f, entry.first, entry.second);
            addGraphic(g);

            column = column + 1;
            if (column >= maxColumns) {
                column = 0;
                row = row + 1;
            }
        }
    }


    void mouseEvent(const MouseEvent& e) override
    {
        printf("SVGColorPage::mouseEvent: %3.0f,%3.0f\n", e.x, e.y);

        fHoverGraphic = graphicAt(e.x+fTranslation.x, e.y-fTranslation.y);
    }

    void drawForeground(IGraphics& ctx) override
    {
        if (fHoverGraphic == nullptr)
            return;

        auto fr = fHoverGraphic->frame();

        ctx.strokeWeight(3);
        ctx.stroke(255, 255, 0);
        ctx.noFill();
        ctx.rect(fr.x, fr.y, fr.w, fr.h);
    }
};

std::shared_ptr<GWindow> mainWin = nullptr;
std::shared_ptr<SVGColorPage> page = nullptr;

void draw()
{
    background(245, 246, 247);
}

// Vertical mouse wheel
void mouseWheel(const MouseEvent& e)
{
    //printf("wheel: %d\n", e.delta);
    page->translateBoundsBy(0, e.delta * scrollSize);
}

// Handling panning
// do translation
void panMoved(const GestureEvent& e)
{
    page->translateBoundsBy(panVecX, panVecY);
}

void onSlide(const float pos)
{
    //printf("onSlide: %f, %f\n", pos.x, pos.y);
    float maxY = page->boundsHeight() - mainWin->frameHeight();
    float transY = maths::map(pos, 0, 1, 0, maxY);

    page->translateBoundsTo(0, -transY);
}


void setup()
{
    createCanvas(800, 600);

    // Create a window to hold stuff
    //mainWin = window(0, 0, canvasWidth, canvasHeight);
    //mainWin->setTitle("contentArea");
    
    page = std::make_shared<SVGColorPage>();
    addGraphic(page);

    //mainWin->addGraphic(page);

    auto sldr = Slider::create(
        { float(page->frameWidth() - 20), 40},
        {float(page->frameWidth() - 20), float(page->frameHeight() - 8)});
    page->addGraphic(sldr);

    sldr->subscribe(onSlide);
}
