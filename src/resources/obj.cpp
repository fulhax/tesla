#include "obj.hpp"

#include <fast_obj_loader.h>

OBJ_Resource::OBJ_Resource()
{
}

OBJ_Resource::~OBJ_Resource()
{
}

int OBJ_Resource::load(const char *filename)
{
    obj *m = loadObj(filename);

    if(m) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            m->numverts * sizeof(vec3),
            m->verts,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            m->numnormals * sizeof(vec3),
            m->normals,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            m->numuvs * sizeof(vec2),
            m->uvs,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            m->numfaces * sizeof(vec3),
            m->faces,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete m;

        lprintf(LOG_INFO, "OBJ Loaded ^g\"%s\"^0", filename);

        return 1;
    }

    lprintf(LOG_ERROR, "Unable to load ^g\"%s\"^0", filename);

    return 0;
}
