#pragma once
#include "w32_types.hpp"

/*
    This file contains any of the interesting function definitions.
*/

// Wrapping everything in this __cplusplus case is necessary because
// various Windows APIs and structures are very C specific, so we don't
// want the names to get C++ mangling
#ifdef __cplusplus
extern "C" {
#endif


#define LOWORD(l)         WORD(DWORD_PTR(l) & 0xffff)
#define HIWORD(l)         WORD((DWORD_PTR(l) >> 16) & 0xffff)

#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp)) 
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp)) 

#define GET_WHEEL_DELTA_WPARAM(w) (short)HIWORD(w)

// Conversion of touch input coordinates to pixels
#define TOUCH_COORD_TO_PIXEL(l) ((l) / 100)

/*
 * Gesture argument helpers
 *   - Angle should be a double in the range of -2pi to +2pi
 *   - Argument should be an unsigned 16-bit value
 */
#define GID_ROTATE_ANGLE_TO_ARGUMENT(_arg_)     ((USHORT)((((_arg_) + 2.0 * 3.14159265) / (4.0 * 3.14159265)) * 65535.0))
#define GID_ROTATE_ANGLE_FROM_ARGUMENT(_arg_)   ((((double)(_arg_) / 65535.0) * 4.0 * 3.14159265) - 2.0 * 3.14159265)






/*
    Function declarations
*/

BOOL __stdcall RegisterRawInputDevices(PCRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize);

// libloaderapi


HMODULE  GetModuleHandleA(const char * lpModuleName);
FARPROC __stdcall GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

// profileapi

int QueryPerformanceCounter(int64_t * lpPerformanceCount);
int QueryPerformanceFrequency(int64_t * lpFrequency);


DWORD __stdcall GetLastError(void);

// Basic Window management
BOOL AdjustWindowRect(LPRECT lpRect, DWORD  dwStyle, BOOL   bMenu);
ATOM RegisterClassExA(const WNDCLASSEXA *);
HWND CreateWindowExA(DWORD dwExStyle, const char * lpClassName, const char * lpWindowName, DWORD dwStyle,
     int X, int Y, int nWidth, int nHeight,
     HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, void * lpParam);
LRESULT DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL DestroyWindow(HWND hWnd);
BOOL GetClassInfoExA(HINSTANCE hInstance, LPCSTR lpszClass, LPWNDCLASSEXA lpwcx);
BOOL GetClientRect(HWND   hWnd, LPRECT lpRect);
BOOL GetWindowInfo(HWND hWnd, PWINDOWINFO pwi);
BOOL GetWindowRect(HWND hWnd, LPRECT lpRect);
UINT GetDpiForWindow(HWND hwnd);
int GetSystemMetrics(int nIndex);
BOOL IsWindow(HWND hWnd);
BOOL InvalidateRect(HWND hWnd, const RECT *lpRect, BOOL bErase);

HCURSOR LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
HICON LoadIconA(HINSTANCE hInstance,LPCSTR    lpIconName);
BOOL RedrawWindow(HWND hWnd, const RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags);

LONG GetWindowLongA(HWND hWnd, int nIndex);
LONG SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);

BOOL ScreenToClient(HWND hWnd, LPPOINT lpPoint);
BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter,int X,int Y,int cx,int cy, UINT uFlags);
BOOL ShowWindow(HWND hWnd, int nCmdShow);
BOOL UpdateLayeredWindow(HWND hWnd, HDC hdcDst, POINT* pptDst, SIZE* psize, 
    HDC hdcSrc, POINT* pptSrc, 
    COLORREF crKey, 
    BLENDFUNCTION* pblend, 
    DWORD dwFlags);

BOOL UpdateLayeredWindowIndirect(HWND hWnd, const UPDATELAYEREDWINDOWINFO* pULWInfo);

DPI_AWARENESS_CONTEXT SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT dpiContext);
UINT __stdcall GetDpiForSystem();

typedef void (* TIMERPROC)(HWND Arg1,UINT Arg2,UINT_PTR Arg3,DWORD Arg4);
UINT_PTR SetTimer(
  HWND      hWnd,
  UINT_PTR  nIDEvent,
  UINT      uElapse,
  TIMERPROC lpTimerFunc
);

BOOL KillTimer(
  HWND     hWnd,
  UINT_PTR uIDEvent
);

// Touch Related founctions
BOOL RegisterTouchWindow(HWND hwnd, ULONG ulFlags);
BOOL UnregisterTouchWindow(HWND hwnd);
BOOL IsTouchWindow(HWND hwnd, PULONG pulFlags);

