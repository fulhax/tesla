#include "eventhandler.hpp"

#include <string.h>

#include <string>
#include <vector>

EventHandler::EventHandler()
{
    current = 0;
}

EventHandler::~EventHandler()
{
    events.clear();
}

int EventHandler::count()
{
    return events.size();
}

bool EventHandler::lastevent()
{
    bool last = (current == events.size());

    if (last) {
        current = 0;
    }

    return last;
}

const Event *EventHandler::poll()
{
    static Event out;
    auto ev = events.begin();

    if (current != events.size()) {
        out = ev[current];
        current += 1;

        return &out;
    }

    current = 0;
    return nullptr;
}

void EventHandler::update()
{
    current = 0;

    for (int i = events.size() - 1; i >= 0; i--) {
        if (!events[i].keep) {
            events.erase(events.begin() + i);
        }
    }

    // auto it = std::next(keptevents.begin(), keptevents.size());
    // std::move(keptevents.begin(), it, std::back_inserter(events));
    // keptevents.clear();
}

void EventHandler::untrigger(const std::string &event)
{
    for (int i = events.size() - 1; i >= 0; i--) {
        if (events[i].event == event) {
            events.erase(events.begin() + i);
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
