#include "shader.hpp"
#include "engine.hpp"

Shader::Shader()
{
    program = 0;
}

Shader::~Shader()
{
}

void Shader::attach(const char *filename)
{
    shaders.push_back(filename);
}

void Shader::unload()
{
    if(program <= 0) {
        return;
    }

    glDeleteProgram(program);
    program = 0;
}

int Shader::use()
{
    if(program <= 0) {
        program = glCreateProgram();

        if(!program) {
            lprintf(LOG_ERROR, "Unable to create shader program!");
            return 0;
        }

        for(auto filename : shaders) {
            ShaderResource *shader =
                engine.resources.getShader(this, filename.c_str());

            if(shader) {
                glAttachShader(program, shader->handle);
            } else {
                unload();
                return 0;
            }
        }

        glLinkProgram(program);

        int status = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &status);

        if(status == GL_FALSE) {
            int length = 0;
            glGetShaderiv(program, GL_INFO_LOG_LENGTH, &length);

            if(length > 0) {
                char *log = new char[length];
                int written = 0;
                glGetProgramInfoLog(program, length, &written, log);

                lprintf(LOG_ERROR, "Program link failed:%s", log);
                delete [] log;

                unload();
                return 0;
            }
        }

        validate();

        uniform_locations.clear();
    }

    glUseProgram(program);
    return 1;
}

void Shader::validate()
{
    if(program <= 0) {
        lprintf(LOG_ERROR, "Program has not been linked.");
        return;
    }

    GLint status;
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

    if(status == GL_FALSE) {
        int length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        if(length > 0) {
            char *log = new char[length];
            int written = 0;
            glGetProgramInfoLog(program, length, &written, log);

            lprintf(LOG_ERROR, "Program failed to validate, %s", log);

            delete [] log;
        }
    }
}

void Shader::bindAttribLocation(GLuint location, const char *name)
{
    glBindAttribLocation(program, location, name);
}

void Shader::bindFragDataLocation(GLuint location, const char *name)
{
    glBindFragDataLocation(program, location, name);
}

void Shader::setUniform(const char *name, float x, float y, float z)
{
    GLuint loc = getUniformLocation(name);
    glUniform3f(loc, x, y, z);
}

void Shader::setUniform(const char *name, const glm::vec3 &v)
{
    setUniform(name, v.x, v.y, v.z);
}

void Shader::setUniform(const char *name, const glm::vec4 &v)
{
    GLint loc = getUniformLocation(name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void Shader::setUniform(const char *name, const glm::vec2 &v)
{
    GLint loc = getUniformLocation(name);
    glUniform2f(loc, v.x, v.y);
}

void Shader::setUniform(const char *name, const glm::mat3 &m)
{
    GLint loc = getUniformLocation(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void Shader::setUniform(const char *name, const glm::mat4 &m)
{
    GLint loc = getUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void Shader::setUniform(const char *name, float f)
{
    GLint loc = getUniformLocation(name);
    glUniform1f(loc, f);
}

void Shader::setUniform(const char *name, int i)
{
    GLint loc = getUniformLocation(name);
    glUniform1i(loc, i);
}

void Shader::setUniform(const char *name, GLuint i)
{
    GLint loc = getUniformLocation(name);
    glUniform1ui(loc, i);
}

void Shader::setUniform(const char *name, bool b)
{
    GLint loc = getUniformLocation(name);
    glUniform1i(loc, b);
}

int Shader::getUniformLocation(const char *name)
{
    auto pos = uniform_locations.find(name);

    GLint uniform = -1;

    if(pos == uniform_locations.end()) {
        uniform = glGetUniformLocation(program, name);
        uniform_locations[name] = glGetUniformLocation(program, name);

        if(uniform_locations[name] < 0) {
            lprintf(LOG_WARNING, "setUniform failed: %s", name);
        } else {
            uniform_locations[name] = uniform;
        }
    } else {
        uniform = pos->second;
    }

    return uniform;
}
