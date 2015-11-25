#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <string>
#include <map>

#include "resource.hpp"
#include "shader.hpp"
#include "script.hpp"

#define MAX_NAMELEN 64

class Entity
{
    std::map<std::string, std::string> textures;
    char model[FILENAME_MAX];
    char script[FILENAME_MAX];
    char name[MAX_NAMELEN];

    glm::vec3 pos;
    glm::vec3 rot;
    float size;

    int ref_count;

    static int cullCheck(const glm::mat4 &ModelMat, ModelResource *m);
public:
    Entity();
    virtual ~Entity();

    void init(const char *name, const char *script);
    void draw(const glm::mat4 &Projection, const glm::mat4 &View);
    void update();

    Shader shader;

    void setPos(float x, float y, float z);
    void setRot(float x, float y, float z);
    void setScale(float size);

    void setModel(const std::string &in);
    void setTexture(const std::string &inname, const std::string &infile);
    void attachShader(const std::string &infile);

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
