#include "engine.hpp"

Engine engine;

Engine::Engine()
{
    running = true;
    oldtime = SDL_GetPerformanceCounter();
    time = 0;
    fps = 0;
    ref_count = 1;
}

Engine::~Engine()
{

}

float Engine::getTime() const
{
    return time;
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

    return 0;
}

void Engine::shutdown()
{
    config.saveConfig("settings.conf");
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
        //printf("FPS %d\n", fps);
        fps = 0;
        timer = time;
    } else {
        timer += time;
        fps += 1;
    }

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

    static int sound = -1;

    if(!audio.isPlaying(sound)) {
        sound = audio.play("sound/Example.ogg", glm::vec3(0, 0, 0));
    }

    audio.update(&video.camera);
    video.update();
    resources.update();
}
