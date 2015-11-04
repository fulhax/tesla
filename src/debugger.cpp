#include "debugger.hpp"

Debugger::Debugger()
{
}

Debugger::~Debugger()
{
}

int Debugger::init()
{
    shader.attach("shaders/debug.vert");
    shader.attach("shaders/debug.frag");

    return 1;
}

Shader *Debugger::useDebugShader()
{
    if(shader.use()) {
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);

        shader.setUniform("in_Color", glm::vec3(1.0, 0.0, 0.0));

        return &shader;
    }

    return nullptr;
}
