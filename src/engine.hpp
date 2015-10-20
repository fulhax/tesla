#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include <GLFW/glfw3.h>

#include "video.hpp"
#include "errorhandler.hpp"

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
private:
    Video video;

    uint64_t oldtime;
};

static Engine engine;

#endif // ENGINE_HPP_
