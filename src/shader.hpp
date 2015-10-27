#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <SDL2/SDL_opengl.h>

#include <map>

#include <string>
#include <vector>

class Shader
{
public:
    Shader();
    virtual ~Shader();

    int use();
    void attach(const char *filename);
    void unload();
    void validate();

    void bindAttribLocation(GLuint location, const char *name);
    void bindFragDataLocation(GLuint location, const char *name);

    void setUniform(const char *name, float x, float y, float z);
    void setUniform(const char *name, const glm::vec2 &v);
    void setUniform(const char *name, const glm::vec3 &v);
    void setUniform(const char *name, const glm::vec4 &v);
    void setUniform(const char *name, const glm::mat4 &m);
    void setUniform(const char *name, const glm::mat3 &m);
    void setUniform(const char *name, float f);
    void setUniform(const char *name, int i);
    void setUniform(const char *name, bool b);
    void setUniform(const char *name, GLuint i);
private:
    std::map<const char *, int> uniform_locations;
    GLint getUniformLocation(const char *name);

    uint32_t program;
    std::vector<std::string> shaders;
};

#endif // SHADER_HPP_
