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

const Event *EventHandler::poll()
{
    static Event out;

    auto ev = events.begin();

    if (ev != events.end()) {
        out = ev[0];
        events.erase(ev);
        return &out;
    }

    return nullptr;
}

void EventHandler::trigger(const std::string &event, const std::string &data)
{
    events.push_back({
        event,
        data
    });
}
