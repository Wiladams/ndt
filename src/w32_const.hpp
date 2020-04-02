#pragma once

/*
    Windows has thousands of me
*/
static const int ERROR_ACCESS_DENIED = 5;
static const int ERROR_ALREADY_EXISTS = 183;
static const int ERROR_INVALID_HANDLE = 6;
static const int ERROR_INVALID_PARAMETER = 87;

static const int GENERIC_READ  = 0x80000000;
static const int GENERIC_WRITE = 0x40000000;

static const int OPEN_EXISTING = 3;
static const int OPEN_ALWAYS   = 4;

static const int FILE_ATTRIBUTE_ARCHIVE = 0x20;
static const int FILE_FLAG_RANDOM_ACCESS = 0x10000000;
static const int FILE_BEGIN            = 0;

//local FILE_MAP_EXECUTE	= 0;
static const int FILE_MAP_READ		= 0x04;
static const int FILE_MAP_WRITE	= 0x02;
//local FILE_MAP_TARGETS_INVALID = 0
static const int FILE_MAP_ALL_ACCESS = 0xf001f;

static const int PAGE_READONLY         = 0x02; 
static const int PAGE_READWRITE        = 0x4;

// winuser

// Class styles
static const int CS_VREDRAW         = 0x0001;
static const int CS_HREDRAW         = 0x0002;
static const int CS_OWNDC           = 0x0020;

// Extended Window Styles
static const int WS_EX_TOPMOST           = 0x00000008L;
static const int WS_EX_ACCEPTFILES       = 0x00000010L;
static const int WS_EX_TRANSPARENT       = 0x00000020L;

static const int WS_EX_LAYERED           = 0x00080000;
static const int WS_EX_NOREDIRECTIONBITMAP = 0x00200000;

// Window styles
static const int WS_OVERLAPPED       = 0x00000000;
static const int WS_CAPTION          = 0x00C00000;     /* WS_BORDER | WS_DLGFRAME  */
static const int WS_SYSMENU          = 0x00080000;
static const int WS_THICKFRAME       = 0x00040000;
static const int WS_MINIMIZEBOX      = 0x00020000;
static const int WS_MAXIMIZEBOX      = 0x00010000;
static const int WS_VISIBLE          = 0x10000000L;
static const int WS_CHILD            = 0x40000000L;
static const int WS_POPUP            = 0x80000000L;
static const int WS_OVERLAPPEDWINDOW = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;

// window showing
static const int SW_HIDE            = 0;
static const int SW_SHOWNORMAL      = 1;
static const int SW_NORMAL          = 1;
static const int SW_SHOW            = 5;

// Window redrawing
static const int RDW_INVALIDATE          = 0x0001;
static const int RDW_INTERNALPAINT       = 0x0002;
static const int RDW_ERASE               = 0x0004;

static const int RDW_VALIDATE            = 0x0008;
static const int RDW_NOINTERNALPAINT     = 0x0010;
static const int RDW_NOERASE             = 0x0020;


static const int RDW_UPDATENOW           = 0x0100;
static const int RDW_ERASENOW            = 0x0200;



static const int PM_REMOVE  =  0x0001;

static const int CW_USEDEFAULT      = 0x80000000;
static const int COLOR_WINDOW          =  5;

// Layered Window Constants
static const int  LWA_COLORKEY          =  0x00000001;
static const int  LWA_ALPHA             =  0x00000002;


static const int  ULW_COLORKEY          =  0x00000001;
static const int  ULW_ALPHA             =  0x00000002;
static const int  ULW_OPAQUE            =  0x00000004;

static const int  ULW_EX_NORESIZE       =  0x00000008;

// GDI constants
//static const int BI_RGB       = 0;
static const int DIB_RGB_COLORS     = 0;                /* color table in RGBs */
static const int SRCCOPY           =  0x00CC0020;       /* dest = source                   */
static const int CAPTUREBLT         = 0x40000000; /* Include layered windows */


enum  {
    BI_RGB = 0,
    BI_RLE8 =1,
    BI_RLE4 = 2,
    BI_BITFIELDS = 3,
    BI_JPEG = 4,
    BI_PNG = 5,
    BI_ALPHABITFIELDS = 6,
    BI_CMYK = 11,
    BI_CMYKRLE8 = 12,
    BI_CMYKRLE4 = 13
};

