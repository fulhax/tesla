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

// SoundResource
#include "resources/ogg.hpp"

ResourceHandler::ResourceHandler()
{
    snprintf(datapath, FILENAME_MAX, "./data");
    snprintf(enginepath, FILENAME_MAX, "./engine");
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
    } else if(strcmp("ogg", ext) == 0) {
        res = new OGG_Resource;
    } else {
        lprintf(LOG_ERROR, "Unrecognized file format ^g%s^0", ext);
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
        auto res = resources.find(changes.first);

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

SoundResource *ResourceHandler::getSound(const char *filename)
{
    return reinterpret_cast<SoundResource *>(getResource(filename));
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
    auto res = resources.find(filename);

    if(res != resources.end()) {
        if(res->second->failed) {
            return nullptr;
        }

        return res->second;
    }

    char fullpath[FILENAME_MAX];
    snprintf(fullpath, FILENAME_MAX, "%s/%s", datapath, filename);

    if(access(fullpath, F_OK) < 0) {
        snprintf(fullpath, FILENAME_MAX, "%s/%s", enginepath, filename);

        if(access(fullpath, F_OK) < 0) {
            lprintf(LOG_WARNING, "File not found ^g\"%s\"^0!", filename);
            return nullptr;
        }
    }

    const char *ext = strrchr(filename, '.') + 1;

    if(ext) {
        Resource *res = getByType(ext);

        if(res) {
            if(res->load(fullpath)) {
                lprintf(LOG_INFO, "^g\"%s\"^0 loaded.", fullpath);
                res->failed = false;
                resources[filename] = res;
                return res;
            }

            delete res;
        }

        resources[filename] = new Resource();
        return nullptr;
    }

    lprintf(LOG_ERROR, "^g\"%s\"^0 invalid resource type ^r%s^0!", filename, ext);

    return nullptr;
}

void ModelResource::updateBoundingBox(glm::vec3 vertex)
{
    for(int i = 0; i < 3; i++) {
        if(vertex[i] < bounding_box.min[i]) {
            bounding_box.min[i] = vertex[i];
        }

        if(vertex[i] > bounding_box.max[i]) {
            bounding_box.max[i] = vertex[i];
        }
    }
}
