#define RAYGUI_STANDALONE
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#define RICONS_IMPLEMENTATION

#include "raygui.h"
#include "p5.hpp"

using namespace p5;

// Input required functions
//-------------------------------------------------------------------------------
static Vector2 GetMousePosition(void)
{
	return Vector2{};
}

static int GetMouseWheelMove(void)
{
	return 0;
}

static bool IsMouseButtonDown(int button)
{
	return false;
}

static bool IsMouseButtonPressed(int button)
{
	return false;
}

static bool IsMouseButtonReleased(int button)
{
	return false;
}

static bool IsKeyDown(int key)
{
	return false; 
}

static bool IsKeyPressed(int key)
{
	return false;
}

static int GetKeyPressed(void)         // -- GuiTextBox(), GuiTextBoxMulti(), GuiValueBox()
{
	return 0;
}

Pixel colorToPixel(const Color& c)
{
	return Pixel(c.r, c.g, c.b, c.a);
}

//-------------------------------------------------------------------------------
// Drawing required functions
//-------------------------------------------------------------------------------
static void DrawRectangle(int x, int y, int width, int height, Color c)
{
	fill(colorToPixel(c));
	rect(x, y, width, height);
}

static void DrawRectangleGradientEx(GRectangle rec, Color col1, Color col2, Color col3, Color col4);     // -- GuiColorPicker()

static void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color c)                              // -- GuiDropdownBox(), GuiScrollBar()
{
	fill(colorToPixel(c));
	triangle(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y);
}



static void DrawTextRec(Font font, const char* text, GRectangle rec, float fontSize, float spacing, bool wordWrap, Color tint) // -- GuiTextBoxMulti()
{

}

//-------------------------------------------------------------------------------
// Text required functions
//-------------------------------------------------------------------------------
static Font GetFontDefault(void);   // -- GuiLoadStyleDefault()
static Vector2 MeasureTextEx(Font font, const char* text, float fontSize, float spacing);                          // -- GetTextWidth(), GuiTextBoxMulti()
static void DrawTextEx(Font font, const char* text, Vector2 position, float fontSize, float spacing, Color tint);  // -- GuiDrawText()

static Font LoadFontEx(const char* fileName, int fontSize, int* fontChars, int charsCount);  // -- GuiLoadStyle()
static char* LoadText(const char* fileName); // -- GuiLoadStyle()
static const char* GetDirectoryPath(const char* filePath);  // -- GuiLoadStyle()