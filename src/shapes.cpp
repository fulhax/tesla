#include "shapes.hpp"

#include <memory.h>
#include "opengl.hpp"

#include "errorhandler.hpp"

Shape::Shape()
{
    vertex_buffer = -1;
    indices_buffer = -1;
    uv_buffer = -1;
    normals_buffer = -1;

    verts = 0;
    tris = 0;
    normals = 0;
    uvs = 0;

    num_vert = 0;
    num_tris = 0;
    mpi_cache = 0;
}

Shape::~Shape()
{
    if(verts) {
        delete [] verts;
    }

    if(tris) {
        delete [] tris;
    }

    if(uvs) {
        delete [] uvs;
    }

    if(normals) {
        delete [] normals;
    }

    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &indices_buffer);
    glDeleteBuffers(1, &uv_buffer);
    glDeleteBuffers(1, &normals_buffer);
}

void Shape::mpiCacheAdd(int key, int tri)
{
    if(mpi_cache) {
        mpi *tmp = new mpi;
        tmp->tri = tri;
        tmp->key = key;

        mpi *curr = mpi_cache;

        while(curr->next) {
            curr = curr->next;
        }

        curr->next = tmp;
    } else {
        mpi_cache = new mpi;
        mpi_cache->tri = tri;
        mpi_cache->key = key;
    }
}

int Shape::mpiCacheGet(int key) const
{
    if(mpi_cache) {
        for(mpi *curr = mpi_cache; curr; curr = curr->next) {
            if(curr->key == key) {
                return curr->tri;
            }
        }
    }

    return 0;
}

void Shape::mpiCacheClear() const
{
    mpi *curr = mpi_cache;

    while(curr) {
        mpi *tmp = curr;
        curr = curr->next;
        delete tmp;
    }
}

int Shape::getMiddlePoint(int p1, int p2)
{
    int tri = 0;
    int smallertri = (p1 < p2) ? p1 : p2;
    int greatertri = (p1 < p2) ? p2 : p1;
    int key = (smallertri << 16) + greatertri;

    if((tri = mpiCacheGet(key))) {
        return tri;
    }

    glm::vec3 *v1 = &verts[p1];
    glm::vec3 *v2 = &verts[p2];

    tri = num_vert;
    num_vert++;

    verts[tri] = glm::normalize(glm::vec3(
                                    (v1->x + v2->x) / 2.0f,
                                    (v1->y + v2->y) / 2.0f,
                                    (v1->z + v2->z) / 2.0f));
    mpiCacheAdd(key, tri);

    return tri;
}

