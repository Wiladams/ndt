#include "p5.hpp"
#include "elements/slider.h"
#include "svgcolorpage.h"

#include <memory>

using namespace p5;


std::shared_ptr<SVGColorPage> page = nullptr;

void draw()
{
    background(245, 246, 247);
}

// Vertical mouse wheel
void mouseWheel(const MouseEvent& e)
{
    page->translateBoundsBy(0, e.delta * SVGColorPage::scrollSize);
}


void setup()
{
    createCanvas(800, 600);

    page = std::make_shared<SVGColorPage>(maths::rectf{0,0,(float)canvasWidth,(float)canvasHeight});
    addGraphic(page);

    auto sldr = Slider::create(
        { float(page->frameWidth() - 20), 10},
        {float(page->frameWidth() - 20), float(page->frameHeight() - 10)}
    );
    addGraphic(sldr);

    sldr->subscribe([](const float pos) {page->scrollTo(pos); });
}
