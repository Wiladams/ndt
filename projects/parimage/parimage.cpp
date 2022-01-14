/*
	Try out various texturing and filtering techniques
*/

#include "p5.hpp"
#include "imagesampler.h"
#include "texture.h"
#include "masker.h"
#include "tinter.h"
#include "canvas.h"

using namespace p5;
using namespace ndt;

shared_ptr<ImageTexture> imgTexture = make_shared<ImageTexture>("breakfast_small.jpg");
shared_ptr<checker_texture> checker = make_shared<checker_texture>(make_shared<SolidColorTexture>(1, 0, 0), make_shared<SolidColorTexture>(0, 1, 0));
shared_ptr<CanvasTexture> maskTexture;

shared_ptr<Texture> tex = imgTexture;
shared_ptr<GCanvas> maskCanvas;
BLFontFace fFontFace;
BLFont fFont;

void setup()
{
	createCanvas(640, 480);

	maskCanvas = std::make_shared<GCanvas>(canvasWidth, canvasHeight, BL_FORMAT_A8);
	maskCanvas->clearAll();
	maskCanvas->setFillStyle(BLRgba32(255, 255, 255));
	maskCanvas->fillRect(40, 40, 200, 200);
	maskCanvas->fillRect(320, 40, 200, 200);

	maskTexture = make_shared<CanvasTexture>(maskCanvas);

	fFontFace.createFromFile("c:\\windows\\fonts\\segoeui.ttf");
	fFont.reset();
	fFont.createFromFace(fFontFace, 170);
	maskCanvas->fillUtf8Text(BLPoint(0, 400), fFont, "blend2d");
}

void draw()
{
	// Chain two samplers together
	//auto effect = maskTexture;
	//shared_ptr<Tinter> effect = make_shared<Tinter>(vec3( 100.0f/255,65.0f/255,0), imgTexture);
	//shared_ptr<Tinter> effect = make_shared<Tinter>(vec3(.41f, .31f, .19f), imgTexture);
	//shared_ptr<Tinter> effect = make_shared<Tinter>(vec3(100.0f / 255, 65.0f / 255, 0), checker);
	//auto masker = make_shared<Masker>(maskTexture, imgTexture);
	//shared_ptr<Tinter> effect = make_shared<Tinter>(vec3(.41f, .31f, .19f), masker);

	shared_ptr<Tinter> tint = make_shared<Tinter>(vec3(.41f, .31f, .19f), imgTexture);
	auto effect = make_shared<Masker>(maskTexture, tint);


	int xskip = 1;
	int yskip = 1;

	loadPixels();
	for (int y = 0; y < canvasHeight; y += yskip) {
		double v = (float)(canvasHeight - 1 - y) / ((float)canvasHeight - 1);
		for (int x = 0; x < canvasWidth; x += xskip) {
			double u = (float)x / ((float)canvasWidth - 1);
			auto c = effect->value(u, v, {});

			set(x, y, BLRgba32((uint32_t)(c.r * 255), (uint32_t)(c.g * 255), (uint32_t)(c.b * 255)));
		}
	}

	updatePixels();

	noLoop();
}