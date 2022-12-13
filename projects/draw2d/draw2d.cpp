#include "apphost.h"

// implementation of bresenham's line algorithm
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void draw_line(PixelAccessor<maths::vec4b>& pix, int x0, int y0, int x1, int y1, const maths::vec4b& color)
{
	int dx = abs(x1 - x0);
	int sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0);
	int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;  /* error value e_xy */
	int e2; /* error value e_xy */

	while (true) {
		/* loop */
		pix.setPixel(x0, y0, color);

		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { /* e_xy+e_x > 0 */
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err += dx;
			y0 += sy;
		}
	}
}

// implement bresenham's circle algorithm
// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void draw_circle(PixelAccessor<maths::vec4b>& pix, int x0, int y0, int radius, const maths::vec4b& color)
{
	int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1); /* shifting bits left by 1 effectively
									multiplies by 2 */

	while (x >= y)
	{
		pix.setPixel(x0 + x, y0 + y, color);
		pix.setPixel(x0 + y, y0 + x, color);
		pix.setPixel(x0 - y, y0 + x, color);
		pix.setPixel(x0 - x, y0 + y, color);
		pix.setPixel(x0 - x, y0 - y, color);
		pix.setPixel(x0 - y, y0 - x, color);
		pix.setPixel(x0 + y, y0 - x, color);
		pix.setPixel(x0 + x, y0 - y, color);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}

		if (err > 0)
		{
			x--;
			dx += 2;
			err += dx - (radius << 1);
		}
	}
}

// Implement polygon filling routine





auto rgb = [](int r, int g, int b) { return maths::vec4b{ (uint8_t)b, (uint8_t)g, (uint8_t)r, 255 }; };

void onLoad()
{
	auto bkgColor = rgb(255, 255, 0);

	// set background of application framebuffer to red
	appFrameBuffer().setAllPixels(bkgColor);

	// Draw a random line
	draw_line(appFrameBuffer(), 0, 0, 100, 100, rgb( 255,0, 0));
	draw_circle(appFrameBuffer(), 100, 100, 50, rgb(0,255,0));
}