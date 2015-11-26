#include "engine.hpp"

Engine engine;

Engine::Engine()
{
    running = true;
    oldtime = SDL_GetPerformanceCounter();
    time = 0;
    fps = 0;
}

Engine::~Engine()
{

}

float Engine::getTime() const
{
    return EngineTick;
}

int Engine::getFPS() const
{
    return fps;
}

int Engine::init()
{
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    if(ret != 0) {
        lprintf(LOG_ERROR, "Failed to init SDL");
        return 1;
    }

    config.readConfig("settings.conf");

    resources.init();
    audio.init();

    if(script.init() != 0) {
        running = false;
        return 1;
    }

    if(video.init() != 0) {
        running = false;
        return 1;
    }

    debugger.init();

    ScriptResource *s = engine.resources.getScript("main.as");

    if(s) {
        script.run(s, "void init()");
    } else {
        lprintf(LOG_WARNING, "Main script not found, shutting down.");
        running = false;
    }

    testentity[0].init("test1", "scripts/test.as");
    testentity[1].init("test2", "scripts/test2.as");
    testentity[2].init("test3", "scripts/test3.as");

    return 0;
}

void Engine::shutdown() const
{
    config.saveConfig("settings.conf");
    SDL_Quit();
}

void Engine::handleEvents()
{
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT: {
                if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    video.resize(event.window.data1, event.window.data2);
                }

                break;
            }


            case SDL_QUIT:
                running = false;
                break;
        }
    }
}

void Engine::update()
{
    ScriptResource *s = engine.resources.getScript("main.as");

    uint64_t ctime = SDL_GetPerformanceCounter();
    uint64_t freq = SDL_GetPerformanceFrequency();

    time = static_cast<double>(ctime - oldtime) /
           static_cast<double>(freq);

    oldtime = ctime;
    static float fpstimer = 0;
    static float mtime = time;

    if(fpstimer >= 1.f) {
        fps = countfps;
        countfps = 0;
        fpstimer = time;
    } else {
        fpstimer += time;
        countfps += 1;
    }

    video.update();
    resources.update();
    audio.update(&video.camera);

    mtime += time;

    script.run(s, "void draw()");

    while(mtime >= EngineTick) {
        mtime -= EngineTick;

        handleEvents();

        // static int sound = -1;
        //
        // if(!audio.isPlaying(sound)) {
        //     sound = audio.play("sound/Example.ogg", glm::vec3(0, 0, 0));
        // }

        testentity[0].update();
        testentity[1].update();
        testentity[2].update();

        script.run(s, "void update()");
    }

    video.swap();
}
