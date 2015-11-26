#ifndef SHAPES_HPP_
#define SHAPES_HPP_

#include <glm/glm.hpp>

class Shape
{

    struct mpi {
        mpi()
        {
            key = 0;
            tri = 0;
            next = 0;
        }
        int key;
        int tri;
        mpi *next;
    };

    mpi *mpi_cache;
    int getMiddlePoint(int p1, int p2);
    void mpiCacheAdd(int key, int tri);
    int mpiCacheGet(int key) const;
    void mpiCacheClear() const;
public:
    struct tri {
        unsigned int i[3];
        tri(unsigned int a, unsigned int b, unsigned int c)
        {
            i[0] = a;
            i[1] = b;
            i[2] = c;
        }
        tri() 
        {
            i[0] = 0;
            i[1] = 0;
            i[2] = 0;
        }
    };

    tri *tris;

    glm::vec3 *normals;
    glm::vec2 *uvs;
    glm::vec3 *verts;

    unsigned int vertex_buffer;
    unsigned int indices_buffer;
    unsigned int uv_buffer;
    unsigned int normals_buffer;

    int num_vert;
    int num_tris;

    void subdivide(int recursion_level);
    virtual void generate() = 0;

    Shape();
    virtual ~Shape();
};

class Plane : public Shape
{
public:
    void generate()
    {
        generate(1, 1);
    };
    void generate(int x, int y);
    static void subdivide(int recursion_level);

    Plane()
    {
        num_tris = 2;
        num_vert = 4;

        tris = new tri[num_tris];
        verts = new glm::vec3[num_vert];
        normals = new glm::vec3[num_vert];
        uvs = new glm::vec2[num_vert];
    };
};

class Cube : public Shape
{
public:
    float size;
    void generate()
    {
        generate(true, true, true, true, true, true);
    };
    void generate(bool xn, bool xp, bool yn, bool yp, bool zn, bool zp);
    static void subdivide(int recursion_level);

    Cube()
    {
        size = 1.0f;
        num_tris = 12;
        num_vert = 24;

        tris = new tri[num_tris];
        verts = new glm::vec3[num_vert];
        normals = new glm::vec3[num_vert];
        uvs = new glm::vec2[num_vert];
    };
};

class IsoSphere : public Shape
{
public:
    void generate();

    IsoSphere()
    {
        num_vert = 0;
        num_tris = 0;
    };
};

#endif // SHAPES_HPP_
