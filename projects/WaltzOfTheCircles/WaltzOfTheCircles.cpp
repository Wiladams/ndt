#include "p5.hpp"

using namespace p5;

/*
  based on: https://observablehq.com/@rreusser/instanced-webgl-circles
*/
constexpr int MAX_CIRCLE_CNT = 5000;
constexpr int MIN_CIRCLE_CNT = 100;
constexpr int MAX_VERTEX_CNT = 30;
constexpr int MIN_VERTEX_CNT = 3;
constexpr double TAU = maths::Pi2;


int circleCnt, vertexCnt;

void updateCntByMouse() {
    float xoffset = abs(mouseX - canvasWidth / 2.0), yoffset = abs(mouseY - canvasHeight / 2.0);
    circleCnt = int(map(xoffset, 0, canvasWidth / 2.0, MAX_CIRCLE_CNT, MIN_CIRCLE_CNT));
    vertexCnt = int(map(yoffset, 0, canvasHeight / 2.0, MAX_VERTEX_CNT, MIN_VERTEX_CNT));
}

maths::vec2f getCenterByTheta(float theta, float time, float scale) {
    maths::vec2f direction = maths::vec2f{ cos(theta), sin(theta) };
    double distance = 0.6 + 0.2 * cos(theta * 6.0 + cos(theta * 8.0 + time));
    maths::vec2f circleCenter = direction * (distance * scale); // PVector::mult(direction, distance * scale);

    return circleCenter;
}

float getSizeByTheta(float theta, float time, float scale) {
    float offset = 0.2 + 0.12 * cos(theta * 9.0 - time * 2.0);
    float circleSize = scale * offset;
    return circleSize;
}

Pixel getColorByTheta(float theta, float time) {
    float th = 8.0 * theta + time * 2.0;
    float r = 0.6 + 0.4 * cos(th),
        g = 0.6 + 0.4 * cos(th - maths::pi / 3),
        b = 0.6 + 0.4 * cos(th - maths::pi * 2.0 / 3.0),
        alpha = map(circleCnt, MIN_CIRCLE_CNT, MAX_CIRCLE_CNT, 150, 30);
    return Pixel(r * 255, g * 255, b * 255, alpha);
}

void setup()
{
    createCanvas(700, 700, "WaltzOfTheCircles", 8);
    //fullscreen("WaltzOfTheCircles", 8);

    frameRate(30);
}

void keyReleased(const KeyboardEvent& event)
{
    if (keyCode == VK_ESCAPE) {
        halt();
    }
}

void draw() {
    background(0);

    translate((float)canvasWidth / 2, (float)canvasHeight / 2);

    updateCntByMouse();
    noFill();

    for (int ci = 0; ci < circleCnt; ci++) {
        float time = float(frameCount()) / 20;
        float thetaC = map(ci, 0, circleCnt, 0, TAU);
        float scale = 300;


        maths::vec2f circleCenter = getCenterByTheta(thetaC, time, scale);
        float circleSize = getSizeByTheta(thetaC, time, scale);
        Pixel c = getColorByTheta(thetaC, time);

        stroke(c);

        beginShape();
        for (int vi = 0; vi < vertexCnt; vi++) {
            float thetaV = map(vi, 0, vertexCnt, 0, TAU);
            float x = circleCenter.x + cos(thetaV) * circleSize;
            float y = circleCenter.y + sin(thetaV) * circleSize;
            vertex(x, y);
        }
        endShape(SHAPEEND::CLOSE);
    }
}
