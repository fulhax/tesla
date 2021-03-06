#include "audio.hpp"

#include <memory.h>

#include "engine.hpp"

Audio::Audio()
{
    device = nullptr;
    context = nullptr;
    sources = nullptr;
    max_sources = 0;
}

Audio::~Audio()
{
    alDeleteSources(max_sources, sources);
    lprintf(LOG_INFO, "Shutting down audio");

    if (sources) {
        delete [] sources;
    }

    if (context) {
        alcDestroyContext(context);
    }

    if (device) {
        alcCloseDevice(device);
    }
}

int Audio::init()
{
    device = alcOpenDevice(nullptr);

    if (!device) {
        lprintf(LOG_ERROR, "Failed to open audio device!");
        return 1;
    }

    context = alcCreateContext(device, nullptr);

    if (!context) {
        lprintf(LOG_ERROR, "Could not create OpenAL context!");
        return 1;
    }

    if (alcMakeContextCurrent(context) == ALC_FALSE) {
        lprintf(LOG_ERROR, "Unable to make context current!");
        return 1;
    }

    max_sources = engine.config.getInt("audio.max_sources", 32);
    sources = new uint32_t[max_sources];
    alGenSources(max_sources, sources);

    lprintf(LOG_INFO, "Audio started successfully");

    return 0;
}

void Audio::update(Camera *camera) const
{
    if (device && context) {
        alListener3f(
            AL_POSITION,
            camera->pos.x,
            camera->pos.y,
            camera->pos.z);

        alListener3f(
            AL_ORIENTATION,
            -camera->yaw * RAD,
            -camera->pitch * RAD,
            0);
    }
}

bool Audio::isPlaying(uint32_t source) const
{
    if (!device || !context || !sources) {
        return false;
    }

    if (source > max_sources) {
        return false;
    }

    int state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);

    return (state == AL_PLAYING);
}

int Audio::play(const char *filename, glm::vec3 position)
{
    if (!device || !context) {
        return -1;
    }

    SoundResource *s = engine.resources.getSound(filename);

    if (s) {
        int curr_source = -1;
        int state;

        for (uint8_t i = 0; i < max_sources; i++) {
            alGetSourcei(sources[i], AL_SOURCE_STATE, &state);

            if (state != AL_PLAYING) {
                curr_source = sources[i];
                break;
            }
        }

        //printf("curr:%d\n", curr_source);
        if (curr_source == -1) {
            lprintf(LOG_WARNING, "No free audio sources!");
            return -1;
        }

        alSourcei(curr_source, AL_BUFFER, s->buffer);
        alSourcePlay(curr_source);

        return curr_source;
    }

    return -1;
}
