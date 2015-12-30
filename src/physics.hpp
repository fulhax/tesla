#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include <glm/glm.hpp>

#include <btBulletDynamicsCommon.h>

#include "resource.hpp"

class Physics
{
    btBroadphaseInterface *broadphase;
    btDefaultCollisionConfiguration *collision_conf;
    btCollisionDispatcher *dispatcher;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamics_world;
public:
    Physics();
    virtual ~Physics();

    void init(glm::vec3 gravity = glm::vec3(0, -9.81f, 0));
    void update();

    void setGravity(glm::vec3 gravity);

    btRigidBody *createBox(glm::vec3 position,
                           glm::quat orientation,
                           glm::vec3 size,
                           int mass = 0);

    btRigidBody *createMesh(ModelResource *m,
                            glm::vec3 position,
                            glm::quat orientation,
                            glm::vec3 size,
                            int mass = 0);
};

#endif // PHYSICS_HPP_
