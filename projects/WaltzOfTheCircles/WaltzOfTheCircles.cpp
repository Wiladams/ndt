#include "p5.hpp"

using namespace p5;

/*
  based on: https://observablehq.com/@rreusser/instanced-webgl-circles
*/
constexpr int MAX_CIRCLE_CNT = 2500;
constexpr int MIN_CIRCLE_CNT = 100;
constexpr int MAX_VERTEX_CNT = 30;
constexpr int MIN_VERTEX_CNT = 3;
constexpr double TAU = 6.2831855;
constexpr double PI = 3.14159;

int circleCnt, vertexCnt;

void updateCntByMouse() {
    double xoffset = abs(mouseX - canvasWidth / 2.0), yoffset = abs(mouseY - canvasHeight / 2.0);
    circleCnt = int(map(xoffset, 0, canvasWidth / 2.0, MAX_CIRCLE_CNT, MIN_CIRCLE_CNT));
    vertexCnt = int(map(yoffset, 0, canvasHeight / 2.0, MAX_VERTEX_CNT, MIN_VERTEX_CNT));
}

PVector getCenterByTheta(double theta, double time, double scale) {
    PVector direction = PVector(cos(theta), sin(theta));
    double distance = 0.6 + 0.2 * cos(theta * 6.0 + cos(theta * 8.0 + time));
    PVector circleCenter = PVector::mult(direction,distance * scale);

    return circleCenter;
}

float getSizeByTheta(double theta, double time, double scale) {
    double offset = 0.2 + 0.12 * cos(theta * 9.0 - time * 2.0);
    double circleSize = scale * offset;
    return circleSize;
}

Pixel getColorByTheta(double theta, double time) {
    double th = 8.0 * theta + time * 2.0;
    double r = 0.6 + 0.4 * cos(th),
        g = 0.6 + 0.4 * cos(th - PI / 3),
        b = 0.6 + 0.4 * cos(th - PI * 2.0 / 3.0),
        alpha = map(circleCnt, MIN_CIRCLE_CNT, MAX_CIRCLE_CNT, 150, 30);
    return Pixel(r * 255, g * 255, b * 255, alpha);
}

void setup()
{
    //createCanvas(700, 700);
    fullscreen();

    frameRate(10);
}

void draw() {
    background(0);

    translate(canvasWidth / 2, canvasHeight / 2);

    updateCntByMouse();
    noFill();

    for (int ci = 0; ci < circleCnt; ci++) {
        float time = float(frameCount) / 20;
        float thetaC = map(ci, 0, circleCnt, 0, TAU);
        float scale = 300;


        PVector circleCenter = getCenterByTheta(thetaC, time, scale);
        float circleSize = getSizeByTheta(thetaC, time, scale);
        Pixel c = getColorByTheta(thetaC, time);

        stroke(c);

        beginShape();
        for (int vi = 0; vi < vertexCnt; vi++) {
            double thetaV = map(vi, 0, vertexCnt, 0, TAU);
            double x = circleCenter.x + cos(thetaV) * circleSize;
            double y = circleCenter.y + sin(thetaV) * circleSize;
            vertex(x, y);
        }
        endShape(SHAPEEND::CLOSE);
    }
}
