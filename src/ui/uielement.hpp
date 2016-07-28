#ifndef UI_RECT_HPP
#define UI_RECT_HPP
#include <string>
#include <glm/glm.hpp>
#include "../eventhandler.hpp"

class UiElement
{
public:
    UiElement(
        int id,
        float x,
        float y,
        float z,
        unsigned int w,
        unsigned int h,
        bool movable,
        bool resizable,
        glm::vec3 color
    );
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
    float x, y, z;
private:
    unsigned int id;
    bool movable;
    bool resizable;
    unsigned int w, h;
    glm::vec3 color;
protected:
};

#endif // UI_RECT_HPP
