#ifndef VIDEO_HPP_
#define VIDEO_HPP_
#include "opengl.hpp"

#include <SDL2/SDL.h>

#include "shader.hpp"
#include "entity.hpp"
#include "camera.hpp"

#define RAD 0.017453292519943295769236907684886f


class Video
{
    static void checkOpenGLErrors();
public:
    Video();
    virtual ~Video();

    glm::mat4 ProjMat;
    glm::mat4 OrthoMat;
    glm::mat4 ViewMat;

    int init();
    void update();
    void resize(int width, int height);
    void swap();

    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_GLContext context;

    int screen_width;
    int screen_height;

    Camera camera;
};

#endif // VIDEO_HPP_
