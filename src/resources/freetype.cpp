#include "freetype.hpp"

#include <string.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

FT_Resource::FT_Resource(void *data) : FontResource(data)
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

void FT_Resource::fillTextureData(uint32_t ch, unsigned char *texture_data)
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
    FT_Library library;

    if(FT_Init_FreeType(&library) != 0) {
        lprintf(LOG_WARNING, "Failed to initialize freetype!");
        return 0;
    }

    FT_Face face;

    if(FT_New_Face(library, filename, 0, &face)) {

        lprintf(
            LOG_WARNING,
            "Failed to load fontfile ^g\"%s\"^0",
            filename);
    }

    FT_Set_Char_Size(face, fontsize << 6, fontsize << 6, 96, 96);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 MAX_TEXTURE_WIDTH,
                 texture_height,
                 0,
                 GL_LUMINANCE_ALPHA,
                 GL_UNSIGNED_BYTE,
                 texture_data);

    delete [] texture_data;

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return 1;
}
