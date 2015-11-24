#ifndef __OGEX_HPP__
#define __OGEX_HPP__

#include "../resource.hpp"

class OGEX_Resource : public ModelResource
{
public:
    OGEX_Resource();
    ~OGEX_Resource();
    int load(const char *filename);
private:
    float *normal_vb;
    float *pos_vb;
    float *uv_vb;
    unsigned int *indices;
};

#endif //__OGEX_HPP__
