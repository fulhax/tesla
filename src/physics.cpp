#include "physics.hpp"

#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "engine.hpp"
#include "errorhandler.hpp"

Physics::Physics()
{
    broadphase = 0;
    collision_conf = 0;
    dispatcher = 0;
    solver = 0;
    dynamics_world = 0;
}

Physics::~Physics()
{

}

void Physics::init(glm::vec3 gravity)
{
    broadphase = new btDbvtBroadphase();
    collision_conf = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collision_conf);
    solver = new btSequentialImpulseConstraintSolver();

    dynamics_world = new btDiscreteDynamicsWorld(
        dispatcher,
        broadphase,
        solver,
        collision_conf);

    setGravity(gravity);

    btCollisionShape *groundShape = new btStaticPlaneShape(
        btVector3(0, 1, 0), 1);
    btDefaultMotionState *groundMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -3, 0)));

    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
        0,
        groundMotionState,
        groundShape,
        btVector3(0, 0, 0));

    btRigidBody *groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamics_world->addCollisionObject(groundRigidBody);

    lprintf(LOG_INFO, "Physics started successfully");
}

void Physics::setGravity(glm::vec3 gravity)
{
    dynamics_world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

btRigidBody *Physics::createMesh(ModelResource *m,
                                 glm::vec3 position,
                                 glm::quat orientation,
                                 glm::vec3 size,
                                 int mass)
{
    btDefaultMotionState *motionstate = new btDefaultMotionState(
        btTransform(
            btQuaternion(
                orientation.x,
                orientation.y,
                orientation.z,
                orientation.w),
            btVector3(
                position.x,
                position.y,
                position.z)
        ));

    btTriangleMesh *mesh = new btTriangleMesh();

    for(int i = 0; i < m->num_tris * 3; i+= 3) {
        int i0 = m->indices[i];
        int i1 = m->indices[i + 1];
        int i2 = m->indices[i + 2];

        btVector3 v0(
            m->verts[i0],
            m->verts[i0 + 1],
            m->verts[i0 + 2]);
        btVector3 v1(
            m->verts[i1],
            m->verts[i1 + 1],
            m->verts[i1 + 2]);
        btVector3 v2(
            m->verts[i2],
            m->verts[i2 + 1],
            m->verts[i2 + 2]);

        mesh->addTriangle(v0, v1, v2);
    }

    btConvexShape *tmpshape = new btConvexTriangleMeshShape(mesh);
    btShapeHull* hull = new btShapeHull(tmpshape);
    tmpshape->setUserPointer(hull);
    btScalar margin = tmpshape->getMargin();
    hull->buildHull(margin);

    btConvexHullShape* hullshape = new btConvexHullShape();

    btVector3* points = const_cast<btVector3*>(hull->getVertexPointer());
    for(int i = 0; i < hull->numVertices(); i++) {
        hullshape->addPoint(points[i]);
    }

    btVector3 localInertia(0,0,0);
    if(mass > 0.f) {
        hullshape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        motionstate,
        hullshape,
        localInertia);

    btRigidBody *body = new btRigidBody(info);
    dynamics_world->addRigidBody(body);

    return body;
}

btRigidBody *Physics::createBox(glm::vec3 position,
                                glm::quat orientation,
                                glm::vec3 size,
                                int mass)
{
    btDefaultMotionState *motionstate = new btDefaultMotionState(
        btTransform(
            btQuaternion(
                orientation.x,
                orientation.y,
                orientation.z,
                orientation.w),
            btVector3(
                position.x,
                position.y,
                position.z)
        ));

    btCollisionShape *box = new btBoxShape(btVector3(size.x, size.y, size.z));

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        motionstate,
        box,
        btVector3(0, 0, 0));

    btRigidBody *body = new btRigidBody(info);
    dynamics_world->addRigidBody(body);

    return body;
}

void Physics::update()
{
    dynamics_world->stepSimulation(engine.getMS() * 0.001f, 7);
}
