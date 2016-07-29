#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <string>
#include <map>

#include "physics.hpp"
#include "resource.hpp"
#include "shader.hpp"
#include "script.hpp"

struct EntityType {
    std::string name;
    std::string script;

    EntityType() {}
    EntityType(
        std::string name,
        std::string script
    ) : name(name), script(script) {}
};

class Entity
{
    std::map<std::string, std::string> textures;
    char model[FILENAME_MAX];

    EntityType *type;

    int ref_count;
    float mass;

    static int cullCheck(const glm::mat4 &ModelMat, ModelResource *m, float scale);
    glm::mat4 getModelMatrix();
public:
    struct {
        btRigidBody *body;
    } physics;

    float scale;

    explicit Entity(EntityType *type);
    Entity() : Entity(nullptr) {}
    virtual ~Entity();

    void spawn(glm::vec3 pos, glm::vec3 rot);
    void draw(const glm::mat4 &Projection, const glm::mat4 &View);
    void update();

    Shader shader;

    void setModel(const std::string &in);
    void setMass(const float mass);
    void setTexture(const std::string &inname, const std::string &infile);
    void attachShader(const std::string &infile);

    void applyForce(glm::vec3 &force);
};

#endif // ENTITY_HPP_
