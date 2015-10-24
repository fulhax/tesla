#ifndef VIDEO_HPP_
#define VIDEO_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "shader.hpp"
#include "entity.hpp"

#define RAD 0.017453292519943295769236907684886f

struct Camera {
    glm::vec3 pos;

    float yaw;
    float pitch;

    Camera()
    {
        pos = glm::vec3(0.0f, 0.0f, 0.0f);
        yaw = 0;
        pitch = 0;
    }
};

class Video
{
public:
    Video();
    virtual ~Video();

    int init(int width = 1024, int height = 768);
    void update();

    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_GLContext context;

    Camera camera;
private:
    void checkOpenGLErrors();

    int screen_width;
    int screen_height;

    Entity testentity[3];
};

#endif // VIDEO_HPP_
