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
    float *normal_vb;
    float *pos_vb;
    float *uv_vb;
    unsigned int *indices;
    bool load_GeometryObject(ODDLParser::DDLNode *node);
    float *load_vertexbuffer(ODDLParser::DDLNode *node);
    unsigned int *load_indexbuffer(ODDLParser::DDLNode *node);
};

#endif //__OGEX_HPP__
