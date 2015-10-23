#include "resource.hpp"

#include <sys/inotify.h>
#include <poll.h>

#include <algorithm>

// TextureResource
#include "resources/tga.hpp"
#include "resources/png.hpp"

// ModelResource
#include "resources/obj.hpp"

ResourceHandler::ResourceHandler()
{
    watcher = 0;
    inotify = 0;
    snprintf(datapath, FILENAME_MAX, "./data");
}

ResourceHandler::~ResourceHandler()
{
    for(auto res : resources) {
        lprintf(LOG_INFO, "Unloading ^g\"%s\"^0.", res.first);
        delete res.second;
    }

    resources.clear();
    inotify_rm_watch(inotify, watcher);
    close(inotify);
}

Resource *ResourceHandler::getByType(const char *ext)
{
    Resource *res = nullptr;

    if(!strcmp("tga", ext)) {
        res = new TGA_Resource;
    } else if(!strcmp("png", ext)) {
        res = new PNG_Resource;
    } else if(!strcmp("obj", ext)) {
        res = new OBJ_Resource;
    }

    return res;
}

int ResourceHandler::init()
{
    inotify = inotify_init1(IN_NONBLOCK);

    if(inotify < 0) {
        lprintf(LOG_WARNING, "Failed to start inotify!");
        return 1;
    }

    watcher = inotify_add_watch(inotify, datapath, IN_CLOSE_WRITE);
    lprintf(LOG_INFO, "Watching %s for filechanges", datapath);

    return 0;
}

void ResourceHandler::update()
{
    int length = 0;
    int i = 0;
    char buffer[BUF_LEN] = {0};

    do {
        length = read(inotify, buffer, BUF_LEN);

        if(length < 0) {
            return;
        }

        const inotify_event &event =
            reinterpret_cast<const inotify_event &>(buffer[i]);

        if(event.mask & IN_CLOSE_WRITE) {
            auto res = resources.find(event.name);

            if(res != resources.end()) {
                lprintf(LOG_INFO, "Unloading ^g\"%s\"^0.", event.name);
                delete res->second;
                resources.erase(res);
            } else {
                for(auto r : resources) {
                    printf("Resource: %s\n", r.first);
                }
            }
        }

        i += EVENT_SIZE + event.len;
    } while(i < length);
}

ModelResource *ResourceHandler::getModel(const char *filename)
{
    return reinterpret_cast<ModelResource *>(getResource(filename));
}

TextureResource *ResourceHandler::getTexture(const char *filename)
{
    return reinterpret_cast<TextureResource *>(getResource(filename));
}

Resource *ResourceHandler::getResource(const char *filename)
{
    char datafile[FILENAME_MAX];
    snprintf(datafile, FILENAME_MAX, "%s/%s", datapath, filename);

    auto res = resources.find(filename);

    if(res != resources.end()) {
        return res->second;
    }

    const char *ext = strrchr(filename, '.') + 1;

    if(ext) {
        Resource *res = getByType(ext);

        if(res != 0) {
            if(res->load(datafile)) {
                lprintf(LOG_INFO, "^g\"%s\"^0 loaded.", filename);
                resources[filename] = res;
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
