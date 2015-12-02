#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <glm/glm.hpp>

#include "shader.hpp"
#include "resource.hpp"
#include "shapes.hpp"

class Debugger
{
    Cube debugCube;

    Shader shader;
    ModelResource *model;
    TextureResource *texture;
public:
    Debugger();
    virtual ~Debugger();

    Shader *useDebugShader();
    ModelResource *useDebugModel();
    TextureResource *useDebugTexture();

    int init();
};

#endif // DEBUGGER_HPP_
