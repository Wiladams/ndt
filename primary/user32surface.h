#pragma once



#include "appsurface.h"
#include "LayeredWindow.hpp"
#include "NativeWindow.hpp"
#include "blend2d.h"

#include <memory>

constexpr int bitsPerPixel = 32;
int alignment = 4;

class User32Surface : public AppSurface
{
	// This is a most generic window message handler.  If the window sub-class
	// has a message handler, call that.
	// Otherwise, call default Window's Procedure
	static LRESULT CALLBACK ClassMsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT res = 0;

		// get userdata from window handle
		User32Surface* winObject = (User32Surface *)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (winObject != nullptr) {
			return winObject->handleMsg(hWnd, msg, wParam, lParam);
		}

		// handle message in default way
		return DefWindowProcA(hWnd, msg, wParam, lParam);
	}

	User32Window *fAppWindow = nullptr;
	HWND fWindowHandle;
	HBITMAP fDIBHandle = nullptr;
	void* fData = nullptr;       // A pointer to the data
	size_t fDataSize = 0;       // How much data is allocated
	HDC     fBitmapDC = nullptr;	// Device Context that has dibsection selected

	BLImage fImage;

	User32Surface(size_t w, size_t h, HWND hWnd)
		:AppSurface(w, h),
		fWindowHandle(hWnd)
	{
		// Construct a layered window
		User32WindowClass wKind("appsurface", CS_GLOBALCLASS | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, ClassMsgHandler);
		fAppWindow = wKind.createWindow("User32 Surface", w, h, WS_POPUP, WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP);
		::SetWindowLongPtr(fAppWindow->getHandle(), GWLP_USERDATA, (LONG_PTR)this);

		// construct an in memory bitmap used for rendering
		int bytesPerRow = w * alignment;
		BITMAPINFO bminfo = {};

		bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bminfo.bmiHeader.biWidth = w;
		bminfo.bmiHeader.biHeight = -(LONG)h;	// top-down DIB Section
		bminfo.bmiHeader.biPlanes = 1;
		bminfo.bmiHeader.biBitCount = bitsPerPixel;
		bminfo.bmiHeader.biSizeImage = bytesPerRow * h;
		bminfo.bmiHeader.biClrImportant = 0;
		bminfo.bmiHeader.biClrUsed = 0;
		bminfo.bmiHeader.biCompression = BI_RGB;

		// Create the actual bitmap that's used for rendering
		fDIBHandle = CreateDIBSection(nullptr, &bminfo, DIB_RGB_COLORS, &fData, nullptr, 0);
		fDataSize = bminfo.bmiHeader.biSizeImage;

		// Create a device context so we can connect out bitmap
		// to GDI/User32 drawing routines
		fBitmapDC = CreateCompatibleDC(nullptr);

		// select the DIBSection into the memory context so we can 
		// peform operations with it
		::SelectObject(fBitmapDC, fDIBHandle);

		// Initialize the BLImage
		// MUST use the PRGB32 in order for SRC_OVER operations to work correctly
		BLResult bResult = blImageInitAsFromData(&fImage, w, h, BL_FORMAT_PRGB32, fData, (intptr_t)bytesPerRow, nullptr, nullptr);

	}

	BLImage& getImage() { return fImage; }

	// by default, don't do anything
	// sub-classes should implement this
	virtual LRESULT handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

	}

	// Display back buffer to the screen
	virtual void display()
	{
		LayeredWindowInfo lw(width(), height());
		lw.display(fWindowHandle, fBitmapDC);
	}


};