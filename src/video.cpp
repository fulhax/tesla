#include "video.hpp"
#include "errorhandler.hpp"

int Video::init(int width, int height)
{
    SDL_RendererInfo ri;
    int ret = SDL_CreateWindowAndRenderer(
                  width,
                  height,
                  SDL_WINDOW_OPENGL,
                  &window,
                  &renderer);

    if(ret != 0) {
        lprintf(LOG_ERROR, "Failed to create SDL window");
        return 1;
    }

    context = SDL_GL_CreateContext(window);

    if(!context) {
        lprintf(LOG_ERROR, "Failed to create GL context");
        return 1;
    }

    SDL_GetRendererInfo(renderer, &ri);

    if(!(ri.flags & SDL_RENDERER_ACCELERATED) ||
        !(ri.flags & SDL_RENDERER_TARGETTEXTURE)) {

        lprintf(LOG_ERROR, "Unsupported hardware!");
        return 1;
    }

    glShadeModel(GL_SMOOTH);
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);

    glPointSize(5.0f);

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45, static_cast<float>(width / height), 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);

    lprintf(LOG_INFO, "Video started successfully");
    return 0;
}

void Video::update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 6.0, 0.1, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glBegin(GL_POINTS);
    glColor3f(1, 1, 0);
    glVertex3f(0, 0, 0);
    glEnd();

    SDL_GL_SwapWindow(window);
}

void Video::shutdown()
{
    lprintf(LOG_INFO, "Shutting down video");
}

Video::Video()
{
    renderer = 0;
    window = 0;
    context = 0;
}

Video::~Video()
{
}
