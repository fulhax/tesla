#ifndef FREETYPE_HPP_
#define FREETYPE_HPP_

#include <glm/glm.hpp>

#include "../resource.hpp"

#define NUMBER_OF_CHARS 512
#define MAX_TEXTURE_WIDTH 2048

class FT_Resource : public FontResource
{
    struct CharInfo {
        CharInfo()
        {
            x = y = left = top = height = width = advance = row = 0;

            memset(uv, 0, sizeof(glm::vec2) * 4);
            memset(v, 0, sizeof(glm::vec2) * 4);

            bitmap = 0;
        }
        uint32_t x;
        uint32_t y;
        uint32_t left;
        uint32_t top;
        uint32_t height;
        uint32_t width;
        uint32_t advance;
        uint32_t row;

        glm::vec2 uv[4];
        glm::vec2 v[4];

        unsigned char *bitmap;
    };

    CharInfo glyphs[NUMBER_OF_CHARS];

    uint32_t max_height;

    static int next_p2(int a);
    void fillTextureData(uint32_t ch, unsigned char *texture_data) const;
public:
    explicit FT_Resource(void *data);
    ~FT_Resource();

    int load(const char *filename);
};

#endif // FREETYPE_HPP_
