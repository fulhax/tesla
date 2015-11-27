#include "ui.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <vector>
#include <string>

#include "engine.hpp"

Ui::Ui()
{
}

Ui::~Ui()
{
}

void Ui::update()
{
}

void Ui::print(glm::vec2 pos, const std::string &in)
{
    static uint32_t vertex_buffer = 0;
    static uint32_t uv_buffer = 0;

    static Shader shader;
    static bool first = true;

    if(first) {
        shader.attach("shaders/ui.frag");
        shader.attach("shaders/ui.vert");
        first = false;
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(shader.use()) {
        FontResource *font =
            engine.resources.getFont("fonts/DejaVuSansMono.ttf:14");

        if(font) {
            if(!vertex_buffer) {
                glGenBuffers(1, &vertex_buffer);
            }

            if(!uv_buffer) {
                glGenBuffers(1, &uv_buffer);
            }

            TextData *text = font->print(in);

            shader.setUniform("in_UiPos", pos);
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
