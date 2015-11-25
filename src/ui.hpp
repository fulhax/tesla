#ifndef UI_HPP_
#define UI_HPP_

#include <glm/glm.hpp>

#include <vector>

struct TextData
{
    std::vector<glm::vec2> verts;
    std::vector<glm::vec2> uvs;
};

class Ui
{
    glm::mat4 OrthoMat;
public:
    Ui();
    virtual ~Ui();

    void print(int x, int y, const char *format, ...);
    void update();
};

#endif // UI_HPP_