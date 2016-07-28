#include "uielement.hpp"
#include "engine.hpp"
#include "../eventhandler.hpp"

UiElement::UiElement(const char *filename)
{
    strcpy(this->filename , filename);
    this->resource = engine.resources.getUI(filename);
    this->detach();
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
    printf("%d %d %d %d\n", (int) this->getX(),
        (int) this->getY(),
        (int) this->getW(),
        (int) this->getH());
    engine.ui.drawRect(
        (int) this->getX(),
        (int) this->getY(),
        (int) this->getW(),
        (int) this->getH(),
        this->resource->color
    );
}

void UiElement::handleEvent(const Event* ev)
{
    this->resource = engine.resources.getUI(this->filename);
}
