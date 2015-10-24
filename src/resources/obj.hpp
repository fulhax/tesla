#ifndef OBJ_HPP_
#define OBJ_HPP_

#include <fast_obj_loader.h>

#include <glm/glm.hpp>

#include "../resource.hpp"

struct obj_face {
    uint32_t f[3];
};

class OBJ_Resource : public ModelResource
{
public:
    OBJ_Resource();
    ~OBJ_Resource();

    int load(const char *filename);
private:
    obj *data;
    obj_face *faces;
};

#endif // OBJ_HPP_
