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

// void UiElement::handleEvent(const Event &e)
// {
//     switch(e.type) {
//         case EventType::MouseMotion: {
//             EventMouseMotion *mm = (EventMouseMotion *) &e;
// 
//             if(this->inBounds(mm->x, mm->y)) {
//                 this->ui->addEvent(OEvent(this->id, EventType::Hover));
// 
//                 if(mm->state == MouseButtonState::BUTTON_DOWN) {
//                     switch(mm->button) {
//                         case MouseButton::BUTTON_LEFT:
//                             if(this->movable == false) {
//                                 break;
//                             }
// 
//                             this->move(this->x += mm->rx, this->y += mm->ry);
//                             this->ui->addEvent(
//                                 OEvent(this->id, EventType::Drag)
//                             );
//                             break;
// 
//                         case MouseButton::BUTTON_RIGHT:
//                             if(this->resizable == false) {
//                                 break;
//                             }
// 
//                             this->resize(this->w += mm->rx, this->h += mm->ry);
//                             this->ui->addEvent(
//                                 OEvent(this->id, EventType::Resize)
//                             );
//                             break;
// 
// 
//                         default:
//                             break;
//                     }
//                 }
//             }
//         }
//         break;
// 
//         default:
//             break;
//             /*
//                 case EventType::MouseWheel: {
//                 EventMouseWheel *mm = (EventMouseWheel *) &e;
//                 printf("Mouse wheel, x:%f y:%f\n", mm->x, mm->y);
//                 }
//                 break;
//                     case EventType::MouseButton: {
//                     EventMouseButton *me = (EventMouseButton *) &e;
//                     if(this->inBounds(me->x, me->y)) {
//                     printf("Click\n");
//                     this->ui->addEvent(OEvent(this->id, EventType::Click));
//                     }
//                     }
//                     break;
//                     case EventType::TextInput: {
//                     EventTextInput *te = (EventTextInput *) &e;
//                     printf("%s\n", te->key);
//                     }
//                     break;
//             */
//     }
// }

void UiElement::handleEvent(const Event* ev)
{
    //UiElement::handleEvent(ev);
    //this->resource = engine.resources.getUI(this->filename);
    printf("%d", this->getElements().size());
    for(auto element : this->getElements()) {
        element->handleEvent(ev);
    }
}


bool UiElement::inBounds(float x, float y)
{
    return (
       (x > this->getX() && x < this->getX() + (float)this->getW()) &&
       (y > this->getY() && y < this->getY() + (float)this->getH())
   );
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
    engine.ui.startClip((int) this->getX(), (int)this->getY(), this->getW(), this->getH());
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
