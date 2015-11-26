#ifndef UI_HPP_
#define UI_HPP_

#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "script.hpp"

struct TextData
{
    std::vector<glm::vec2> verts;
    std::vector<glm::vec2> uvs;
};

class Ui : public ASClass
{
    glm::mat4 OrthoMat;
public:
    Ui();
    virtual ~Ui();

    void print(int x, int y, const std::string &in);
    void update();
};

#endif // UI_HPP_
