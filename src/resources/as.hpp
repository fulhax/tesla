#ifndef AS_HPP_
#define AS_HPP_

#include "../resource.hpp"

class AS_Resource : public ScriptResource
{
public:
    AS_Resource();
    ~AS_Resource();

    int load(const char *filename);
};

#endif // AS_HPP_
