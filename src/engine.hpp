#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include <vector>
#include <string>
#include <map>

#include "errorhandler.hpp"
#include "eventhandler.hpp"
#include "video.hpp"
#include "resource.hpp"
#include "script.hpp"
#include "audio.hpp"
#include "debugger.hpp"
#include "config.hpp"
#include "entity.hpp"
#include "physics.hpp"

#define EngineTick 1.f/30.f
#define NUM_MSFRAMES 100
#define MAX_MOUSEBUTTONS 32

class Engine
{
public:
    Engine();
    virtual ~Engine();

    int init();
    void update();
    void shutdown();
    float getTick();
    void draw();
    int getFPS() const;
    float getMS() const;

    void createEntityType(const std::string &name,
                          const std::string &script);

    int spawnEntity(const std::string &name, const glm::vec3 &pos,
                    const glm::vec3 &rot);
    Entity *getEntityById(const int &id);
    int getEntitiesCount();

    bool running;

    Ui ui;
    Audio audio;
    Video video;
    Script script;
    ResourceHandler resources;
    Debugger debugger;
    Config config;
    Physics physics;
    EventHandler events;
    Camera camera;

    struct {
        int x;
        int y;
        float rx;
        float ry;
        bool button[MAX_MOUSEBUTTONS];
    } mouse;

    float time;
    uint64_t oldtime;
private:
    std::vector<Entity *> entities;
    std::map<std::string, EntityType> entityTypes;
    void handleEvents();

    uint32_t framebuffer;
    float msframe[NUM_MSFRAMES];
    int currframe;
    int countfps;
    int fps;
};

extern Engine engine;

#endif // ENGINE_HPP_
