#ifndef RESOURCE_HPP_
#define RESOURCE_HPP_

#include <SDL2/SDL_opengl.h>
#include <unistd.h>
#include <GL/glu.h>

#include <vector>

#include "errorhandler.hpp"

class Resource
{
public:
    Resource()
    {
        filename = 0;
    }
    virtual ~Resource() {}
    virtual int load(const char *filename) = 0;

    char *filename;
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

typedef std::vector<Resource *> resContainer;

class ResourceHandler
{
public:
    ResourceHandler();
    ~ResourceHandler();

    Resource *get(const char *filename);
    void unload(const char *filename);
private:
    Resource *getByType(const char *ext);

    resContainer resources;
};

#endif // RESOURCE_HPP_
