#include "window.hpp"
#include "../engine.hpp"

void UiWindow::draw()
{
    engine.ui.startClip(this->getX(), this->getY(), this->getW(), this->getH());
    UiElement::draw();
    for(auto element : this->getElements()) {
        element->draw();
    }
    engine.ui.endClip();
    
}

//void UiWindow::handleEvent(const Event &event)
//{
//}

void UiWindow::addElement(UiElement *element)
{
    element->attach(this);
    this->elements.push_back(element);
}

std::list<UiElement *> UiWindow::getElements()
{
    return this->elements;
}

void UiWindow::handleEvent(const Event* ev)
{
    UiElement::handleEvent(ev);
    for(auto element : this->getElements()) {
        element->handleEvent(ev);
    }

}

UiWindow::~UiWindow()
{
    for(auto element : this->getElements()) {
        delete element;
    }
}


