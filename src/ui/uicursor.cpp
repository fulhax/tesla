#include "uicursor.hpp"
#include "../engine.hpp"

void UICursor::handleEvent(const Event* ev){
    this->resource->x = engine.mouse.x; 
    this->resource->y = engine.mouse.y; 
}
