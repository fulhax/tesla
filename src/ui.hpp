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

class Ui : public ASClass<Ui>
{
    glm::mat4 OrthoMat;
public:
    Ui();
    virtual ~Ui();

    void printDef(int x, int y, const std::string &in);
    void print(const std::string &fontfile, int x, int y, const std::string &in);
    static void update();
};

#endif // UI_HPP_
