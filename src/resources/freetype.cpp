#include "freetype.hpp"

#include <string.h>

/* seriosly freetype why? */
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

#include <string>
#include <vector>

FT_Resource::FT_Resource()
{
    max_height = 0;
}

FT_Resource::~FT_Resource()
{
}

int FT_Resource::next_p2(int a)
{
    int rval = 1;

    while(rval < a) {
        rval <<= 1;
    }

    return rval;
}

void FT_Resource::fillTextureData(uint32_t ch,
                                  unsigned char *texture_data) const
{
    unsigned char *ch_bmp = glyphs[ch].bitmap;

    uint32_t bmp_pos = 0;
    uint32_t tex_pos = 0;

    for(uint32_t bmp_y = 0; bmp_y < glyphs[ch].height; bmp_y++) {
        for(uint32_t bmp_x = 0; bmp_x < glyphs[ch].width; bmp_x++) {
            bmp_pos = 2 * (bmp_x + bmp_y * glyphs[ch].width);
            tex_pos = 2 * ((glyphs[ch].x + bmp_x) +
                           ((glyphs[ch].y + bmp_y) * MAX_TEXTURE_WIDTH));

            texture_data[tex_pos] = ch_bmp[bmp_pos];
            texture_data[tex_pos + 1] = ch_bmp[bmp_pos + 1];
        }
    }
}

int FT_Resource::load(const char *filename)
{
    char real_filename[FILENAME_MAX];
    snprintf(real_filename, FILENAME_MAX, "%s", filename);

    char *size = 0;
    strtok_r(real_filename, ":", &size);

    if(size) {
        max_height = atoi(size);
    }

    if(max_height == 0) {
        lprintf(LOG_WARNING, "No fontsize specified for ^g\"%s\"^0.", filename);
        return 0;
    }

    FT_Library library;

    if(FT_Init_FreeType(&library) != 0) {
        lprintf(LOG_WARNING, "Failed to initialize freetype!");
        return 0;
    }

    FT_Face face;

    if(FT_New_Face(library, real_filename, 0, &face)) {

        lprintf(
            LOG_WARNING,
            "Failed to load fontfile ^g\"%s\"^0",
            real_filename);
    }

    FT_Set_Char_Size(face, max_height << 6, max_height << 6, 96, 96);

    int max_width = 0;
    int max_rows = 0;

    for(uint32_t ch = 0; ch < NUMBER_OF_CHARS; ch++) {
        if(FT_Load_Glyph(
                face,
                FT_Get_Char_Index(face, ch),
                FT_LOAD_NO_HINTING)) {

            lprintf(LOG_WARNING, "Unable to load glyph ^r\"%c\"^0", ch);
            return 0;
        }

        FT_Glyph glyph;

        if(FT_Get_Glyph(face->glyph, &glyph)) {
            lprintf(LOG_WARNING, "Unable to get glyph ^r\"%c\"^0", ch);
            return 0;
        }

        FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
        FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);

        FT_Bitmap &bitmap = bitmap_glyph->bitmap;

        glyphs[ch].width = bitmap.width;
        glyphs[ch].height = bitmap.rows;

        glyphs[ch].bitmap =
            new unsigned char[2 *
                              glyphs[ch].width *
                              glyphs[ch].height];

        for(uint32_t y = 0; y < glyphs[ch].height; y++) {
            for(uint32_t x = 0; x < glyphs[ch].width; x++) {
                glyphs[ch].bitmap[2 * (x + y * glyphs[ch].width)] =
                    glyphs[ch].bitmap[2 * (x + y * glyphs[ch].width) + 1] =
                        (x >= bitmap.width || y >= bitmap.rows) ?
                        0 : bitmap.buffer[x + bitmap.width * y];
            }
        }

        max_width += glyphs[ch].width;

        if(max_width > MAX_TEXTURE_WIDTH - 1) {
            max_width = glyphs[ch].width;
            max_rows++;
        }

        if(glyphs[ch].height > max_height) {
            max_height = glyphs[ch].height;
        }

        glyphs[ch].row = max_rows;
        glyphs[ch].left = bitmap_glyph->left;
        glyphs[ch].top = bitmap_glyph->top;
        glyphs[ch].advance = face->glyph->advance.x >> 6;
        glyphs[ch].x = max_width - glyphs[ch].width;
    }

    float texture_height = next_p2(max_height * (max_rows + 1));
    unsigned char *texture_data = new unsigned char[
        MAX_TEXTURE_WIDTH *
        static_cast<int>(texture_height) * 2];

    for(uint32_t ch = 0; ch < NUMBER_OF_CHARS; ch++) {
        glyphs[ch].y = max_height * glyphs[ch].row;
        fillTextureData(ch, texture_data);

        glyphs[ch].uv[0].x =
            static_cast<float>(glyphs[ch].x) /
            MAX_TEXTURE_WIDTH;

        glyphs[ch].uv[0].y =
            static_cast<float>(glyphs[ch].y + glyphs[ch].height) /
            texture_height;

        glyphs[ch].v[0].x = 0.0f;
        glyphs[ch].v[0].y = glyphs[ch].height;

        glyphs[ch].uv[1].x =
            static_cast<float>(glyphs[ch].x) /
            MAX_TEXTURE_WIDTH;

        glyphs[ch].uv[1].y =
            static_cast<float>(glyphs[ch].y) /
            texture_height;

        glyphs[ch].v[1].x = 0.0f;
        glyphs[ch].v[1].y = 0.0f;

        glyphs[ch].uv[2].x =
            static_cast<float>(glyphs[ch].x + glyphs[ch].width) /
            MAX_TEXTURE_WIDTH;

        glyphs[ch].uv[2].y =
            static_cast<float>(glyphs[ch].y) /
            texture_height;

        glyphs[ch].v[2].x = glyphs[ch].width;
        glyphs[ch].v[2].y = 0.0f;

        glyphs[ch].uv[3].x =
            static_cast<float>(glyphs[ch].x + glyphs[ch].width) /
            MAX_TEXTURE_WIDTH;

        glyphs[ch].uv[3].y =
            static_cast<float>(glyphs[ch].y + glyphs[ch].height) /
            texture_height;

        glyphs[ch].v[3].x = glyphs[ch].width;
        glyphs[ch].v[3].y = glyphs[ch].height;

        delete [] glyphs[ch].bitmap;
    }

    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 MAX_TEXTURE_WIDTH,
                 texture_height,
                 0,
                 GL_RG,
                 GL_UNSIGNED_BYTE,
                 texture_data);

    delete [] texture_data;

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return 1;
}

TextData *FT_Resource::print(const std::string &in)
{
    TextData *output = new TextData;

    const char *buffer = in.c_str();

    int len = strlen(buffer);
    int advance = 0;

    for(int c = 0; c < len; c++) {
        int bc = static_cast<int>(buffer[c]);

        advance += glyphs[bc].advance;

        for(int i = 0; i < 4; i++) {
            glm::vec2 pos =
                glm::vec2(
                    glyphs[bc].left + advance + glyphs[bc].v[i].x,
                    glyphs[bc].v[i].y + (max_height - glyphs[bc].top));

            output->verts.push_back(pos);
            output->uvs.push_back(
                glm::vec2(
                    glyphs[bc].uv[i].x,
                    glyphs[bc].uv[i].y
                ));
        }
    }

    return output;
}
