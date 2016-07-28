#ifndef UI_WINDOW_HPP
#define UI_WINDOW_HPP
#include <list>
#include "uielement.hpp"

class UiWindow : public UiElement
{
public:
    using UiElement::UiElement;
    virtual void draw();
    virtual void addElement(UiElement *element);
    virtual void handleEvent(const Event* ev);
    virtual std::list<UiElement *> getElements();
    ~UiWindow();
private:
    std::list<UiElement *> elements;
};

#endif /* ifndef UI_WINDOW_HPP */
