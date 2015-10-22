#include "tga.hpp"

#include <memory.h>
#include <stdio.h>

TGA_Resource::TGA_Resource()
{
}

TGA_Resource::~TGA_Resource()
{
}

int TGA_Resource::load(const char *filename)
{
    char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char TGAcompare[12];
    char header[6];
    char *imageData;
    unsigned int bytesPerPixel;
    unsigned int imageSize;

    FILE *file = fopen(filename, "rb");

    if(file == NULL) {
        lprintf(LOG_WARNING, "Unable to open ^g\"%s\"^0", filename);
        return 0;
    }

    if(fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
        memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
        fread(header, 1, sizeof(header), file) != sizeof(header)) {
        fclose(file);

        lprintf(
            LOG_WARNING,
            "Failed to load ^g\"%s\"^0, incorrect header",
            filename);

        return 0;
    }

    width = header[1] * 256 + header[0];
    height = header[3] * 256 + header[2];

    if(width  == 0 || height == 0 || (header[4] != 24 && header[4] != 32)) {
        lprintf(
            LOG_WARNING,
            "Failed to load ^g\"%s\"^0, incorrect format",
            filename);

        fclose(file);
        return 0;
    }

    bpp = header[4];
    bytesPerPixel = bpp / 8;
    imageSize = width * height * bytesPerPixel;

    imageData = new char[imageSize];

    if(imageData == NULL || fread(imageData, 1, imageSize, file) != imageSize) {
        if(imageData != NULL) {
            delete [] imageData;
        }

        lprintf(
            LOG_WARNING,
            "Failed to load ^g\"%s\"^0, unable to read image data",
            filename);

        fclose(file);
        return 0;
    }

    for(unsigned int i = 0; i < imageSize; i += bytesPerPixel) {
        unsigned int temp = imageData[i];
        imageData[i] = imageData[i + 2];
        imageData[i + 2] = temp;
    }

    fclose(file);

    lprintf(
        LOG_INFO,
        "TGA Loaded ^g\"%s\"^0 (^y%d^0x^y%d^0x^y%d^0)",
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

    return 1;
}
