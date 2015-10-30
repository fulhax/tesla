#ifndef OGG_HPP_
#define OGG_HPP_

#include <vector>

#include "../resource.hpp"

class OGG_Resource : public SoundResource
{
public:
    OGG_Resource();
    ~OGG_Resource();

    int load(const char *filename);
private:
    std::vector <char> bufferData;
};

#endif // OGG_HPP_
