#ifndef __OGEX_HPP__
#define __OGEX_HPP__

#ifdef NOENGINE // for my test code without access to opengl or a c++11 compiler TODO: remove when done
class ModelResource {};
#else
#include "../resource.hpp"
#endif

namespace ODDLParser
{
    class DDLNode;
}

class OGEX_Resource : public ModelResource
{
public:
    OGEX_Resource();
    ~OGEX_Resource();
    int load(const char *filename);
private:
    unsigned int numVerts;
    unsigned int numFaces;
    float *color_vb;
    float *normal_vb;
    float *binormal_vb;
    float *tangent_vb;
    float *uv_vb;
    bool load_GeometryObject(ODDLParser::DDLNode *node);
    float *load_vertexbuffer(ODDLParser::DDLNode *node);
    unsigned int *load_indexbuffer(ODDLParser::DDLNode *node);
    void SetupGL();
    void writeObj(const char *filename); // for debugging porposes
};

#endif //__OGEX_HPP__
