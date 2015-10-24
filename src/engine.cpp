#include "engine.hpp"

Engine engine;

Engine::Engine()
{
    running = true;
    oldtime = 0;
    time = 0;
    fps = 0;
}

Engine::~Engine()
{

}

int Engine::init()
{
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    if(ret != 0) {
        lprintf(LOG_ERROR, "Failed to init SDL");
        return 1;
    }

    resources.init();

    if(video.init()) {
        running = false;
        return 1;
    }

    if(script.init()) {
        running = false;
        return 1;
    }

    return 0;
}

void Engine::shutdown()
{
    SDL_Quit();
}

void Engine::update()
{
    uint64_t ctime = SDL_GetPerformanceCounter();
    uint64_t freq = SDL_GetPerformanceFrequency();

    time = static_cast<double>(ctime - oldtime) / static_cast<double>(freq);
    oldtime = ctime;

    static float timer = 0;

    if(timer >= 1.f) {
        printf("FPS %d\n", fps);
        fps = 0;
        timer = time;
    } else {
        timer += time;
        fps += 1;
    }

    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                running = false;
                break;
        }
    }

    video.update();
    resources.update();
}
