#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <glm/glm.hpp>

#include "shader.hpp"

class Debugger
{
    Shader shader;
public:
    Debugger();
    virtual ~Debugger();

    Shader *useDebugShader();
    int init();
};

#endif // DEBUGGER_HPP_
