#ifndef __OGEX_HPP__
#define __OGEX_HPP__

#include "../resource.hpp"

class OGEX_Resource : public ModelResource
{
public:
    OGEX_Resource();
    ~OGEX_Resource();
    int load(const char *filename);
};

#endif //__OGEX_HPP__
