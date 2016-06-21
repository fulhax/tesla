#include "debugger.hpp"

Debugger::Debugger()
{
    model = 0;
    texture = 0;
}

Debugger::~Debugger()
{
    if (model) {
        delete model;
    }

    if (texture) {
        delete texture;
    }
}

int Debugger::init()
{
    model = new ModelResource();

    shader.attach("shaders/debug.vert");
    shader.attach("shaders/debug.frag");

    debugCube.generate();

    model->has_uv_buffer = true;

    for (int i = 0; i < debugCube.num_vert; ++i) {
        model->updateBoundingBox(debugCube.verts[i]);
    }

    model->verts = new float[debugCube.num_vert * 3];
    unsigned int j = 0;

    for (int i = 0; i < debugCube.num_vert * 3; i += 3) {
        model->verts[i] =  debugCube.verts[j].x;
        model->verts[i + 1] = debugCube.verts[j].y;
        model->verts[i + 2] = debugCube.verts[j].z;

        j++;
    }

    model->indices = new uint32_t[debugCube.num_tris * 3];
    j = 0;

    for (int i = 0; i < debugCube.num_tris * 3; i += 3) {
        model->indices[i] =  debugCube.tris[j].i[0];
        model->indices[i + 1] = debugCube.tris[j].i[1];
        model->indices[i + 2] = debugCube.tris[j].i[2];

        j++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, model->uv_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        debugCube.num_vert * 12,
        debugCube.uvs,
        GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, model->vertex_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        debugCube.num_vert * 12,
        debugCube.verts,
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indices_buffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        debugCube.num_tris * 12,
        debugCube.tris,
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    model->num_tris = debugCube.num_tris;

    texture = new TextureResource();

    texture->type = GL_RGBA;
    texture->width = 1;
    texture->height = 1;
    texture->bpp = 32;

    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_NEAREST);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR);

    uint32_t debugData = 0xff0000ff;
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        texture->type,
        texture->width,
        texture->height,
        0,
        texture->type,
        GL_UNSIGNED_BYTE,
        &debugData);

    return 1;
}

Shader *Debugger::useDebugShader()
{
    if (shader.use()) {
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);

        shader.setUniform("in_Color", glm::vec3(1.0, 0.0, 0.0));

        return &shader;
    }

    return nullptr;
}

ModelResource *Debugger::useDebugModel()
{
    return model;
}

TextureResource *Debugger::useDebugTexture()
{
    return texture;
}
