#ifndef EVENTHANDLER_HPP_
#define EVENTHANDLER_HPP_

#include <string>
#include <vector>

#include "script.hpp"

class EventHandler : public ASClass<EventHandler>
{
    std::vector<std::string> events;
public:
    EventHandler();
    virtual ~EventHandler();

    int count();
    void trigger(const std::string &event);
    const std::string *poll();
};

#endif // EVENTHANDLER_HPP_
