#include "obj.hpp"

OBJ_Resource::OBJ_Resource()
{
    data = 0;
}

OBJ_Resource::~OBJ_Resource()
{
    if(data) {
        delete data;
    }
}

int OBJ_Resource::load(const char *filename)
{
    if(data) {
        delete data;
    }

    data = loadObj(filename);

    if(data) {
        num_tris = data->numfaces;

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            data->numverts * sizeof(vec3),
            data->verts,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            data->numnormals * sizeof(vec3),
            data->normals,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            data->numuvs * sizeof(vec2),
            data->uvs,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            data->numfaces * sizeof(vec3),
            data->faces,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        lprintf(LOG_INFO, "OBJ Loaded ^g\"%s\"^0", filename);

        return 1;
    }

    lprintf(LOG_ERROR, "Unable to load ^g\"%s\"^0", filename);

    return 0;
}
