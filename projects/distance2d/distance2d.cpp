#include "p5.hpp"
#include "Recorder.hpp"


std::shared_ptr<Recorder> recorder = nullptr;

using namespace p5;

int max_distance;

void setup() {
    //createCanvas(1920, 1080);
    //setCanvasPosition(0, 0);
    fullscreen();

    noStroke();
    fill(255);

    max_distance = dist(canvasWidth/2, 0, canvasWidth, canvasHeight);

    layered();

    recorder = std::make_shared<Recorder>(gAppSurface, "dist2d-");

}

void draw() {
    if (isLayered())
    {
        clear();
        background(10, 127);
    }
    else
        background(10,127);

    fill(250,220);
    for (int i = 0; i <= canvasWidth; i += 20) {
        for (int j = 0; j <= canvasHeight; j += 20) {
            float size = (dist(mouseX, mouseY, i, j))/2;
            size = (size / max_distance) * 66;
            ellipse(i, j, size, size);
        }
    }
}

void onComposed()
{
    if (recorder != nullptr)
        recorder->saveFrame();
}

void keyReleased(const KeyboardEvent& e)
{
    switch (e.keyCode) {
    case 'R':	// toggle recording
        recorder->toggleRecording();
        break;

    case VK_ESCAPE:
        halt();
        break;
    }
}
