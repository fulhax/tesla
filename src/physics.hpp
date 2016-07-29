#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include <glm/glm.hpp>

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "resource.hpp"

class DebugDrawer : public btIDebugDraw
{
    int debugMode;
public:
    DebugDrawer();
    virtual ~DebugDrawer();


    virtual void drawLine(
        const btVector3 &from,
        const btVector3 &to,
        const btVector3 &fromColor,
        const btVector3 &toColor);

    virtual void drawLine(
        const btVector3 &from,
        const btVector3 &to,
        const btVector3 &color);

    virtual void drawSphere(
        const btVector3 &p,
        btScalar radius,
        const btVector3 &color);

    virtual void drawTriangle(
        const btVector3 &a,
        const btVector3 &b,
        const btVector3 &c,
        const btVector3 &color,
        btScalar alpha);

    virtual void drawContactPoint(
        const btVector3 &PointOnB,
        const btVector3 &normalOnB,
        btScalar distance,
        int lifeTime,
        const btVector3 &color);

    virtual void reportErrorWarning(const char *warningString);
    virtual void draw3dText(const btVector3 &location, const char *textString);
    virtual void setDebugMode(int debugMode);
    virtual int getDebugMode() const;
};

class Physics
{
    btBroadphaseInterface *broadphase;
    btDefaultCollisionConfiguration *collision_conf;
    btCollisionDispatcher *dispatcher;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamics_world;

    DebugDrawer debugDrawer;
public:
    Physics();
    virtual ~Physics();

    void init(glm::vec3 gravity = glm::vec3(0, -9.81f, 0));
    void update();
    void updateCharacter(btKinematicCharacterController* character);

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

    btKinematicCharacterController *createCharacter(glm::vec3 position,
            glm::vec2 size, float stepheight);
};

#endif // PHYSICS_HPP_