// GetSystemMetrics

static const int SM_CXSCREEN = 0;
static const int SM_CYSCREEN = 1;



/*
 * SetWindowPos Flags
 */
static const int SWP_NOSIZE         = 0x0001;
static const int SWP_NOMOVE         = 0x0002;
static const int SWP_NOZORDER       = 0x0004;
static const int SWP_NOREDRAW       = 0x0008;
static const int SWP_NOACTIVATE     = 0x0010;
static const int SWP_FRAMECHANGED   = 0x0020;  /* The frame changed: send WM_NCCALCSIZE */
static const int SWP_SHOWWINDOW     = 0x0040;
static const int SWP_HIDEWINDOW     = 0x0080;
static const int SWP_NOCOPYBITS     = 0x0100;
static const int SWP_NOOWNERZORDER  = 0x0200;  /* Don't do owner Z ordering */
static const int SWP_NOSENDCHANGING = 0x0400;  /* Don't send WM_WINDOWPOSCHANGING */

static const int SWP_DRAWFRAME      = SWP_FRAMECHANGED;
static const int SWP_NOREPOSITION   = SWP_NOOWNERZORDER;

static const int AC_SRC_OVER                = 0x00;
static const int AC_SRC_ALPHA               = 0x01;








static const int MK_LBUTTON         = 0x0001;
static const int MK_RBUTTON         = 0x0002;
static const int MK_SHIFT           = 0x0004;
static const int MK_CONTROL         = 0x0008;
static const int MK_MBUTTON         = 0x0010;
static const int MK_XBUTTON1        = 0x0020;
static const int MK_XBUTTON2        = 0x0040;


// Window Messages

static const int WM_DESTROY     = 0x0002;
static const int WM_PAINT       = 0x000F;
static const int WM_QUIT        = 0x0012;

static const int WM_ERASEBKGND  =  0x0014;


// raw input

static const int WM_INPUT_DEVICE_CHANGE          = 0x00FE;
static const int WM_INPUT                        = 0x00FF;


// Keyboard messages

static const int WM_KEYFIRST                     = 0x0100;
static const int WM_KEYDOWN                      = 0x0100;
static const int WM_KEYUP                        = 0x0101;
static const int WM_CHAR                         = 0x0102;
static const int WM_DEADCHAR                     = 0x0103;
static const int WM_SYSKEYDOWN                   = 0x0104;
static const int WM_SYSKEYUP                     = 0x0105;
static const int WM_SYSCHAR                      = 0x0106;
static const int WM_SYSDEADCHAR                  = 0x0107;

static const int WM_UNICHAR                      = 0x0109;
static const int WM_KEYLAST                      = 0x0109;

static const int WM_TIMER 			             = 0x0113;

// Mouse Messages

static const int WM_MOUSEFIRST                   = 0x0200;
static const int WM_MOUSEMOVE                    = 0x0200;
static const int WM_LBUTTONDOWN                  = 0x0201;
static const int WM_LBUTTONUP                    = 0x0202;
static const int WM_LBUTTONDBLCLK                = 0x0203;
static const int WM_RBUTTONDOWN                  = 0x0204;
static const int WM_RBUTTONUP                    = 0x0205;
static const int WM_RBUTTONDBLCLK                = 0x0206;
static const int WM_MBUTTONDOWN                  = 0x0207;
static const int WM_MBUTTONUP                    = 0x0208;
static const int WM_MBUTTONDBLCLK                = 0x0209;
static const int WM_MOUSEWHEEL                   = 0x020A;
static const int WM_XBUTTONDOWN                  = 0x020B;
static const int WM_XBUTTONUP                    = 0x020C;
static const int WM_XBUTTONDBLCLK                = 0x020D;
static const int WM_MOUSEHWHEEL                  = 0x020E;
static const int WM_MOUSELAST                    = 0x020E;



static const int WM_MOUSEHOVER                   = 0x02A1;
static const int WM_MOUSELEAVE                   = 0x02A3;





