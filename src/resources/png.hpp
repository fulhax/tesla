#ifndef PNG_HPP_
#define PNG_HPP_

#include <png.h>

#include "../resource.hpp"

class PNG_Resource : public TextureResource
{
public:
    PNG_Resource();
    ~PNG_Resource();

    int load(const char *filename);
    void unload();
};

#endif // PNG_HPP_
