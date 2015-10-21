#include "png.hpp"

#include <stdio.h>
#include <memory.h>

PNG_Resource::PNG_Resource()
{
}

PNG_Resource::~PNG_Resource()
{
}

int PNG_Resource::load(const char *filename)
{
    png_byte header[8];

    png_struct *png = png_create_read_struct(
                          PNG_LIBPNG_VER_STRING,
                          NULL,
                          NULL,
                          NULL);

    if(png == NULL) {
        lprintf(LOG_ERROR, "Unable to create png struct!");
        return 0;
    }

    png_info *info = png_create_info_struct(png);

    if(info == NULL) {
        lprintf(LOG_ERROR, "Unable to create png_info struct!");
        png_destroy_read_struct(&png, NULL, NULL);
        return 0;
    }

    png_info *info_end = png_create_info_struct(png);

    if(info == NULL) {
        lprintf(LOG_ERROR, "Unable to create png_info struct!");
        png_destroy_read_struct(&png, &info, NULL);
        return 0;
    }

    if(setjmp(png_jmpbuf(png))) {
        lprintf(LOG_ERROR, "PNG error!");
        png_destroy_read_struct(&png, &info, &info_end);
        return 0;
    }

    FILE *file = fopen(filename, "rb");

    if(file == NULL) {
        lprintf(LOG_WARNING, "Unable to open ^g\"%s\"^0", filename);
        return 0;
    }

    fread(header, 1, 8, file);

    if(png_sig_cmp(header, 0, 8)) {
        lprintf(LOG_ERROR, "^g\"%s\"^0 is not a PNG file!", filename);
        png_destroy_read_struct(&png, &info, &info_end);
        fclose(file);
        return 0;
    }

    png_init_io(png, file);
    png_set_sig_bytes(png, 8);

    png_read_info(png, info);

    int* pbpp = reinterpret_cast<int*>(&bpp);
    png_get_IHDR(png, info, &width, &height, pbpp, NULL, NULL, NULL, NULL);

    png_read_update_info(png, info);

    int imageSize = png_get_rowbytes(png, info);
    imageSize += 3 - ((imageSize - 1) % 4);

    png_bytep imageData = new png_byte[imageSize * height * sizeof(png_byte) + 15];
    png_bytepp imageDataPtr = new png_bytep[height];

    for(uint32_t i = 0; i < height; ++i) {
        imageDataPtr[height - 1 - i] = imageData + i * imageSize;
    }

    png_read_image(png, imageDataPtr);
    png_destroy_read_struct(&png, &info, &info_end);

    fclose(file);

    lprintf(
        LOG_INFO,
        "PNG Loaded ^g\"%s\"^0 (^y%d^0x^y%d^0x^y%d^0)",
        filename,
        width,
        height,
        bpp);

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

    if(bpp == 24) {
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
    delete [] imageDataPtr;

    return 1;
}
