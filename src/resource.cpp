#include "resource.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include "engine.hpp"

// TextureResource
#include "resources/tga.hpp"
#include "resources/png.hpp"

// ModelResource
#include "resources/obj.hpp"
#include "resources/ogex.hpp"

// ShaderResource
#include "resources/glsl.hpp"

// ScriptResource
#include "resources/as.hpp"

// SoundResource
#include "resources/ogg.hpp"

// FontResource
#include "resources/freetype.hpp"

ResourceHandler::ResourceHandler()
{
    memset(datapath, 0, FILENAME_MAX);
    memset(enginepath, 0, FILENAME_MAX);
}

ResourceHandler::~ResourceHandler()
{
    lprintf(LOG_INFO, "Shutting down resourcehandler");

    for (auto res : resources) {
        lprintf(LOG_INFO, "Unloading ^g\"%s\"^0.", res.first.c_str());
        delete res.second;
    }

    resources.clear();
}

Resource *ResourceHandler::getByType(const char *ext)
{
    Resource *res = nullptr;
    char *post = const_cast<char *>(strrchr(ext, ':'));

    if (post) {
        post[0] = 0;
    }

    if (strcmp("tga", ext) == 0) {
        res = new TGA_Resource;
    } else if (strcmp("png", ext) == 0) {
        res = new PNG_Resource;
    } else if (strcmp("obj", ext) == 0) {
        res = new OBJ_Resource;
    } else if (strcmp("ogex", ext) == 0) {
        res = new OGEX_Resource;
    } else if (
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
    } else if (strcmp("as", ext) == 0) {
        res = new AS_Resource;
    } else if (strcmp("ogg", ext) == 0) {
        res = new OGG_Resource;
    } else if (
        strcmp("ttf", ext) == 0 ||
        strcmp("otf", ext) == 0
    ) {
        res = new FT_Resource;
    } else {
        lprintf(LOG_ERROR, "Unrecognized file format ^g%s^0", ext);
    }

    if (post) {
        post[0] = ':';
    }

    return res;
}

int ResourceHandler::init()
{
    snprintf(
        datapath,
        FILENAME_MAX,
        "%s",
        engine.config.getString(
            "resource.datapath",
            "./data"
        ).c_str());

    snprintf(
        enginepath,
        FILENAME_MAX,
        "%s",
        engine.config.getString(
            "resource.enginepath",
            "./engine"
        ).c_str());

    notify.watchDir(datapath);

    return 0;
}

void ResourceHandler::update()
{
    auto check = notify.checkForChanges();

    for (auto changes : check) {
        auto res = resources.lower_bound(changes.first);

        std::vector<std::string> files;

        while (res != resources.end()) {
            int partial = strncmp(res->first.c_str(),
                                  changes.first.c_str(),
                                  changes.first.length());

            if (partial == 0) {
                lprintf(LOG_INFO, "Unloading ^g\"%s\"^0.", res->first.c_str());
                delete res->second;
                files.push_back(res->first);
                resources.erase(res);
                res = resources.lower_bound(changes.first);
            } else {
                ++res;
            }
        }

        if (files.size() > 0) {
            for (auto file : files) {
                getResource(file.c_str());
            }
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

FontResource *ResourceHandler::getFont(const char *filename)
{
    return reinterpret_cast<FontResource *>(getResource(filename));
}

ShaderResource *ResourceHandler::getShader(
    Shader *parent,
    const char *filename)
{
    ShaderResource *s = reinterpret_cast<ShaderResource *>(getResource(filename));

    if (s) {
        s->parents.push_back(parent);
    }

    return s;
}

bool ResourceHandler::fileExists(const char *filename)
{
    char real_filename[FILENAME_MAX];
    snprintf(real_filename, FILENAME_MAX, "%s", filename);

    char *size = 0;
    strtok_r(real_filename, ":", &size);

    if (access(real_filename, F_OK) < 0) {
        return 0;
    }

    return 1;
}

Resource *ResourceHandler::getResource(const char *filename)
{
    auto res = resources.find(filename);

    if (res != resources.end()) {
        if (res->second->failed) {
            return nullptr;
        }

        return res->second;
    }

    char fullpath[FILENAME_MAX];
    snprintf(fullpath, FILENAME_MAX, "%s/%s", datapath, filename);

    if (!fileExists(fullpath)) {
        snprintf(fullpath, FILENAME_MAX, "%s/%s", enginepath, filename);

        if (!fileExists(fullpath)) {
            lprintf(LOG_WARNING, "File not found ^g\"%s\"^0!", filename);
            resources[filename] = new Resource;
            return nullptr;
        }
    }

    const char *ext = strrchr(fullpath, '.') + 1;

    if (ext) {
        Resource *res = getByType(ext);

        if (res) {
            if (res->load(fullpath)) {
                lprintf(LOG_INFO, "^g\"%s\"^0 loaded.", filename);
                res->failed = false;
                resources[filename] = res;
                return res;
            }

            lprintf(LOG_ERROR, "^g\"%s\"^0 failed to load.", filename);
            delete res;
        }

        resources[filename] = new Resource;
        return nullptr;
    }

    lprintf(LOG_ERROR, "^g\"%s\"^0 invalid resource type ^r%s^0!", filename, ext);

    return nullptr;
}

void ModelResource::updateBoundingBox(glm::vec3 vertex)
{
    for (int i = 0; i < 3; i++) {
        if (vertex[i] < bounding_box.min[i]) {
            bounding_box.min[i] = vertex[i];
        }

        if (vertex[i] > bounding_box.max[i]) {
            bounding_box.max[i] = vertex[i];
        }
    }
}
