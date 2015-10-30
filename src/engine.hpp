#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include "errorhandler.hpp"
#include "video.hpp"
#include "resource.hpp"
#include "script.hpp"
#include "audio.hpp"

class Engine
{
public:
    Engine();
    virtual ~Engine();

    int init();
    void update();
    static void shutdown();
    float getTime() const;

    bool running;
    int fps;

    Audio audio;
    Video video;
    Script script;
    ResourceHandler resources;

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
private:
    uint64_t oldtime;
    float time;
    int ref_count;
};

extern Engine engine;

#endif // ENGINE_HPP_
