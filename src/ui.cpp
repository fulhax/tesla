#include "ui.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <string>
#include <vector>

#include "engine.hpp"

Ui::Ui()
{
}

Ui::~Ui()
{
}

void Ui::update()
{
    drawRect(10, 10, 200, 200, "test2.png");

    startClip(10, 10, 200, 200);
    drawRect(20, 20, 120, 120, "dagger/Textures/Dagger_Albedo.png");
    endClip();
}

void Ui::startClip(int x, int y, int w, int h)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, (engine.video.screen_height - y) - h, w, h);
}

void Ui::endClip()
{
    glDisable(GL_SCISSOR_TEST);
}

void Ui::drawRect(int x, int y, int w, int h, const char *texture)
{
    Plane plane;

    plane.generate2d(w, h);

    static Shader shader;
    static bool first = true;

    if (first) {
        shader.attach("shaders/ui.frag");
        shader.attach("shaders/ui.vert");
        first = false;
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (shader.use()) {
        TextureResource *t = engine.resources.getTexture(texture);

        if (t == nullptr) {
            t = engine.debugger.useDebugTexture();
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t->id);

        shader.setUniform("Texture", 0);

        shader.setUniform("in_UiPos", glm::vec2(x, y));
        shader.setUniform("in_OrthoMatrix", engine.video.OrthoMat);

        shader.bindAttribLocation(0, "in_Position");
        shader.bindAttribLocation(1, "in_TexCoord");

        glBindBuffer(GL_ARRAY_BUFFER, plane.vertex_buffer);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec3),
            nullptr);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, plane.uv_buffer);
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec2),
            nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane.indices_buffer);
        glDrawElements(
            GL_TRIANGLES,
            plane.num_tris * 12,
            GL_UNSIGNED_INT,
            nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glUseProgram(0);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void Ui::printDef(int x, int y, const std::string &in)
{
    print(engine.config.getString(
              "font.default",
              "fonts/DejaVuSansMono.ttf:12"), x, y, in);
}

void Ui::print(const std::string &fontfile, int x, int y,
               const std::string &in)
{
    static uint32_t vertex_buffer = 0;
    static uint32_t uv_buffer = 0;

    static Shader shader;
    static bool first = true;

    if (first) {
        shader.attach("shaders/font.frag");
        shader.attach("shaders/font.vert");
        first = false;
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (shader.use()) {
        FontResource *font = engine.resources.getFont(fontfile.c_str());

        if (font) {
            if (!vertex_buffer) {
                glGenBuffers(1, &vertex_buffer);
            }

            if (!uv_buffer) {
                glGenBuffers(1, &uv_buffer);
            }

            TextData *text = font->print(in);

            shader.setUniform("in_UiPos", glm::vec2(x, y));
            shader.setUniform("in_Color", glm::vec4(1, 1, 1, 1));
            shader.setUniform("in_OrthoMatrix", engine.video.OrthoMat);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, font->id);
            shader.setUniform("FontTexture", 0);

            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                sizeof(glm::vec2) * text->verts.size(),
                &text->verts[0],
                GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                sizeof(glm::vec2) * text->uvs.size(),
                &text->uvs[0],
                GL_STATIC_DRAW);

            shader.bindAttribLocation(0, "in_Position");
            shader.bindAttribLocation(1, "in_TexCoord");

            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glVertexAttribPointer(
                0,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::vec2),
                nullptr);

            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
            glVertexAttribPointer(
                1,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::vec2),
                nullptr);

            glEnableVertexAttribArray(1);

            glDrawArrays(GL_QUADS, 0, text->verts.size());

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);

            glUseProgram(0);

            delete text;
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}
