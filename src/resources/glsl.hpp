#ifndef GLSL_HPP_
#define GLSL_HPP_

#include "../resource.hpp"

class GLSL_Resource : public ShaderResource
{
public:
    GLSL_Resource();
    ~GLSL_Resource();

    int load(const char *filename);
};

#endif // GLSL_HPP_
