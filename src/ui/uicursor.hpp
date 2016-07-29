#ifndef UI_CURSOR_HPP
#define UI_CURSOR_HPP
#include <vector>
#include "uielement.hpp"
#include "../eventhandler.hpp"

class UICursor : public UiElement{
public:
    using UiElement::UiElement;
    void handleEvent(const Event *ev);
};

#endif /* ifndef UI_CURSOR_HPP */

