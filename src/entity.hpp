#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <string>
#include <map>

#include "resource.hpp"
#include "shader.hpp"
#include "script.hpp"

struct EntityType {
    std::string name;
    std::string script;

    EntityType() {}
    EntityType(std::string name, std::string script)
        : name(name), script(script) {}
};

class Entity : public ASClass<Entity>
{
    std::map<std::string, std::string> textures;
    char model[FILENAME_MAX];

    EntityType *type;

    glm::vec3 pos;
    glm::vec3 rot;
    float size;

    int ref_count;

    static int cullCheck(const glm::mat4 &ModelMat, ModelResource *m);
public:
    explicit Entity(EntityType *type);
    Entity() : Entity(nullptr) {}
    virtual ~Entity();

    void spawn(glm::vec3 pos);
    void draw(const glm::mat4 &Projection, const glm::mat4 &View);
    void update();

    Shader shader;

    void setPos(float x, float y, float z);
    void setRot(float x, float y, float z);
    void setScale(float size);

    void setModel(const std::string &in);
    void setTexture(const std::string &inname, const std::string &infile);
    void attachShader(const std::string &infile);
};

#endif // ENTITY_HPP_
