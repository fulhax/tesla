#ifndef AUDIO_HPP_
#define AUDIO_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <glm/glm.hpp>

#include "camera.hpp"

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

    // TODO(c0r73x): reserve one source for music
    uint8_t max_sources;
    uint32_t *sources;
};

#endif // AUDIO_HPP_
