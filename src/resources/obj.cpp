#include "obj.hpp"
#include <fast_obj_loader.h>

#include <glm/glm.hpp>

OBJ_Resource::OBJ_Resource()
{
    data = 0;
}

OBJ_Resource::~OBJ_Resource()
{
    if (data) {
        delete data;
    }
}

int OBJ_Resource::load(const char *filename)
{
    if (data) {
        delete data;
        data = 0;
    }

    obj *tmpObj = loadObj(filename);

    if (tmpObj) {

        has_uv_buffer = tmpObj->uvs != nullptr;
        has_normals_buffer = tmpObj->normals != nullptr;

        data = ObjMakeUniqueFullVerts(tmpObj);
        delete tmpObj;

        verts = new float[data->numverts * 3];
        unsigned int j = 0;

        for (unsigned int i = 0; i < data->numverts * 3; i += 3) {
            verts[i] = data->verts[j].x;
            verts[i + 1] = data->verts[j].y;
            verts[i + 2] = data->verts[j].z;

            j++;
        }

        num_verts = data->numverts;
        num_tris = data->numfaces;

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            data->numverts * sizeof(vec3),
            data->verts,
            GL_STATIC_DRAW);

        for (unsigned int i = 0; i < data->numverts; ++i) {
            updateBoundingBox(glm::vec3(
                                  data->verts[i].x,
                                  data->verts[i].y,
                                  data->verts[i].z
                              ));
        }

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

        indices = new uint32_t[data->numfaces * 3];
        j = 0;

        for (unsigned int i = 0; i < data->numfaces * 3; i += 3) {
            indices[i] = data->faces[j].verts[0];
            indices[i + 1] = data->faces[j].verts[1];
            indices[i + 2] = data->faces[j].verts[2];

            j++;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            (data->numfaces * 3) * sizeof(uint32_t),
            indices,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // lprintf(LOG_INFO, "OBJ Loaded ^g\"%s\"^0", filename);

        return 1;
    }

    lprintf(LOG_ERROR, "Unable to load ^g\"%s\"^0", filename);

    return 0;
}
