#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "resource.hpp"
#include "shader.hpp"

#define MAX_NAMELEN 64

class Entity
{
    char model[FILENAME_MAX];
    char texture[FILENAME_MAX];
    char name[MAX_NAMELEN];
public:
    Entity();
    virtual ~Entity();

    void init(const char *name, const char *model, const char *texture);
    void draw(glm::mat4 *Projection, glm::mat4 *View, glm::mat4 *World);

    Shader shader;
    glm::vec3 pos;
};

#endif // ENTITY_HPP_
