#include "p5.hpp"
#include "texture.h"
#include "User32PixelMap.h"
#include "dcsnapper.h"

#include <memory>


using namespace p5;

std::shared_ptr<DCSnapper> winSnapper = nullptr;		// picture of window
HWND gSelectedWindow = nullptr;



struct MouseHooker
{
	int fKind;
	HHOOK fHook;

	// track the mouse, and select a window
	// to capture when mouse clicks
	static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (lParam == NULL) {
			return 0;
		}
		
		MSLLHOOKSTRUCT* mhs = (MSLLHOOKSTRUCT*)lParam;

		//printf("--------------------------------\n");
		//printf("MouseHookProc: %d\n", nCode);
		//printf("          MSG: 0x%04x\n", wParam);
		//printf("     Position: %d %d\n", mhs->pt.x, mhs->pt.y);

		// Get handle to window under point if it exists
		HWND hwnd = WindowFromPoint(mhs->pt);

		if (hwnd == NULL)
			return 0;


		switch ((UINT)wParam) 
		{
		case WM_MOUSEMOVE:
			//printf("     MOVE: %d %d\n", mhs->pt.x, mhs->pt.y);
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:

			break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			int button = 0xffff0000 & mhs->mouseData;

			//printf(" MOUSE UP: %d\n", button);

			HWND hwnd = WindowFromPoint(mhs->pt);

			// Get module name of window
			char modulename[2048];
			size_t res = GetWindowModuleFileNameA(hwnd, modulename, 2048);
			modulename[res] = 0;

			char title[2048];
			int wint = GetWindowTextA(hwnd, title, 2048);

			printf("--------------------------------\n");
			printf("         HWND: 0x%x\n", hwnd);
			//printf("       Module: (%d) %s\n", res, modulename);
			printf("        Title: %s\n", title);
			//printf("Size: %d, %d %d X %d\n", winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top);

			if (!strcmp(title, "Calculator") && (winSnapper == nullptr))
			{
				gSelectedWindow = hwnd;
			}
		}


		break;

		case WM_MOUSEWHEEL:
		{
			int delta = GET_WHEEL_DELTA_WPARAM(mhs->mouseData);
			printf(" WHEEL: %d\n", delta/WHEEL_DELTA);

			//e.activity = MOUSEWHEEL;
			//e.delta = GET_WHEEL_DELTA_WPARAM(wParam);
		}
			break;

		default:

			break;
		}

		if (nCode < 0)
			return CallNextHookEx(NULL, nCode, wParam, lParam);

		return 0;
	}

	MouseHooker()
	{
		init(WH_MOUSE_LL);
	}

	void init(int kind)
	{
		fKind = kind;
		fHook = NULL;
	}

	bool start()
	{
		HINSTANCE hmod = NULL;
		DWORD dwThreadId = 0;

		fHook = ::SetWindowsHookExA(fKind, MouseHookProc, hmod, dwThreadId);

		return fHook != NULL;
	}

	bool stop()
	{
		BOOL res = ::UnhookWindowsHookEx(fHook);

		return res!= 0;
	}
};


MouseHooker hooker;

void draw()
{
	background(255, 127, 127);

	//printf("raw mouse: %d %d\n", rawMouseX, rawMouseY);
	if (gSelectedWindow != nullptr)
	{
		if (nullptr == winSnapper)
		{
			RECT winRect;
			GetWindowRect(gSelectedWindow, &winRect);
			size_t winWidth = winRect.right - winRect.left;
			size_t winHeight = winRect.bottom - winRect.top;
			printf("Win SIZE: %d %d\n", winWidth, winHeight);

			HDC windc = GetWindowDC(gSelectedWindow);
			//HDC windc = GetDC(NULL);
			printf("WIN DC: 0x%p\n", windc);

			winSnapper = std::make_shared<DCSnapper>(windc, winRect.left, winRect.top, winWidth, winHeight);
		
			setCanvasSize(winWidth, winHeight);
		}
	}

	if (nullptr != winSnapper)
	{
		winSnapper->next();
		
		// display snapshot
		//printf("WIN SNAP: %d, %d\n", winSnapper->width(), winSnapper->height());
		image(winSnapper->image(), 0, 0);
		//gAppSurface->image(winSnapper->image(), 10, 10);
	}
}

void setup() {
	createCanvas(320, 240, "trackwin");

	//winSnapper = std::make_shared<DCSnapper>(GetDC(NULL),0,0,320,240);

	hooker.start();
}
