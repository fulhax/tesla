#include "entity.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <string>

#include "engine.hpp"

Entity::Entity(EntityType *type)
{
    this->type = type;
    memset(model, 0, FILENAME_MAX);

    ref_count = 1;

    pos = glm::vec3(0, 0, 0);
    rot = glm::vec3(0, 0, 0);
    scale = 1.0f;
}

Entity::~Entity()
{
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

void Entity::spawn(glm::vec3 pos)
{
    if(type == nullptr) {
        lprintf(LOG_WARNING, "Trying to spawn uninitialized entity!");
        return;
    }

    this->pos = pos;

    lprintf(LOG_INFO, "Spawning Entity ^m\"%s\"^0", type->name.c_str());
    ScriptResource *s = engine.resources.getScript(type->script.c_str());

    if(s) {
        engine.script.run(s, "void init(Entity@ self)", this);
    } else {
        lprintf(
            LOG_WARNING,
            "Entity ^m\"%s\"^0 no script found!",
            type->name.c_str());
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
    if(type == nullptr) {
        lprintf(LOG_WARNING, "Trying to update uninitialized entity!");
        return;
    }

    ScriptResource *s = engine.resources.getScript(type->script.c_str());

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
        glm::mat4 Scale = glm::scale(
                              glm::mat4(1.0f),
                              glm::vec3(scale, scale, scale));

        glm::mat4 Pos = glm::translate(glm::mat4(1.0f), pos);

        glm::mat4 RotX = glm::rotate(
                             glm::mat4(1.0f),
                             rot.x,
                             glm::vec3(1, 0, 0));

        glm::mat4 RotY = glm::rotate(
                             glm::mat4(1.0f),
                             rot.y,
                             glm::vec3(0, 1, 0));

        glm::mat4 RotZ = glm::rotate(
                             glm::mat4(1.0f),
                             rot.z,
                             glm::vec3(0, 0, 1));

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

        if(m->has_uv_buffer) {
            current->bindAttribLocation(1, "in_TexCoord");
        }

        if(m->has_normals_buffer) {
            current->bindAttribLocation(2, "in_Normal");
        }

        if(m->has_tangent_buffer) {
            current->bindAttribLocation(3, "in_Tangent");
        }

        if(m->has_binormals_buffer) {
            current->bindAttribLocation(4, "in_Binormal");
        }

        if(m->has_color_buffer) {
            current->bindAttribLocation(5, "in_Color");
        }

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

        if(m->has_uv_buffer) {
            glBindBuffer(GL_ARRAY_BUFFER, m->uv_buffer);
            glVertexAttribPointer(
                1,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::vec2),
                nullptr);

            glEnableVertexAttribArray(1);
        }

        if(m->has_normals_buffer) {
            glBindBuffer(GL_ARRAY_BUFFER, m->normals_buffer);
            glVertexAttribPointer(
                2,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::vec3),
                nullptr);

            glEnableVertexAttribArray(2);
        }

        if(m->has_tangent_buffer) {
            glBindBuffer(GL_ARRAY_BUFFER, m->tangent_buffer);
            glVertexAttribPointer(
                2,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::vec3),
                nullptr);

            glEnableVertexAttribArray(3);
        }

        if(m->has_binormals_buffer) {
            glBindBuffer(GL_ARRAY_BUFFER, m->binormals_buffer);
            glVertexAttribPointer(
                2,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::vec3),
                nullptr);

            glEnableVertexAttribArray(4);
        }

        if(m->has_color_buffer) {
            glBindBuffer(GL_ARRAY_BUFFER, m->color_buffer);
            glVertexAttribPointer(
                2,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::vec3),
                nullptr);

            glEnableVertexAttribArray(5);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indices_buffer);
        glDrawElements(
            GL_TRIANGLES,
            m->num_tris * 12,
            GL_UNSIGNED_INT,
            nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);

        if(m->has_uv_buffer) {
            glDisableVertexAttribArray(1);
        }

        if(m->has_normals_buffer) {
            glDisableVertexAttribArray(2);
        }

        if(m->has_tangent_buffer) {
            glDisableVertexAttribArray(3);
        }

        if(m->has_binormals_buffer) {
            glDisableVertexAttribArray(4);
        }

        if(m->has_color_buffer) {
            glDisableVertexAttribArray(5);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glUseProgram(0);
    }
}
