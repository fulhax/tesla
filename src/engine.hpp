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

class Engine
{
public:
    Engine();
    virtual ~Engine();

    int init();
    void update();
    void shutdown() const;
    float getTime() const;

    bool running;
    int fps;

    Ui ui;
    Audio audio;
    Video video;
    Script script;
    ResourceHandler resources;
    Debugger debugger;
    Config config;

    // For AngelScript {
    void addRef()
    {
        ref_count++;
    }
    void releaseRef()
    {
        if(--ref_count == 0) {
            delete this;
        }
    }
    // }

    Entity testentity[3];
private:
    void handleEvents();

    uint64_t oldtime;
    float time;
    int ref_count;
};

extern Engine engine;

#endif // ENGINE_HPP_
