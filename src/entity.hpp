#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "resource.hpp"
#include "shader.hpp"

class Entity
{
    char model[FILENAME_MAX];
    char texture[FILENAME_MAX];
public:
    Entity();
    virtual ~Entity();

    void init(const char* model, const char* texture);
    void draw(glm::mat4* Projection, glm::mat4* View);

    Shader shader;
    glm::vec3 pos;
};

#endif // ENTITY_HPP_
