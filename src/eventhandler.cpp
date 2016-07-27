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

        if (ev->keep == true) {
            keptevents.push_back(out);
        }

        events.erase(ev);

        return &out;
    }

    return nullptr;
}

void EventHandler::update()
{
    auto it = std::next(keptevents.begin(), keptevents.size());
    std::move(keptevents.begin(), it, std::back_inserter(events));
    keptevents.erase(keptevents.begin(), it);
}

void EventHandler::untrigger(const std::string &event)
{
    for (auto ev = events.begin(); ev != events.end(); ++ev) {
        if (ev->event == event) {
            events.erase(ev);
            return;
        }
    }
}

void EventHandler::trigger(const std::string &event, const std::string &data,
                           bool keep)
{
    if (keep) {
        for (auto ev : events) {
            if (ev.event == event) {
                return;
            }
        }
    }

    events.push_back({
        event,
        data,
        keep
    });
}
