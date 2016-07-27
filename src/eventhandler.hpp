#ifndef EVENTHANDLER_HPP_
#define EVENTHANDLER_HPP_

#include <string>
#include <vector>

#include "script.hpp"

class Event
{
public:
    Event() {}
    ~Event() {}
    Event(std::string event, std::string data, bool keep)
    {
        this->event = event;
        this->data = data;
        this->keep = keep;
    }
    explicit Event(Event* ev)
    {
        this->event = ev->event;
        this->data = ev->data;
        this->keep = ev->keep;
    }

    std::string event;
    std::string data;
    bool keep;
};

class EventHandler : public ASClass<EventHandler>
{
    std::vector<Event> events;
    std::vector<Event> keptevents;
public:
    EventHandler();
    virtual ~EventHandler();

    int count();
    void update();
    void untrigger(const std::string &event);
    void trigger(const std::string &event, const std::string &data = "1",
                 bool keep = false);
    const Event *poll();
};

#endif // EVENTHANDLER_HPP_
