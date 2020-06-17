#include "p5.hpp"
#include "imagesampler.h"
#include "texture.h"

using namespace p5;
using namespace ndt;

shared_ptr<ImageTexture> imgTexture = make_shared<ImageTexture>("breakfast_small.jpg");
shared_ptr<checker_texture> checker = make_shared<checker_texture>(make_shared<SolidColorTexture>(1, 0, 0), make_shared<SolidColorTexture>(0, 1, 0));

shared_ptr<Texture> tex = imgTexture;

// Use a texture as a mask for a source
class Masker : public Texture
{
	shared_ptr<Texture> fSource;
	shared_ptr<Texture> fMask;
	
public:
	Masker(shared_ptr<Texture> src, shared_ptr<Texture> mask)
		:fSource(src),
		fMask(mask)
	{}

	virtual rtcolor value(double u, double v, const vec3& p) const
	{
		// get pixel from source
		auto srcC = fSource->value(u, v, p);
		auto maskC = fMask->value(u, v, p);

		// either do a blend, or a basic threshold
		if (maskC.r == 0 && maskC.g == 0 && maskC.b == 0)
			return maskC;

		return srcC;
	}
};

class Tinter : public Texture
{
	shared_ptr<Texture> fSource;
	vec3 fTint;

public:
	Tinter(const vec3& tint, shared_ptr<Texture> src)
		:fSource(src),
		fTint(tint)
	{
	}

	virtual rtcolor value(double u, double v, const vec3& p) const
	{
		// get pixel from source
		auto srcC = fSource->value(u, v, p);

		// add tint
		return vec3(constrain((srcC.r + fTint.r), 0, 1.0f), constrain((srcC.g + fTint.g), 0, 1.0f), constrain((srcC.b + fTint.b), 0, 1.0f));
	}
};


void setup()
{
	// Simple scaling can occur by varying the 
	// size of the window.
	// the image sampler will make it work
	//createCanvas(displayWidth, displayHeight);
	createCanvas(640, 480);
	noLoop();
}

void draw()
{
	// Chain two samplers together
	//shared_ptr<Tinter> effect = make_shared<Tinter>(vec3( 100.0f/255,65.0f/255,0), imgTexture);
	shared_ptr<Tinter> effect = make_shared<Tinter>(vec3(.41f, .31f, .19f), imgTexture);
	//shared_ptr<Tinter> effect = make_shared<Tinter>(vec3(100.0f / 255, 65.0f / 255, 0), checker);

	int xskip = 1;
	int yskip = 1;

	loadPixels();
	for (int y = 0; y < height; y += yskip) {
		double v = (float)(height-1-y) / ((float)height-1);
		for (int x = 0; x < width; x += xskip) {
			double u = (float)x / ((float)width-1);
			auto c = effect->value(u, v, {float(x%(width/8)),float(y%(height/8)),1.0f});
			//auto c = checker->value(u, v, { float(x % (width / 8)),float(y % (height / 8)),1.0f });

			set(x, y, BLRgba32(c.r*255, c.g*255,c.b*255));
		}
	}
	updatePixels();
}