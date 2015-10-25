#include "png.hpp"

#include <memory.h>
#include <stdio.h>
#include <string.h>

PNG_Resource::PNG_Resource()
{
}

PNG_Resource::~PNG_Resource()
{
}

int PNG_Resource::load(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if(file == NULL) {
        lprintf(LOG_WARNING, "Unable to open ^g\"%s\"^0", filename);
        return 0;
    }

    png_struct *png = png_create_read_struct(
                          PNG_LIBPNG_VER_STRING,
                          NULL,
                          NULL,
                          NULL);

    if(png == NULL) {
        lprintf(LOG_ERROR, "Unable to create png struct!");
        fclose(file);
        return 0;
    }

    png_info *info = png_create_info_struct(png);

    if(info == NULL) {
        lprintf(LOG_ERROR, "Unable to create png_info struct!");
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(file);
        return 0;
    }

    if(setjmp(png_jmpbuf(png))) {
        lprintf(LOG_ERROR, "PNG error!");
        png_destroy_read_struct(&png, &info, NULL);
        fclose(file);
        return 0;
    }

    png_init_io(png, file);
    png_set_sig_bytes(png, 0);

    png_read_png(
        png,
        info,
        PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND,
        NULL);

    int *pbpp = reinterpret_cast<int *>(&bpp);
    int color = 0;
    int interlace = 0;
    png_get_IHDR(
        png,
        info,
        &width,
        &height,
        pbpp,
        &color,
        &interlace,
        NULL,
        NULL);

    uint32_t row_bytes = png_get_rowbytes(png, info);
    unsigned char *imageData = new unsigned char[row_bytes * height];

    png_bytepp row_pointers = png_get_rows(png, info);

    for(unsigned int i = 0; i < height; i++) {
        memcpy(
            imageData + row_bytes * (height - 1 - i),
            row_pointers[i],
            row_bytes);
    }

    png_destroy_read_struct(&png, &info, NULL);
    fclose(file);

    // lprintf(
    //     LOG_INFO,
    //     "PNG Loaded ^g\"%s\"^0 (^y%d^0x^y%d^0x^y%d^0)",
    //     filename,
    //     width,
    //     height,
    //     bpp);

    type = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_NEAREST);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR);

    if(color == PNG_COLOR_TYPE_RGB) {
        type = GL_RGB;
    }

    gluBuild2DMipmaps(
        GL_TEXTURE_2D,
        type,
        width,
        height,
        type,
        GL_UNSIGNED_BYTE,
        imageData);

    delete [] imageData;

    return 1;
}
