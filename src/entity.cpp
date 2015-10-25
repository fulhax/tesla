#include "entity.hpp"

#include "engine.hpp"

Entity::Entity()
{
    memset(texture, 0, FILENAME_MAX);
    memset(model, 0, FILENAME_MAX);
    memset(script, 0, FILENAME_MAX);

    memset(name, 0, MAX_NAMELEN);

    ref_count = 1;
}

Entity::~Entity()
{
}

void Entity::setPos(float x, float y, float z)
{
    pos = glm::vec3(x, y, z);
}

void Entity::setModel(const std::string &in)
{
    snprintf(model, FILENAME_MAX, "%s", in.c_str());
}

void Entity::setTexture(const std::string &in)
{
    snprintf(texture, FILENAME_MAX, "%s", in.c_str());
}

void Entity::init(const char *name, const char *script)
{
    snprintf(this->name, MAX_NAMELEN, "%s", name);
    snprintf(this->script, FILENAME_MAX, "%s", script);

    lprintf(LOG_INFO, "Entity ^g\"%s\"^0 initialized", name);

    ScriptResource *s = engine.resources.getScript(script);
    engine.script.run(s->module, "void init(Entity@ this)", this);
}

void Entity::draw(const glm::mat4 &Projection, const glm::mat4 &View)
{
    ScriptResource *s = engine.resources.getScript(script);
    engine.script.run(s->module, "void update(Entity@ this)", this);

    if(!strlen(model) || !strlen(texture)) {
        return;
    }

    static float rot = 0;
    rot = (rot + 1.0f * engine.time);

    if(rot > 360) {
        rot = 0;
    }

    if(shader.use()) {
        glm::mat4 Pos = glm::translate(glm::mat4(1.0f), pos);
        glm::mat4 Rot = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 1));
        glm::mat4 Model = Pos * Rot;

        ModelResource *m = engine.resources.getModel(model);

        if(m) {
            TextureResource *t = engine.resources.getTexture(texture);

            if(t) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, t->id);
            }

            shader.bindAttribLocation(0, "in_Position");
            shader.bindAttribLocation(1, "in_TexCoord");
            shader.bindAttribLocation(2, "in_Normal");

            shader.setUniform("DiffuseMap", 0);
            shader.setUniform("in_ModelMatrix", Model);
            shader.setUniform("in_ProjMatrix", Projection);
            shader.setUniform("in_ViewMatrix", View);

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
    }

    glUseProgram(0);
}
