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

int Entity::cullCheck(const glm::mat4 &ModelMat, ModelResource *m)
{
    Camera *camera = &engine.video.camera;

    glm::vec4 tmax =  ModelMat * glm::vec4(m->bounding_box.max, 1.0);
    glm::vec4 tmin =  ModelMat * glm::vec4(m->bounding_box.min, 1.0);

    glm::vec3 bb_size =
        glm::vec3(
            tmax.x - tmin.x,
            tmax.y - tmin.y,
            tmax.z - tmin.z
        );

    glm::vec3 bb_center =
        glm::vec3(
            (tmin.x + tmax.x) / 2,
            (tmin.y + tmax.y) / 2,
            (tmin.z + tmax.z) / 2
        );

    if(camera->rectInFrustum(bb_center, bb_size) == FRUSTUM_OUTSIDE) {
        return 0;
    }

    return 1;
}

void Entity::update()
{
    ScriptResource *s = engine.resources.getScript(script);

    if(s) {
        engine.script.run(s, "void update(Entity@ self)", this);
    }
}

void Entity::draw(const glm::mat4 &ProjMat, const glm::mat4 &ViewMat)
{
    if(!strlen(model) || textures.empty()) {
        return;
    }

    ModelResource *m = engine.resources.getModel(model);

    if(m) {
        glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(size, size, size));
        glm::mat4 Pos = glm::translate(glm::mat4(1.0f), pos);

        glm::mat4 RotX = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1, 0, 0));
        glm::mat4 RotY = glm::rotate(glm::mat4(1.0f), rot.y, glm::vec3(0, 1, 0));
        glm::mat4 RotZ = glm::rotate(glm::mat4(1.0f), rot.z, glm::vec3(0, 0, 1));

        glm::mat4 ModelMat = Pos * Scale *  RotX * RotY * RotZ;

        if(!cullCheck(ModelMat, m)) {
            return;
        }

        Shader *current = &shader;

        if(!current->use()) {
            current = engine.debugger.useDebugShader();

            if(!current) {
                lprintf(LOG_ERROR, "Unable to load debug shader!");
                return;
            }
        }

        int num = 0;

        for(auto texture : textures) {
            TextureResource *t = engine.resources.getTexture(texture.second.c_str());

            if(t) {
                glActiveTexture(GL_TEXTURE0 + num);
                glBindTexture(GL_TEXTURE_2D, t->id);

                current->setUniform(texture.first.c_str(), num);
                num++;
            }
        }


        current->bindAttribLocation(0, "in_Position");
        current->bindAttribLocation(1, "in_TexCoord");
        current->bindAttribLocation(2, "in_Normal");

        current->setUniform("in_ModelMatrix", ModelMat);
        current->setUniform("in_ProjMatrix", ProjMat);
        current->setUniform("in_ViewMatrix", ViewMat);

        glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec3),
            nullptr);

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, m->uv_buffer);
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec2),
            nullptr);

        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, m->normals_buffer);
        glVertexAttribPointer(
            2,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec3),
            nullptr);

        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indices_buffer);
        glDrawElements(
            GL_TRIANGLES,
            m->num_tris * 12,
            GL_UNSIGNED_INT,
            nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glUseProgram(0);
    }
}
