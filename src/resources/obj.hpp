#ifndef OBJ_HPP_
#define OBJ_HPP_

class obj;

#include "../resource.hpp"

class OBJ_Resource : public ModelResource
{
public:
    OBJ_Resource();
    ~OBJ_Resource();

    int load(const char *filename);
private:
    obj *data;
};

#endif // OBJ_HPP_
