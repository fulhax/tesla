#include "entity.hpp"

#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "engine.hpp"

Entity::Entity()
{
    memset(model, 0, FILENAME_MAX);
    memset(script, 0, FILENAME_MAX);

    memset(name, 0, MAX_NAMELEN);

    ref_count = 1;

    pos = glm::vec3(0, 0, 0);
    rot = glm::vec3(0, 0, 0);
    size = 1.0f;
}

Entity::~Entity()
{
}

void Entity::setScale(float size)
{
    this->size = size;
}

void Entity::setRot(float x, float y, float z)
{
    rot = glm::vec3(x, y, z);
}

void Entity::setPos(float x, float y, float z)
{
    pos = glm::vec3(x, y, z);
}

void Entity::setModel(const std::string &in)
{
    snprintf(model, FILENAME_MAX, "%s", in.c_str());
}

void Entity::attachShader(const std::string &infile)
{
    shader.attach(infile.c_str());
}

void Entity::setTexture(const std::string &inname, const std::string &infile)
{
    textures[inname] = infile;
    //snprintf(texture, FILENAME_MAX, "%s", in.c_str());
}

void Entity::init(const char *name, const char *script)
{
    snprintf(this->name, MAX_NAMELEN, "%s", name);
    snprintf(this->script, FILENAME_MAX, "%s", script);

    lprintf(LOG_INFO, "Entity ^m\"%s\"^0 initialized", name);

    ScriptResource *s = engine.resources.getScript(script);

    if(s) {
        engine.script.run(s, "void init(Entity@ self)", this);
    } else {
        lprintf(LOG_WARNING, "Entity ^m\"%s\"^0 no script found!", name);
    }
}

void Entity::draw(const glm::mat4 &Projection, const glm::mat4 &View)
{
    ScriptResource *s = engine.resources.getScript(script);

    if(s) {
        engine.script.run(s, "void update(Entity@ self)", this);
    }

    if(!strlen(model) || !textures.size()) {
        return;
    }

    if(shader.use()) {
        glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(size, size, size));
        glm::mat4 Pos = glm::translate(glm::mat4(1.0f), pos);

        glm::mat4 RotX = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1, 0, 0));
        glm::mat4 RotY = glm::rotate(glm::mat4(1.0f), rot.y, glm::vec3(0, 1, 0));
        glm::mat4 RotZ = glm::rotate(glm::mat4(1.0f), rot.z, glm::vec3(0, 0, 1));

        glm::mat4 Model = Pos * Scale *  RotX * RotY * RotZ;

        ModelResource *m = engine.resources.getModel(model);

        if(m) {
            int num = 0;

            for(auto texture : textures) {
                TextureResource *t = engine.resources.getTexture(texture.second.c_str());

                if(t) {
                    glActiveTexture(GL_TEXTURE0 + num);
                    glBindTexture(GL_TEXTURE_2D, t->id);

                    shader.setUniform(texture.first.c_str(), num);
                    num++;
                }
            }


            shader.bindAttribLocation(0, "in_Position");
            shader.bindAttribLocation(1, "in_TexCoord");
            shader.bindAttribLocation(2, "in_Normal");

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
