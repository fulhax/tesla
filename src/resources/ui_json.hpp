#ifndef UIJSON_HPP_
#define UIJSON_HPP_

#include "../resource.hpp"

class UI_Resource : public UiResource
{
public:
    UI_Resource();
    ~UI_Resource();

    int load(const char *filename);
};

#endif // UIJSON_HPP_