static const int WM_DPICHANGED                   = 0x02E0;

// Multimedia Extensions Window Messages

static const int MM_JOY1MOVE         = 0x3A0;           /* joystick */
static const int MM_JOY2MOVE         = 0x3A1;
static const int MM_JOY1ZMOVE        = 0x3A2;
static const int MM_JOY2ZMOVE        = 0x3A3;
static const int MM_JOY1BUTTONDOWN   = 0x3B5;
static const int MM_JOY2BUTTONDOWN   = 0x3B6;
static const int MM_JOY1BUTTONUP     = 0x3B7;
static const int MM_JOY2BUTTONUP     = 0x3B8;

static const int WM_USER 			= 0x0400;

// Touch Window Messages

static const int WM_TOUCH                        = 0x0240;
static const int WM_GESTURE                      = 0x0119;
static const int WM_GESTURENOTIFY                = 0x011A;



/*
 * Touch input flag values (TOUCHINPUT.dwFlags)
 */
static const int TOUCHEVENTF_MOVE            = 0x0001;
static const int TOUCHEVENTF_DOWN            = 0x0002;
static const int TOUCHEVENTF_UP              = 0x0004;
static const int TOUCHEVENTF_INRANGE         = 0x0008;
static const int TOUCHEVENTF_PRIMARY         = 0x0010;
static const int TOUCHEVENTF_NOCOALESCE      = 0x0020;
static const int TOUCHEVENTF_PEN             = 0x0040;
static const int TOUCHEVENTF_PALM            = 0x0080;

/*
 * Touch input mask values (TOUCHINPUT.dwMask)
 */
static const int TOUCHINPUTMASKF_TIMEFROMSYSTEM  = 0x0001;  // the dwTime field contains a system generated value
static const int TOUCHINPUTMASKF_EXTRAINFO       = 0x0002;  // the dwExtraInfo field is valid
static const int TOUCHINPUTMASKF_CONTACTAREA     = 0x0004;  // the cxContact and cyContact fields are valid

// RegisterTouchWindow flag values
static const int TWF_FINETOUCH      = 0x00000001;
static const int TWF_WANTPALM       = 0x00000002;






/*
 * Gesture flags - GESTUREINFO.dwFlags
 */
static const int GF_BEGIN                       = 0x00000001;
static const int GF_INERTIA                     = 0x00000002;
static const int GF_END                         = 0x00000004;

/*
 * Gesture IDs
 */
static const int GID_BEGIN                      = 1;
static const int GID_END                        = 2;
static const int GID_ZOOM                       = 3;
static const int GID_PAN                        = 4;
static const int GID_ROTATE                     = 5;
static const int GID_TWOFINGERTAP               = 6;
static const int GID_PRESSANDTAP                = 7;
static const int GID_ROLLOVER                   = GID_PRESSANDTAP;







//    Related to raw input

static const int RIDEV_REMOVE           = 0x00000001;
static const int RIDEV_EXCLUDE          = 0x00000010;
static const int RIDEV_PAGEONLY         = 0x00000020;
static const int RIDEV_NOLEGACY         = 0x00000030;
static const int RIDEV_INPUTSINK        = 0x00000100;
static const int RIDEV_CAPTUREMOUSE     = 0x00000200;  // effective when mouse nolegacy is specified, otherwise it would be an error
static const int RIDEV_NOHOTKEYS        = 0x00000200;  // effective for keyboard.
static const int RIDEV_APPKEYS          = 0x00000400; // effective for keyboard.
static const int RIDEV_EXINPUTSINK      = 0x00001000;
static const int RIDEV_DEVNOTIFY        = 0x00002000;
static const int RIDEV_EXMODEMASK       = 0x000000F0;


static const int GIDC_ARRIVAL        =     1;
static const int GIDC_REMOVAL        =     2;

static const int    RIM_INPUT      = 0;
static const int    RIM_INPUTSINK  = 1;

#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE  (DPI_AWARENESS_CONTEXT,-2);
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2  (DPI_AWARENESS_CONTEXT,-4);

static const HANDLE INVALID_HANDLE_VALUE = ((HANDLE)(LONG_PTR)-1);
