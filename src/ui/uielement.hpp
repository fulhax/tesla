#ifndef UI_RECT_HPP
#define UI_RECT_HPP
#include <string>
#include <glm/glm.hpp>
#include "../eventhandler.hpp"
#include "../resource.hpp"

class UiElement
{
public:
    explicit UiElement(const char *filename);
    virtual void attach(UiElement *parent);
    virtual void detach();
    virtual bool inBounds(float x, float y);
    virtual void handleEvent(const Event* ev);
    virtual void draw();
    virtual void resize(unsigned int w, unsigned int h);
    virtual void move(float x, float y);
    float getX();
    float getY();
    unsigned int getW();
    unsigned int getH();
    UiElement *parent;
    UiResource *resource;
    char filename[255];
private:
protected:
};

#endif // UI_RECT_HPP
