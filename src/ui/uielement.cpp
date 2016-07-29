#include "uielement.hpp"
#include "engine.hpp"
#include "../eventhandler.hpp"

#define EQUAL(a,b) (strcmp(a,b) == 0)

UiElement::UiElement()
{
    this->resource = nullptr;
    this->movable = true;
    this->resizable = false;
}

UiElement::~UiElement()
{
    for(int i = this->elements.size() - 1; i >= 0; i--) {
        delete this->elements[i];
    }

    this->elements.clear();
}

void UiElement::addElement(UiElement *element)
{
    element->attach(this);
    this->elements.push_back(element);
}

std::vector<UiElement *> UiElement::getElements()
{
    return this->elements;
}

UiElement::UiElement(const char *filename)
{
    strcpy(this->filename , filename);
    this->detach();
    //this->resource = engine.resources.getUI(filename);
}

float UiElement::getX()
{
    if(this->parent != nullptr) {
        return this->parent->resource->x + this->resource->x;
    } else {
        return this->resource->x;
    }
}

float UiElement::getY()
{
    if(this->parent != nullptr) {
        return this->parent->resource->y + this->resource->y;
    } else {
        return this->resource->y;
    }
}

unsigned int UiElement::getW()
{
    return this->resource->w;
}

unsigned int UiElement::getH()
{
    return this->resource->h;
}

void UiElement::attach(UiElement *parent)
{
    this->parent = parent;
}

void UiElement::detach()
{
    this->parent = nullptr;
}

void UiElement::handleEvent(const Event *ev)
{

    if(this->inBounds(engine.mouse.x, engine.mouse.y)) {
        if((EQUAL("camera.yaw", ev->event.c_str()) ||
            EQUAL("camera.pitch", ev->event.c_str())) &&
            SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)
        ) {
            this->move(
                this->getX() + engine.mouse.rx,
                this->getY() + engine.mouse.ry
            );
        }
    }

    for(auto element : this->getElements()) {
        element->handleEvent(ev);
    }
}


bool UiElement::inBounds(float x, float y)
{
    return ((x > this->getX() && x < this->getX() + (float)this->getW()) &&
            (y > this->getY() && y < this->getY() + (float)this->getH()));
};

void UiElement::resize(unsigned int w, unsigned int h)
{
    this->resource->w = w;
    this->resource->h = h;
}

void UiElement::move(float x, float y)
{
    this->resource->x = x;
    this->resource->y = y;
}

void UiElement::draw()
{

    engine.ui.drawRect(
        (int) this->getX(),
        (int) this->getY(),
        (int) this->getW(),
        (int) this->getH(),
        this->resource->color
    );

    int cm[4] = {this->getX(), this->getY(), this->getW(), this->getH()};

    for(auto element : this->getElements()) {
        if(this->resource->clip) {
            engine.ui.startClip(cm[0], cm[1], cm[2], cm[3]);
        }

        element->draw();

        if(this->resource->clip) {
            engine.ui.endClip();
        }
    }
}
