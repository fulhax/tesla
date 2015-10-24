#include "video.hpp"
#include "errorhandler.hpp"

#include "engine.hpp"

int Video::init(int width, int height)
{
    screen_width = width;
    screen_height = height;

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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);

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

    glEnable(GL_TEXTURE_2D);

    glClearColor(0, 0.3f, 0.3f, 0);
    glViewport(0, 0, width, height);

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

    testshader.attach("shaders/default.frag");
    testshader.attach("shaders/default.vert");

    return 0;
}

void Video::checkOpenGLErrors()
{
    GLenum err = glGetError();

    while(err != GL_NO_ERROR) {
        lprintf(LOG_ERROR, "OpenGL: %s (%i)", gluErrorString(err), err);
        err = glGetError();
    }
}

void Video::update()
{
    checkOpenGLErrors();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.pos = glm::vec3(0, 0, -5);

    static glm::mat4 mProjection =
        glm::perspective(
            45.0f,
            static_cast<float>(
                static_cast<float>(screen_width) / static_cast<float>(screen_height)
            ),
            0.1f,
            1000.0f);

    glm::mat4 mRotation =
        glm::yawPitchRoll(
            -camera.yaw * RAD,
            -camera.pitch * RAD,
            0.0f);

    glm::mat4 mTranslate = glm::translate(glm::mat4(1.0f), camera.pos);

    glm::mat4 mCamera = mTranslate * mRotation;
    glm::mat4 mView = glm::inverse(mCamera);

    if(testshader.use()) {
        glm::mat4 mModel = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        TextureResource *t = engine.resources.getTexture("WoodResource.png");
        ModelResource *m = engine.resources.getModel("WoodResource.obj");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t->id);

        testshader.bindAttribLocation(0, "in_Position");
        testshader.bindAttribLocation(1, "in_TexCoord");
        testshader.bindAttribLocation(2, "in_Normal");

        testshader.setUniform("DiffuseMap", 0);
        testshader.setUniform("in_ModelMatrix", mModel);
        testshader.setUniform("in_ProjMatrix", mProjection);
        testshader.setUniform("in_ViewMatrix", mView);

        glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, m->uv_buffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, m->normals_buffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indices_buffer);
        glDrawElements(GL_TRIANGLES, m->num_tris * 12, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }

    glUseProgram(0);

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

    screen_width = 0;
    screen_height = 0;
}

Video::~Video()
{
}
