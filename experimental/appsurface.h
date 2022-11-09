#pragma once

class AppSurface
{
	size_t fWidth;
	size_t fHeight;

public:
	AppSurface(size_t w, size_t h)
		:fWidth(w),
		fHeight(h)
	{}

	size_t width() { return fWidth; }
	size_t height() { return fHeight; }

	// Display contents to user
	virtual void display() = 0;
};