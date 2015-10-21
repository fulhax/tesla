#include "resource.hpp"

#include <stdio.h>
#include <string.h>

#include <algorithm>

// TextureResource
#include "resources/tga.hpp"
#include "resources/png.hpp"

struct findResource {
    explicit findResource(const char *s)
    {
        this->s = s;
    }

    bool operator()(Resource *r)
    {
        return !(strcmp(r->filename, s) == 0);
    }

    const char *s;
};

ResourceHandler::ResourceHandler()
{
}

ResourceHandler::~ResourceHandler()
{
    for(uint32_t i = 0; i < resources.size(); ++i) {
        lprintf(LOG_INFO, "Unloading ^g\"%s\"^0.", resources[i]->filename);
        delete resources[i];
    }
}

Resource *ResourceHandler::getByType(const char *ext)
{
    Resource *res = nullptr;

    if(!strcmp("tga", ext)) {
        res = new TGA_Resource;
    } else if(!strcmp("png", ext)) {
        res = new PNG_Resource;
    }

    return res;
}

void ResourceHandler::unload(const char *filename)
{
    auto find_res = std::find_if(
                        resources.begin(),
                        resources.end(),
                        findResource(filename));

    if(find_res == resources.end()) {
        lprintf(
            LOG_WARNING,
            "Unable to unload ^g\"%s\"^0, resource not loaded!",
            filename);

        return;
    }

    lprintf(LOG_INFO, "Unloading ^g\"%s\"^0.", filename);

    resources.erase(find_res);
}

Resource *ResourceHandler::get(const char *filename)
{
    auto find_res = std::find_if(
                        resources.begin(),
                        resources.end(),
                        findResource(filename));

    if(find_res != resources.end()) {
        return *find_res;
    }

    const char *ext = strrchr(filename, '.') + 1;

    if(ext) {
        Resource *res = getByType(ext);

        if(res != 0) {
            if(res->load(filename)) {
                res->filename = new char[strlen(filename) + 1];
                snprintf(res->filename, strlen(filename) + 1, "%s", filename);

                lprintf(LOG_INFO, "^g\"%s\"^0 loaded.", filename);

                resources.push_back(res);
                return res;
            }

            return nullptr;

        }
    } else {
        lprintf(LOG_WARNING, "Unable to get extension from ^g\"%s\"^0!", filename);
        return nullptr;
    }

    lprintf(LOG_ERROR, "^g\"%s\"^0 invalid resource type ^r%s^0!", filename, ext);

    return nullptr;
}
