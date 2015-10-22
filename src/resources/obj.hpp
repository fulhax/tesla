#ifndef OBJ_HPP_
#define OBJ_HPP_

#include "../resource.hpp"

class OBJ_Resource : public ModelResource
{
public:
    OBJ_Resource();
    ~OBJ_Resource();

    int load(const char *filename);
    void unload();
};

#endif // OBJ_HPP_
