#ifndef VIDEO_HPP_
#define VIDEO_HPP_
#ifdef _WIN32
#include <GL/gl3w.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <GL/glu.h>

#include "shader.hpp"
#include "entity.hpp"
#include "camera.hpp"

#define RAD 0.017453292519943295769236907684886f


class Video
{
public:
    Video();
    virtual ~Video();

    int init();
    void update();
    void resize(int width, int height);

    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_GLContext context;

    Camera camera;
private:
    glm::mat4 ProjMat;

    static void checkOpenGLErrors();

    int screen_width;
    int screen_height;
};

#endif // VIDEO_HPP_
