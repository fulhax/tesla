#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <string>

#include "resource.hpp"
#include "shader.hpp"
#include "script.hpp"

#define MAX_NAMELEN 64

class Entity
{
    char model[FILENAME_MAX];
    char texture[FILENAME_MAX];
    char script[FILENAME_MAX];
    char name[MAX_NAMELEN];

    glm::vec3 pos;
    int ref_count;
public:
    Entity();
    virtual ~Entity();

    void init(const char *name, const char *script);
    void draw(const glm::mat4 &Projection, const glm::mat4 &View);

    Shader shader;

    void setPos(float x, float y, float z);
    void setModel(const std::string &in);
    void setTexture(const std::string &in);

    // For AngelScript {
    void addRef()
    {
        ref_count++;
    }
    void releaseRef()
    {
        if(--ref_count == 0) {
            delete this;
        }
    }
    static Entity *factory()
    {
        return new Entity();
    }
    // }
};

#endif // ENTITY_HPP_
