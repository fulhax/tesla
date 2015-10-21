#ifndef TGA_HPP_
#define TGA_HPP_

#include "../resource.hpp"

class TGA_Resource : public TextureResource
{
public:
    TGA_Resource();
    ~TGA_Resource();

    int load(const char *filename);
    void unload();
};

#endif // TGA_HPP_
