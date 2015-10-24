#include "entity.hpp"

#include "engine.hpp"

Entity::Entity()
{
    memset(texture, 0, FILENAME_MAX);
    memset(model, 0, FILENAME_MAX);
}

Entity::~Entity()
{
}

void Entity::init(const char* model, const char* texture)
{
    snprintf(this->model, FILENAME_MAX, "%s", model);
    snprintf(this->texture, FILENAME_MAX, "%s", texture);
}

void Entity::draw(glm::mat4* Projection, glm::mat4* View)
{
    static float rot = 0;
    rot = (rot+1.0f*engine.time);
    if(rot>360) {
        rot = 0;
    }

    if(shader.use()) {
        glm::mat4 Rot = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0,1,0));
        glm::mat4 Model = glm::translate(Rot, pos);

        TextureResource *t = engine.resources.getTexture(texture);
        ModelResource *m = engine.resources.getModel(model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t->id);

        shader.bindAttribLocation(0, "in_Position");
        shader.bindAttribLocation(1, "in_TexCoord");
        shader.bindAttribLocation(2, "in_Normal");

        shader.setUniform("DiffuseMap", 0);
        shader.setUniform("in_ModelMatrix", Model);
        shader.setUniform("in_ProjMatrix", *Projection);
        shader.setUniform("in_ViewMatrix", *View);

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
}
