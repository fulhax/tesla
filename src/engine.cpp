#include "engine.hpp"

#include <string>

Engine engine;

Engine::Engine()
{
    running = true;
    oldtime = SDL_GetPerformanceCounter();
    time = 0;
    fps = 0;
    countfps = 0;
    memset(msframe, 0, NUM_MSFRAMES * sizeof(float));
    currframe = 0;
    framebuffer = 0;

    for (int i = 0; i < MAX_MOUSEBUTTONS; i++) {
        mouse.button[i] = false;
    }
}

Engine::~Engine()
{

}

float Engine::getTick()
{
    return EngineTick;
}

float Engine::getMS() const
{
    float sum = 0;

    for (int i = 0; i < NUM_MSFRAMES; i++) {
        sum += msframe[i];
    }

    sum /= NUM_MSFRAMES;

    return (ceil(sum * 10.f) / 10.f);
}

int Engine::getFPS() const
{
    return fps;
}

void Engine::createEntityType(const std::string &name,
                              const std::string &script)
{
    entityTypes[name] = EntityType(name, script);
}

int Engine::spawnEntity(const std::string &name, const glm::vec3 &pos,
                        const glm::vec3 &rot)
{
    auto type = entityTypes.find(name);

    if (type == entityTypes.end()) {
        lprintf(LOG_WARNING, "Unknown entity type ^g\"%s\"^0", name.c_str());
        return -1;
    }

    Entity *e = new Entity(&type->second);
    e->spawn(pos, rot);

    entities.push_back(e);
    return entities.size() - 1;
}

Entity *Engine::getEntityById(const int &id)
{
    if (id < 0 || id > static_cast<int>(entities.size())) {
        return nullptr;
    }

    return entities[id];
}

int Engine::init()
{
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    if (ret != 0) {
        lprintf(LOG_ERROR, "Failed to init SDL");
        return 1;
    }

    config.readConfig("settings.conf");

    resources.init();
    audio.init();
    physics.init();

    if (script.init() != 0) {
        running = false;
        return 1;
    }

    if (video.init() != 0) {
        running = false;
        return 1;
    }

    debugger.init();

    ScriptResource *s = engine.resources.getScript("main.as");

    if (s) {
        script.run(s, "void init()");
    } else {
        lprintf(LOG_WARNING, "Main script not found, shutting down.");
        running = false;
    }

    physics.update();

    camera.attachCharacter(
        physics.createCharacter(
            glm::vec3(0, 10, 25),
            glm::vec2(1, 2),
            0.25f
        )
    );

    return 0;
}

void Engine::shutdown()
{
    for (auto e : entities) {
        delete e;
    }

    entities.clear();
    entityTypes.clear();

    config.saveConfig("settings.conf");
    SDL_Quit();
}

void Engine::handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT: {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    video.resize(event.window.data1, event.window.data2);
                }

                break;
            }

            case SDL_MOUSEMOTION: {
                if (event.motion.xrel != 0) {
                    events.trigger(
                        config.getString("input.mouse.x", "camera.yaw"),
                        std::to_string(event.motion.xrel)
                    );
                }

                if (event.motion.yrel != 0) {
                    events.trigger(
                        config.getString("input.mouse.y", "camera.pitch"),
                        std::to_string(event.motion.yrel)
                    );
                }

                mouse.x = event.motion.x;
                mouse.y = event.motion.y;
                mouse.rx = event.motion.xrel;
                mouse.ry = event.motion.yrel;

                break;
            }

            case SDL_KEYDOWN: {
                char key[64] = {0};
                snprintf(
                    key,
                    sizeof(key),
                    "input.keyboard.%c",
                    event.key.keysym.sym
                );

                events.trigger(
                    config.getString(key, "action.trigger"),
                    "1",
                    true
                );

                break;
            }

            case SDL_KEYUP: {
                char key[64] = {0};
                snprintf(
                    key,
                    sizeof(key),
                    "input.keyboard.%c",
                    event.key.keysym.sym
                );

                events.untrigger(config.getString(key, "action.trigger"));

                break;
            }

            case SDL_MOUSEBUTTONDOWN: {
                mouse.button[event.button.button] = true;
                break;
            }

            case SDL_MOUSEBUTTONUP: {
                mouse.button[event.button.button] = false;
                break;
            }

            case SDL_QUIT:
                running = false;
                break;
        }
    }

    for (int i = 0; i < MAX_MOUSEBUTTONS; i++) {
        if (mouse.button[i]) {
            char button[64] = {0};
            snprintf(button, sizeof(button), "input.mouse.button%d", i);
            events.trigger(config.getString(button, "action.trigger"));
        }
    }
}

void Engine::draw()
{
    ScriptResource *s = engine.resources.getScript("main.as");

    video.update(&camera);
    script.run(s, "void draw()");
}

int Engine::getEntitiesCount()
{
    return entities.size();
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

    if (fpstimer >= 1.f) {
        fps = countfps;
        countfps = 0;
        fpstimer = time;
    } else {
        fpstimer += time;
        countfps += 1;
    }

    resources.update();
    audio.update(&camera);

    mtime += time;

    draw();
    ui.update();

    static char foo[256] = {0};
    static char bar[256] = {0};
    static int ticks = 0;

    while (mtime >= EngineTick) {
        mtime -= EngineTick;

        handleEvents();

        // static int sound = -1;
        //
        // if(!audio.isPlaying(sound)) {
        //     sound = audio.play("sound/Example.ogg", glm::vec3(0, 0, 0));
        // }

        script.run(s, "void update()");

        for (auto e : entities) {
            e->update();
        }


        snprintf(
            foo,
            sizeof(foo),
            "vel: %f, %f, %f",
            camera.vel.x,
            camera.vel.y,
            camera.vel.z
        );

        camera.update(video.ProjMat, video.ViewMat);
        //test->updateAction(physics.dynamics_world, engine.getTick());

        physics.update();

        // while (!events.lastevent()) {
        //     auto ev = events.poll();
        //
        //     lprintf(
        //         LOG_WARNING,
        //         "Stray event ^r\"%s\"^0 found!",
        //         ev->event.c_str());
        // }
        snprintf(bar, sizeof(bar), "time: %f, ticks: %d", time, ticks);
        ticks = 0;
    }

    ticks++;

    ui.printDef(0, 250, foo);
    ui.printDef(0, 300, bar);

    events.update();
    video.swap();

    // btTransform trans;
    // trans = test->getGhostObject()->getWorldTransform();
    //
    // camera.pos = glm::vec3(
    //                  trans.getOrigin().x(),
    //                  trans.getOrigin().y(),
    //                  trans.getOrigin().z()
    //              );

    ctime = SDL_GetPerformanceCounter();
    freq = SDL_GetPerformanceFrequency();

    msframe[currframe] = static_cast<double>((ctime - oldtime) * 1000) /
                         static_cast<double>(freq);

    currframe = (currframe + 1) % NUM_MSFRAMES;
}
