#include "resource.hpp"

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

int ResourceHandler::init()
{
    notify.watchDir(datapath);

    return 0;
}

void ResourceHandler::update()
{
    auto check = notify.checkForChanges();

    for(auto changes : check) {
        auto res = resources.find(changes.second);

        if(res != resources.end()) {
            lprintf(LOG_INFO, "Unloading ^g\"%s\"^0.", changes.first.c_str());
            delete res->second;
            resources.erase(res);
            getResource(changes.first.c_str());
        }
    }
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