void Shape::subdivide(int recursion_level)
{
    if(num_vert == 0 || num_tris == 0) {
        generate();
    }

    int newnum_vert = num_vert;

    for(int i = 0; i < recursion_level; i++) {
        newnum_vert += 30 * pow(4.0f, i);
    }

    glm::vec3 *newverts = new glm::vec3[newnum_vert + 1];
    memcpy(newverts, verts, num_vert * sizeof(glm::vec3));
    delete [] verts;
    verts = newverts;

    for(int i = 1; i <= recursion_level; i++) {
        int newnum_tris = num_tris * 4;
        tri *newtris = new tri[newnum_tris + 1];

        for(int t = 0, nt = 0; t < num_tris; t++) {
            int a = getMiddlePoint(tris[t].i[0], tris[t].i[1]);
            int b = getMiddlePoint(tris[t].i[1], tris[t].i[2]);
            int c = getMiddlePoint(tris[t].i[2], tris[t].i[0]);

            newtris[nt++] = tri(tris[t].i[0], a, c);
            newtris[nt++] = tri(tris[t].i[1], b, a);
            newtris[nt++] = tri(tris[t].i[2], c, b);
            newtris[nt++] = tri(a, b, c);
        }

        delete [] tris;
        tris = newtris;
        num_tris = newnum_tris;
    }

    mpiCacheClear();

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, num_vert * 12, verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_tris * 12, tris, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void IsoSphere::generate()
{
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &indices_buffer);

    num_vert = 12;
    verts = new glm::vec3[num_vert];

    float t = (1.0f + sqrtf(5.0f)) / 2.0f;

    verts[0] = glm::normalize(glm::vec3(-1,  t, 0));
    verts[1] = glm::normalize(glm::vec3(1,  t, 0));
    verts[2] = glm::normalize(glm::vec3(-1, -t, 0));
    verts[3] = glm::normalize(glm::vec3(1, -t, 0));

    verts[4] = glm::normalize(glm::vec3(0, -1,  t));
    verts[5] = glm::normalize(glm::vec3(0,  1,  t));
    verts[6] = glm::normalize(glm::vec3(0, -1, -t));
    verts[7] = glm::normalize(glm::vec3(0,  1, -t));

    verts[8]  = glm::normalize(glm::vec3(t, 0, -1));
    verts[9]  = glm::normalize(glm::vec3(t, 0,  1));
    verts[10] = glm::normalize(glm::vec3(-t, 0, -1));
    verts[11] = glm::normalize(glm::vec3(-t, 0,  1));

    num_tris = 20;
    tris = new tri[num_tris];

    tris[0] = tri(0, 11,  5);
    tris[1] = tri(0,  5,  1);
    tris[2] = tri(0,  1,  7);
    tris[3] = tri(0,  7, 10);
    tris[4] = tri(0, 10, 11);

    tris[5] = tri(1,  5, 9);
    tris[6] = tri(5, 11, 4);
    tris[7] = tri(11, 10, 2);
    tris[8] = tri(10,  7, 6);
    tris[9] = tri(7,  1, 8);

    tris[10] = tri(3, 9, 4);
    tris[11] = tri(3, 4, 2);
    tris[12] = tri(3, 2, 6);
    tris[13] = tri(3, 6, 8);
    tris[14] = tri(3, 8, 9);

    tris[15] = tri(4, 9,  5);
    tris[16] = tri(2, 4, 11);
    tris[17] = tri(6, 2, 10);
    tris[18] = tri(8, 6,  7);
    tris[19] = tri(9, 8,  1);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, num_vert * 12, verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_tris * 12, tris, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Plane::subdivide(int recursion_level)
{
    lprintf(LOG_WARNING, "Planes cannot be subdivided\n");
}

void Plane::generate(int x, int y)
{
    if(vertex_buffer != 0) {
        glGenBuffers(1, &vertex_buffer);
    }

    if(indices_buffer != 0) {
        glGenBuffers(1, &indices_buffer);
    }

    if(uv_buffer != 0) {
        glGenBuffers(1, &uv_buffer);
    }

    uvs[0] = glm::vec2(1, 0);
    uvs[1] = glm::vec2(1, 1);
    uvs[2] = glm::vec2(0, 1);
    uvs[3] = glm::vec2(0, 0);

    verts[0] = glm::vec3(x, -y, 0);
    verts[1] = glm::vec3(x, y, 0);
    verts[2] = glm::vec3(-x, y, 0);
    verts[3] = glm::vec3(-x, -y, 0);

    tris[0] = tri(0, 1, 2);
    tris[1] = tri(0, 2, 3);

    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, num_vert * 12, uvs, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, num_vert * 12, verts, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_tris * 12, tris, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::subdivide(int recursion_level)
{
    lprintf(LOG_WARNING, "Cubes cannot be subdivided\n");
}

void Cube::generate(bool xn, bool xp, bool yn, bool yp, bool zn, bool zp)
{
    float hsize = size * 0.5f;

    int rvert = 0;
    int rtris = 0;

    //// Front
    if(zn) {
        uvs[rvert] = glm::vec2(0, 0);
        uvs[rvert + 1] = glm::vec2(0, 1);
        uvs[rvert + 2] = glm::vec2(1, 1);
        uvs[rvert + 3] = glm::vec2(1, 0);

        normals[rvert] = glm::vec3(0, 0, -1);
        normals[rvert + 1] = glm::vec3(0, 0, -1);
        normals[rvert + 2] = glm::vec3(0, 0, -1);
        normals[rvert + 3] = glm::vec3(0, 0, -1);

        verts[rvert] = glm::vec3(-hsize, hsize, -hsize);
        verts[rvert + 1] = glm::vec3(hsize, hsize, -hsize);
        verts[rvert + 2] = glm::vec3(hsize, -hsize, -hsize);
        verts[rvert + 3] = glm::vec3(-hsize, -hsize, -hsize);

        tris[rtris] = tri(rvert, rvert + 1, rvert + 2);
        tris[rtris + 1] = tri(rvert, rvert + 2, rvert + 3);

        rvert += 4;
        rtris += 2;
    }

    // Back
    if(zp) {
        uvs[rvert] = glm::vec2(0, 0);
        uvs[rvert + 1] = glm::vec2(0, 1);
        uvs[rvert + 2] = glm::vec2(1, 1);
        uvs[rvert + 3] = glm::vec2(1, 0);

        normals[rvert] = glm::vec3(0, 0, 1);
        normals[rvert + 1] = glm::vec3(0, 0, 1);
        normals[rvert + 2] = glm::vec3(0, 0, 1);
        normals[rvert + 3] = glm::vec3(0, 0, 1);

        verts[rvert] = glm::vec3(hsize, -hsize, hsize);
        verts[rvert + 1] = glm::vec3(hsize, hsize, hsize);
        verts[rvert + 2] = glm::vec3(-hsize, hsize, hsize);
        verts[rvert + 3] = glm::vec3(-hsize, -hsize, hsize);

        tris[rtris] = tri(rvert, rvert + 1, rvert + 2);
        tris[rtris + 1] = tri(rvert, rvert + 2, rvert + 3);

        rvert += 4;
        rtris += 2;
    }

    // Right
    if(xp) {
        uvs[rvert] = glm::vec2(0, 0);
        uvs[rvert + 1] = glm::vec2(0, 1);
        uvs[rvert + 2] = glm::vec2(1, 1);
        uvs[rvert + 3] = glm::vec2(1, 0);

        normals[rvert] = glm::vec3(1, 0, 0);
        normals[rvert + 1] = glm::vec3(1, 0, 0);
        normals[rvert + 2] = glm::vec3(1, 0, 0);
        normals[rvert + 3] = glm::vec3(1, 0, 0);

        verts[rvert] = glm::vec3(hsize, -hsize, -hsize);
        verts[rvert + 1] = glm::vec3(hsize, hsize, -hsize);
        verts[rvert + 2] = glm::vec3(hsize, hsize, hsize);
        verts[rvert + 3] = glm::vec3(hsize, -hsize, hsize);

        tris[rtris] = tri(rvert, rvert + 1, rvert + 2);
        tris[rtris + 1] = tri(rvert, rvert + 2, rvert + 3);

        rvert += 4;
        rtris += 2;
    }

    // Left
    if(xn) {
        uvs[rvert] = glm::vec2(0, 0);
        uvs[rvert + 1] = glm::vec2(0, 1);
        uvs[rvert + 2] = glm::vec2(1, 1);
        uvs[rvert + 3] = glm::vec2(1, 0);

        normals[rvert] = glm::vec3(-1, 0, 0);
        normals[rvert + 1] = glm::vec3(-1, 0, 0);
        normals[rvert + 2] = glm::vec3(-1, 0, 0);
        normals[rvert + 3] = glm::vec3(-1, 0, 0);

        verts[rvert] = glm::vec3(-hsize, -hsize, hsize);
        verts[rvert + 1] = glm::vec3(-hsize, hsize, hsize);
        verts[rvert + 2] = glm::vec3(-hsize, hsize, -hsize);
        verts[rvert + 3] = glm::vec3(-hsize, -hsize, -hsize);

        tris[rtris] = tri(rvert, rvert + 1, rvert + 2);
        tris[rtris + 1] = tri(rvert, rvert + 2, rvert + 3);

        rvert += 4;
        rtris += 2;
    }

    // Top
    if(yp) {
        uvs[rvert] = glm::vec2(0, 0);
        uvs[rvert + 1] = glm::vec2(0, 1);
        uvs[rvert + 2] = glm::vec2(1, 1);
        uvs[rvert + 3] = glm::vec2(1, 0);

        normals[rvert] = glm::vec3(0, 1, 0);
        normals[rvert + 1] = glm::vec3(0, 1, 0);
        normals[rvert + 2] = glm::vec3(0, 1, 0);
        normals[rvert + 3] = glm::vec3(0, 1, 0);

        verts[rvert] = glm::vec3(hsize, hsize, hsize);
        verts[rvert + 1] = glm::vec3(hsize, hsize, -hsize);
        verts[rvert + 2] = glm::vec3(-hsize, hsize, -hsize);
        verts[rvert + 3] = glm::vec3(-hsize, hsize, hsize);

        tris[rtris] = tri(rvert, rvert + 1, rvert + 2);
        tris[rtris + 1] = tri(rvert, rvert + 2, rvert + 3);

        rvert += 4;
        rtris += 2;
    }

    // Bottom
    if(yn) {
        uvs[rvert] = glm::vec2(0, 0);
        uvs[rvert + 1] = glm::vec2(0, 1);
        uvs[rvert + 2] = glm::vec2(1, 1);
        uvs[rvert + 3] = glm::vec2(1, 0);

        normals[rvert] = glm::vec3(0, -1, 0);
        normals[rvert + 1] = glm::vec3(0, -1, 0);
        normals[rvert + 2] = glm::vec3(0, -1, 0);
        normals[rvert + 3] = glm::vec3(0, -1, 0);

        verts[rvert] = glm::vec3(hsize, -hsize, -hsize);
        verts[rvert + 1] = glm::vec3(hsize, -hsize, hsize);
        verts[rvert + 2] = glm::vec3(-hsize, -hsize, hsize);
        verts[rvert + 3] = glm::vec3(-hsize, -hsize, -hsize);

        tris[rtris] = tri(rvert, rvert + 1, rvert + 2);
        tris[rtris + 1] = tri(rvert, rvert + 2, rvert + 3);

        rvert += 4;
        rtris += 2;
    }

    num_tris = rtris;
    num_vert = rvert;
}
