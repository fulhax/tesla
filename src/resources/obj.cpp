#include "obj.hpp"
#include <fast_obj_loader.h>

#include <glm/glm.hpp>

OBJ_Resource::OBJ_Resource()
{
    data = 0;
    faces = 0;
}

OBJ_Resource::~OBJ_Resource()
{
    if(data) {
        delete data;
    }

    if(faces) {
        delete [] faces;
    }
}

int OBJ_Resource::load(const char *filename)
{
    if(data) {
        delete data;
        data = 0;
    }

    if(faces) {
        delete [] faces;
        faces = 0;
    }

    obj *tmpObj = loadObj(filename);

    if(tmpObj) {
        data = ObjMakeUniqueFullVerts(tmpObj);
        delete tmpObj;

        num_tris = data->numfaces;

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            data->numverts * sizeof(vec3),
            data->verts,
            GL_STATIC_DRAW);

        for(unsigned int i = 0; i < data->numverts; ++i) {
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

        faces = new obj_face[data->numfaces];

        for(unsigned int i = 0; i < data->numfaces; ++i) {
            faces[i].f[0] = data->faces[i].verts[0];
            faces[i].f[1] = data->faces[i].verts[1];
            faces[i].f[2] = data->faces[i].verts[2];
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            data->numfaces * sizeof(obj_face),
            faces,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // lprintf(LOG_INFO, "OBJ Loaded ^g\"%s\"^0", filename);

        return 1;
    }

    lprintf(LOG_ERROR, "Unable to load ^g\"%s\"^0", filename);

    return 0;
}
