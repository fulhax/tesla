#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include <GLFW/glfw3.h>

#include "errorhandler.hpp"
#include "video.hpp"
#include "resource.hpp"
#include "script.hpp"

class Engine
{
public:
    Engine();
    virtual ~Engine();

    int init();
    void shutdown();
    void update();

    bool running;
    float time;
    int fps;

    Video video;
    Script script;
    ResourceHandler resources;
private:
    uint64_t oldtime;
};

extern Engine engine;

#endif // ENGINE_HPP_
