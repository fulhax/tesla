#include "entity.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "engine.hpp"

Entity::Entity(EntityType *type)
{
    this->type = type;
    memset(model, 0, FILENAME_MAX);

    ref_count = 1;
    scale = 1.0f;
    mass = 0;

    physics.body = 0;
}

Entity::~Entity()
{
}

void Entity::setMass(const float mass)
{
    this->mass = mass;
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

void Entity::spawn(glm::vec3 pos, glm::vec3 rot)
{
    if(type == nullptr) {
        lprintf(LOG_WARNING, "Trying to spawn uninitialized entity!");
        return;
    }

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

    ModelResource *m = engine.resources.getModel(model);

    if(m == nullptr) {
        m = engine.debugger.useDebugModel();
    }

    physics.body = engine.physics.createMesh(
                       m,
                       pos,
                       glm::quat(rot),
                       glm::vec3(scale, scale, scale),
                       mass);
}

int Entity::cullCheck(const glm::mat4 &ModelMat, ModelResource *m)
{
    Camera *camera = &engine.camera;

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

glm::mat4 Entity::getModelMatrix()
{
    btTransform transform = physics.body->getCenterOfMassTransform();

    glm::mat4 ModelMat = glm::mat4(1.0);
    transform.getOpenGLMatrix(glm::value_ptr(ModelMat));

    return ModelMat;
}

void Entity::draw(const glm::mat4 &ProjMat, const glm::mat4 &ViewMat)
{
    if(!strlen(model) || textures.empty()) {
        return;
    }

    glm::mat4 ModelMat = getModelMatrix();

    ModelResource *m = engine.resources.getModel(model);

    if(m == nullptr) {
        m = engine.debugger.useDebugModel();
    }

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

        if(t == nullptr) {
            t = engine.debugger.useDebugTexture();
        }

        glActiveTexture(GL_TEXTURE0 + num);
        glBindTexture(GL_TEXTURE_2D, t->id);

        current->setUniform(texture.first.c_str(), num);
        num++;
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
            3,
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
            4,
            1,
            GL_FLOAT,
            GL_FALSE,
            sizeof(float),
            nullptr);

        glEnableVertexAttribArray(4);
    }

    if(m->has_color_buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, m->color_buffer);
        glVertexAttribPointer(
            5,
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