// Regular Windows messaging

BOOL GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
void PostQuitMessage(int nExitCode);
int PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
LRESULT SendMessageA(HWND hWnd,UINT Msg,WPARAM wParam, LPARAM lParam);
int TranslateMessage(const MSG *lpMsg);
LRESULT DispatchMessageA(const MSG *lpMsg);

// Thread messaging
BOOL PostThreadMessageA(DWORD idThread, UINT Msg, WPARAM wParam, LPARAM lParam);

BOOL GetKeyboardState(PBYTE lpKeyState);
SHORT GetAsyncKeyState(int vKey);

// Windows paint
HDC BeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
int EndPaint(HWND hWnd, const PAINTSTRUCT *lpPaint);

// wingdi
HDC CreateDCA(LPCSTR pwszDriver, LPCSTR pwszDevice, LPCSTR pszPort, const DEVMODEA *pdm);

HDC GetDC(HWND hWnd);
int ReleaseDC(HWND hWnd,HDC  hDC);
HDC CreateCompatibleDC(HDC hdc);
HGDIOBJ SelectObject(HDC hdc, HGDIOBJ h);

HBITMAP CreateDIBSection(HDC hdc, const BITMAPINFO *pbmi, UINT usage, void **ppvBits, HANDLE hSection, DWORD offset);
int  BitBlt(  HDC hdc,  int x,  int y,  int cx,  int cy,  HDC hdcSrc,  int x1,  int y1,  uint32_t rop);
BOOL StretchBlt(HDC   hdcDest, int   xDest, int   yDest, int   wDest, int   hDest,
  HDC   hdcSrc, int   xSrc, int   ySrc, int   wSrc, int   hSrc,
  DWORD rop);
int  StretchDIBits( HDC hdc,  int xDest,  int yDest,  int DestWidth,  int DestHeight,  
    int xSrc,  int ySrc,  int SrcWidth,  int SrcHeight,
    const void * lpBits,  const BITMAPINFO * lpbmi,  UINT iUsage,  DWORD rop);

//BOOL Rectangle( HDC hdc,  int left,  int top,  int right,  int bottom);

// hTouchInput - input event handle; from touch message lParam
// cInputs - number of elements in the array
// pInputs - array of touch inputs
// cbSize - sizeof(TOUCHINPUT)
BOOL GetTouchInputInfo(HTOUCHINPUT hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize);                           
BOOL CloseTouchInputHandle(HTOUCHINPUT hTouchInput);                   // input event handle; from touch message lParam




/*
 * Gesture information retrieval
 *   - HGESTUREINFO is received by a window in the lParam of a WM_GESTURE message.
 */

BOOL
__stdcall
GetGestureInfo(
     HGESTUREINFO hGestureInfo,
     PGESTUREINFO pGestureInfo);


BOOL
__stdcall
GetGestureExtraArgs(
     HGESTUREINFO hGestureInfo,
     UINT cbExtraArgs,
     PBYTE pExtraArgs);


BOOL
__stdcall
CloseGestureInfoHandle(
     HGESTUREINFO hGestureInfo);




// Basic file handling
HANDLE CreateFileA(
        LPCSTR lpFileName,
        DWORD dwDesiredAccess,
        DWORD dwShareMode,
        void * lpSecurityAttributes,
        DWORD dwCreationDisposition,
        DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile);

BOOL DeleteFileA(LPCSTR lpFileName);

DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);

BOOL WriteFile(
		HANDLE       hFile,
		LPCVOID      lpBuffer,
		DWORD        nNumberOfBytesToWrite,
		LPDWORD      lpNumberOfBytesWritten,
		void * lpOverlapped);


// File mapping
HANDLE CreateFileMappingA(
		HANDLE hFile,
		void * lpAttributes,
		DWORD flProtect,
		DWORD dwMaximumSizeHigh,
		DWORD dwMaximumSizeLow,
		LPCSTR lpName);

LPVOID MapViewOfFile(
    	HANDLE hFileMappingObject,
    	DWORD dwDesiredAccess,
    	DWORD dwFileOffsetHigh,
    	DWORD dwFileOffsetLow,
    	SIZE_T dwNumberOfBytesToMap);

BOOL UnmapViewOfFile(LPCVOID lpBaseAddress);

BOOL CloseHandle(HANDLE hObject);


#ifdef __cplusplus
}
#endif
