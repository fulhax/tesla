#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include "errorhandler.hpp"
#include "video.hpp"
#include "resource.hpp"
#include "script.hpp"
#include "audio.hpp"
#include "debugger.hpp"
#include "config.hpp"

#define EngineTick 0.032f // 1 tick = 32ms

class Engine : public ASClass
{
public:
    Engine();
    virtual ~Engine();

    int init();
    void update();
    void shutdown() const;
    float getTime();
    int getFPS() const;

    bool running;

    Ui ui;
    Audio audio;
    Video video;
    Script script;
    ResourceHandler resources;
    Debugger debugger;
    Config config;

    Entity testentity[3];
private:
    void handleEvents();

    uint64_t oldtime;
    float time;
    int countfps;
    int fps;
};

extern Engine engine;

#endif // ENGINE_HPP_
