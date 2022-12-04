#pragma once

#include "apphost.h"
#include "graphic.hpp"

// Things a studio application can implement
#ifdef __cplusplus
extern "C" {
#endif
    APP_EXPORT void setup();

#ifdef __cplusplus
}
#endif

// Functions a studio application can call
void addGraphic(std::shared_ptr<GraphicElement> g);
std::shared_ptr<GraphicElement> hoverGraphic();

void setDesktopDrawing(std::function<void(IGraphics& ctx)> func);
void setWindowDrawing(std::function<void(IGraphics& ctx, std::shared_ptr<GraphicGroup> gs)> func);