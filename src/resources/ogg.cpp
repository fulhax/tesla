#include "ogg.hpp"

#include <AL/alc.h>
#include <AL/alext.h>

#include <vorbis/vorbisfile.h>

#define BUFFER_SIZE 32768

OGG_Resource::OGG_Resource()
{
}

OGG_Resource::~OGG_Resource()
{
    bufferData.clear();
}

int OGG_Resource::load(const char *filename)
{
    char array[BUFFER_SIZE];
    int64_t bytes;
    int bitStream;
    int endian = 0;

    vorbis_info *info;
    OggVorbis_File file;

    if (ov_fopen(filename, &file) != 0) {
        lprintf(
            LOG_ERROR,
            "Unable to open Ogg Vorbis file ^g\"%s\"^0!",
            filename);
        return 0;
    }

    info = ov_info(&file, -1);

    if (info->channels == 1) {
        format = AL_FORMAT_MONO16;
    } else {
        format = AL_FORMAT_STEREO16;
    }

    freq = info->rate;

    do {
        bytes = ov_read(&file, array, BUFFER_SIZE, endian, 2, 1, &bitStream);

        if (bytes < 0) {
            lprintf(LOG_ERROR, "Ogg Vorbis read error!");
            ov_clear(&file);

            ov_clear(&file);
            bufferData.clear();

            return 0;
        }

        bufferData.insert(bufferData.end(), array, array + bytes);
    } while (bytes > 0);

    ov_clear(&file);

    alBufferData(
        buffer,
        format,
        &bufferData[0],
        static_cast<ALsizei>(bufferData.size()),
        freq);

    return 1;
}
