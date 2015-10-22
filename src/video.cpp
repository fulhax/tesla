#include "video.hpp"
#include "errorhandler.hpp"

#include "engine.hpp"

int Video::init(int width, int height)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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
    glClearColor(0, 0.3f, 0.3f, 0);
    glClearDepth(1);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
        45,
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        100.0f);

    glMatrixMode(GL_MODELVIEW);

    lprintf(LOG_INFO, "Video started successfully");
    lprintf(
        LOG_INFO,
        "^cOpenGL:^0\t%s",
        glGetString(GL_VERSION));
    lprintf(
        LOG_INFO,
        "^cShaderModel:^0\t%s",
        glGetString(GL_SHADING_LANGUAGE_VERSION));
    lprintf(
        LOG_INFO,
        "^cVendor:^0\t%s",
        glGetString(GL_VENDOR));
    lprintf(
        LOG_INFO,
        "^cRenderer:^0\t%s",
        glGetString(GL_RENDERER));

    return 0;
}

void Video::update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0,0,-10);

    TextureResource* t = engine.resources.getTexture("tux.png");
    if(t) {
        glBindTexture(GL_TEXTURE_2D, t->id);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_POLYGON);
        glTexCoord2f(0, 1); 
        glVertex3f(-1, 1, 0);

        glTexCoord2f(1, 1); 
        glVertex3f(1, 1, 0);

        glTexCoord2f(1, 0); 
        glVertex3f(1, -1, 0);

        glTexCoord2f(0, 0); 
        glVertex3f(-1, -1, 0);
        glEnd();

        glDisable(GL_BLEND);
    }

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
