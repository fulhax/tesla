#ifndef UI_HPP_
#define UI_HPP_

#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "script.hpp"

struct TextData {
    std::vector<glm::vec2> verts;
    std::vector<glm::vec2> uvs;
};

class Ui
{
    glm::mat4 OrthoMat;
public:
    Ui();
    virtual ~Ui();

    void drawRect(int x, int y, int w, int h, const char *texture);
    void startClip(int x, int y, int w, int h);
    void endClip();

    void printDef(int x, int y, const std::string &in);
    void print(const std::string &fontfile, int x, int y, const std::string &in);
    void update();
};

#endif // UI_HPP_
