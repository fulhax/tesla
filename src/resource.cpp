#include "resource.hpp"

#include <sys/inotify.h>
#include <poll.h>
#include <dirent.h>

#include <algorithm>

// TextureResource
#include "resources/tga.hpp"
#include "resources/png.hpp"

// ModelResource
#include "resources/obj.hpp"

// ShaderResource
#include "resources/glsl.hpp"

// ScriptResource
#include "resources/as.hpp"

ResourceHandler::ResourceHandler()
{
    inotify = 0;
    snprintf(datapath, FILENAME_MAX, "./data");
}

ResourceHandler::~ResourceHandler()
{
    lprintf(LOG_INFO, "Shutting down resourcehandler");

    for(auto res : resources) {
        lprintf(LOG_INFO, "Unloading ^g\"%s\"^0.", res.first.c_str());
        delete res.second;
    }

    resources.clear();

    for(auto watch : watchers) {
        inotify_rm_watch(inotify, watch.first);
    }

    watchers.clear();
    close(inotify);
}

Resource *ResourceHandler::getByType(const char *ext)
{
    Resource *res = nullptr;

    if(strcmp("tga", ext) == 0) {
        res = new TGA_Resource;
    } else if(strcmp("png", ext) == 0) {
        res = new PNG_Resource;
    } else if(strcmp("obj", ext) == 0) {
        res = new OBJ_Resource;
    } else if(
        strcmp("vs", ext) == 0 ||
        strcmp("vert", ext) == 0 ||
        strcmp("gs", ext) == 0 ||
        strcmp("geom", ext) == 0 ||
        strcmp("tcs", ext) == 0 ||
        strcmp("tes", ext) == 0 ||
        strcmp("fs", ext) == 0 ||
        strcmp("frag", ext) == 0
    ) {
        res = new GLSL_Resource;
    } else if(strcmp("as", ext) == 0) {
        res = new AS_Resource;
    }

    return res;
}

void ResourceHandler::watchDir(const char *dirname)
{
    DIR *dh;
    dirent *entry;
    char fullpath[FILENAME_MAX];

    int watch = inotify_add_watch(inotify, dirname, IN_CLOSE_WRITE | IN_MOVED_TO);
    watchers[watch] = dirname;

    lprintf(LOG_INFO, "Watching %s for filechanges", dirname);

    if((dh = opendir(dirname)) == NULL) {
        lprintf(LOG_ERROR, "Could not open directory ^g\"%s\"^0", dirname);
        return;
    }

    while((entry = readdir(dh)) != NULL) {
        if(strncmp(entry->d_name, "..", 2) != 0 &&
            strncmp(entry->d_name, ".", 1) != 0) {

            if(entry->d_type == DT_DIR) {
                snprintf(
                    fullpath,
                    FILENAME_MAX,
                    "%s/%s",
                    dirname,
                    entry->d_name);

                watchDir(fullpath);
            }
        }
    }
}

int ResourceHandler::init()
{
    inotify = inotify_init1(IN_NONBLOCK);

    if(inotify < 0) {
        lprintf(LOG_WARNING, "Failed to start inotify!");
        return 1;
    }

    watchDir(datapath);

    return 0;
}

void ResourceHandler::update()
{
    int length = 0;
    int i = 0;
    inotify_event buffer[BUF_LEN] = {0};

    do {
        length = read(inotify, buffer, BUF_LEN);

        if(length < 0) {
            return;
        }

        const inotify_event &event =
            reinterpret_cast<const inotify_event &>(buffer[i]);

        if(event.mask & IN_CLOSE_WRITE || event.mask & IN_MOVED_TO) {
            char fullpath[FILENAME_MAX];
            auto watch = watchers[event.wd];

            snprintf(
                fullpath,
                FILENAME_MAX,
                "%s/%s",
                watch.c_str(),
                event.name);

            auto res = resources.find(fullpath);

            if(res != resources.end()) {
                lprintf(LOG_INFO, "Unloading ^g\"%s\"^0.", event.name);
                delete res->second;
                resources.erase(res);
                getResource(event.name);
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

ScriptResource *ResourceHandler::getScript(const char *filename)
{
    return reinterpret_cast<ScriptResource *>(getResource(filename));
}

ShaderResource *ResourceHandler::getShader(Shader *parent,
        const char *filename)
{
    ShaderResource *s = reinterpret_cast<ShaderResource *>(getResource(filename));

    if(s) {
        s->parents.push_back(parent);
    }

    return s;
}

Resource *ResourceHandler::getResource(const char *filename)
{
    char fullpath[FILENAME_MAX];
    snprintf(fullpath, FILENAME_MAX, "%s/%s", datapath, filename);

    auto res = resources.find(fullpath);

    if(res != resources.end()) {
        if(res->second->failed) {
            return 0;
        }

        return res->second;
    }


    const char *ext = strrchr(filename, '.') + 1;

    if(ext) {
        Resource *res = getByType(ext);

        if(res) {
            if(res->load(fullpath)) {
                lprintf(LOG_INFO, "^g\"%s\"^0 loaded.", filename);
                res->failed = false;
                resources[fullpath] = res;
                return res;
            }

            delete res;
        }

        resources[fullpath] = new Resource();
        return 0;
    }

    lprintf(LOG_ERROR, "^g\"%s\"^0 invalid resource type ^r%s^0!", filename, ext);

    return 0;
}
