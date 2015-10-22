#ifndef VIDEO_HPP_
#define VIDEO_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

class Video
{
public:
    Video();
    virtual ~Video();

    int init(int width = 640, int height = 480);
    void update();
    void shutdown();

    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_GLContext context;
private:
    void checkOpenGLErrors();
};

#endif // VIDEO_HPP_
