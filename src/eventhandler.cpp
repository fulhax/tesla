#include "eventhandler.hpp"

#include <string.h>

#include <string>
#include <vector>

EventHandler::EventHandler()
{

}

EventHandler::~EventHandler()
{
    events.clear();
}

int EventHandler::count()
{
    return events.size();
}

const std::string *EventHandler::poll()
{
    static std::string out;

    auto ev = events.begin();
    if(ev != events.end()) {
        out = ev[0];
        events.erase(ev);
        return &out;
    }
    return nullptr;
}

void EventHandler::trigger(const std::string &event)
{
    for(auto ev : events) {
        if(strcmp(event.c_str(), ev.c_str()) == 0) {
            return;
        }
    }

    events.push_back(event);
}
