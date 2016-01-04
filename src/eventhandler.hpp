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
    Event(std::string event, std::string data)
    {
        this->event = event;
        this->data = data;
    }

    std::string event;
    std::string data;
};

class EventHandler : public ASClass<EventHandler>
{
    std::vector<Event> events;
public:
    EventHandler();
    virtual ~EventHandler();

    int count();
    void trigger(const std::string &event, const std::string &data = "1");
    const Event *poll();
};

#endif // EVENTHANDLER_HPP_
