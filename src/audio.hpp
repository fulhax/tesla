#ifndef AUDIO_HPP_
#define AUDIO_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <glm/glm.hpp>

#include "camera.hpp"

// TODO(c0r73x): reserve one source for music
#define MAX_SOURCES 32

class Audio
{
public:
    Audio();
    virtual ~Audio();

    int init();
    void update(Camera *camera);
    int play(const char *filename, glm::vec3 position);
    bool isPlaying(uint32_t source);
private:
    ALCdevice *device;
    ALCcontext *context;

    uint32_t sources[MAX_SOURCES];
};

#endif // AUDIO_HPP_
