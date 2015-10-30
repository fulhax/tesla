#include "audio.hpp"

#include <memory.h>

#include "engine.hpp"

Audio::Audio()
{
    device = 0;
    context = 0;
    memset(sources, 0, MAX_SOURCES);
}

Audio::~Audio()
{
    alDeleteSources(MAX_SOURCES, sources);
    lprintf(LOG_INFO, "Shutting down audio");

    if(context) {
        alcDestroyContext(context);
    }

    if(device) {
        alcCloseDevice(device);
    }
}

int Audio::init()
{
    device = alcOpenDevice(0);

    if(!device) {
        lprintf(LOG_ERROR, "Failed to open audio device!");
        return 1;
    }

    context = alcCreateContext(device, 0);

    if(!context) {
        lprintf(LOG_ERROR, "Could not create OpenAL context!");
        return 1;
    }

    if(alcMakeContextCurrent(context) == ALC_FALSE) {
        lprintf(LOG_ERROR, "Unable to make context current!");
        return 1;
    }

    alGenSources(MAX_SOURCES, sources);

    lprintf(LOG_INFO, "Audio started successfully");

    return 0;
}

void Audio::update(glm::vec3 player)
{
    if(device && context) {
        alListener3f(AL_POSITION, player.x, player.y, player.z);
    }
}

bool Audio::isPlaying(uint32_t source)
{
    if(!device || !context) {
        return false;
    }

    if(source > MAX_SOURCES) {
        return false;
    }

    int state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);

    return (state == AL_PLAYING);
}

int Audio::play(const char *filename, glm::vec3 position)
{
    if(!device || !context) {
        return -1;
    }

    SoundResource *s = engine.resources.getSound(filename);

    if(s) {
        int curr_source = -1;
        int state;

        for(auto source : sources) {
            alGetSourcei(source, AL_SOURCE_STATE, &state);

            if(state != AL_PLAYING) {
                curr_source = source;
                break;
            }
        }

        //printf("curr:%d\n", curr_source);
        if(curr_source == -1) {
            lprintf(LOG_WARNING, "No free audio sources!");
            return -1;
        }

        alSourcei(curr_source, AL_BUFFER, s->buffer);
        alSourcePlay(curr_source);

        return curr_source;
    }

    return -1;
}
