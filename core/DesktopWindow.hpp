#include <windows.h>

class DesktopWindow {
public:
    ATOM fwinclass;
    HWND fHandle;
    //PixelBuffer * fBackground;
    WINDOWINFO fInfo;
    int fScreenX;
    int fScreenY;
    int fSystemDpi;

    // default to default desktop
    DesktopWindow()
    {
        // First thing to do is let the system know we are
        // going to be DPI aware
        DPI_AWARENESS_CONTEXT oldContext = ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        // Get the DPI for the monitor
        fSystemDpi = ::GetDpiForSystem();

        // Get the screen size
        int fScreenX = ::GetSystemMetrics(SM_CXSCREEN);
        int fScreenY = ::GetSystemMetrics(SM_CYSCREEN);

        // create a dib section to match the size
    }
};
