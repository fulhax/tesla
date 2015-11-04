#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <glm/glm.hpp>

#include "shader.hpp"

class DebugBox
{
    glm::vec3 verts[8];
    uint16_t indices[16];
public:
    DebugBox();
    virtual ~DebugBox();

    uint32_t vertex_buffer;
    uint32_t indices_buffer;
};

class Debugger
{
    Shader shader;
public:
    Debugger();
    virtual ~Debugger();

    Shader* useDebugShader();
    int init();
};

#endif // DEBUGGER_HPP_
