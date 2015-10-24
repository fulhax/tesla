#ifndef RESOURCE_HPP_
#define RESOURCE_HPP_

#include <SDL2/SDL_opengl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <GL/glu.h>

#include <map>
#include <vector>
#include <string>

#include "errorhandler.hpp"
#include "shader.hpp"

#define EVENT_SIZE (sizeof(inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

class Resource
{
public:
    Resource() {}
    virtual ~Resource() {}
    virtual int load(const char *filename) = 0;
};

class TextureResource : public Resource
{
public:
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t type;

    TextureResource()
    {
        id     = -1;
        width  = 0;
        height = 0;
        bpp    = 0;
        type   = 0;

        glGenTextures(1, &id);
    }
    ~TextureResource()
    {
        glDeleteTextures(1, &id);
    }
};

class ShaderResource : public Resource
{
public:
    std::vector<Shader*> parents;
    uint32_t handle;

    ShaderResource()
    {
        handle = 0;
    }
    ~ShaderResource()
    {
        for(auto p : parents) {
            p->unload();
        }
        parents.clear();

        if(handle) {
            glDeleteShader(handle);
        }
    }
};

class ModelResource : public Resource
{
public:
    int num_tris;

    uint32_t vertex_buffer;
    uint32_t indices_buffer;
    uint32_t uv_buffer;
    uint32_t normals_buffer;

    ModelResource()
    {
        num_tris = 0;
        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &indices_buffer);
        glGenBuffers(1, &uv_buffer);
        glGenBuffers(1, &normals_buffer);
    }
    ~ModelResource()
    {
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteBuffers(1, &indices_buffer);
        glDeleteBuffers(1, &uv_buffer);
        glDeleteBuffers(1, &normals_buffer);
    }
};

class ResourceHandler
{
public:
    ResourceHandler();
    ~ResourceHandler();

    int init();
    void update();

    TextureResource *getTexture(const char *filename);
    ModelResource *getModel(const char *filename);
    ShaderResource *getShader(Shader* parent, const char *filename);
private:
    std::map<std::string, Resource *> resources;
    std::map<int, std::string> watchers;

    void watchDir(const char *dirname);
    Resource *getResource(const char *filename);
    Resource *getByType(const char *ext);

    char datapath[FILENAME_MAX];
    int inotify;
};

#endif // RESOURCE_HPP_
