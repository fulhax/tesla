#ifndef UIJSON_HPP_
#define UIJSON_HPP_

#include "../resource.hpp"
#include "../../externals/json.hpp"

// class UiElement {};

class UI_Resource : public UiResource
{
public:
    UI_Resource();
    UI_Resource(const char *type);
    ~UI_Resource();

    int load(const char *filename);
    void setByJson(nlohmann::json &json);
};

#endif // UIJSON_HPP_
