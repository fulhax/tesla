#include "physics.hpp"

#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "engine.hpp"
#include "errorhandler.hpp"

DebugDrawer::DebugDrawer()
{
    debugMode = 0;
}

DebugDrawer::~DebugDrawer()
{
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to,
                           const btVector3 &fromColor, const btVector3 &toColor)
{
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to,
                           const btVector3 &color)
{
    std::vector<glm::vec3> verts;
    verts.push_back(glm::vec3(from.x(), from.y(), from.z()));
    verts.push_back(glm::vec3(to.x(), to.y(), to.z()));

    static uint32_t vertex_buffer = 0;

    static Shader shader;
    static bool first = true;

    if(first) {
        shader.attach("shaders/debug.frag");
        shader.attach("shaders/debug.vert");
        first = false;
    }

    if(shader.use()) {
        if(!vertex_buffer) {
            glGenBuffers(1, &vertex_buffer);
        }

        shader.setUniform("in_Color", glm::vec3(
                              color.x(),
                              color.y(),
                              color.z()));

        shader.setUniform("in_ModelMatrix", glm::mat4(1.f));
        shader.setUniform("in_ViewMatrix", engine.video.ViewMat);
        shader.setUniform("in_ProjMatrix", engine.video.ProjMat);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(glm::vec3) * verts.size(),
            &verts[0],
            GL_STATIC_DRAW);

        shader.bindAttribLocation(0, "in_Position");

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec3),
            nullptr);

        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINES, 0, verts.size());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);

        glUseProgram(0);
    }
}

void DebugDrawer::drawSphere(const btVector3 &p, btScalar radius,
                             const btVector3 &color)
{
}

void DebugDrawer::drawTriangle(const btVector3 &a, const btVector3 &b,
                               const btVector3 &c, const btVector3 &color,
                               btScalar alpha)
{
}

void DebugDrawer::drawContactPoint(const btVector3 &PointOnB,
                                   const btVector3 &normalOnB,
                                   btScalar distance,
                                   int lifeTime,
                                   const btVector3 &color)
{

}

void DebugDrawer::reportErrorWarning(const char *warningString)
{
    lprintf(LOG_WARNING, "%s", warningString);
}

void DebugDrawer::draw3dText(const btVector3 &location,
                             const char *textString)
{

}

void DebugDrawer::setDebugMode(int debugMode)
{
    this->debugMode = debugMode;
}

int DebugDrawer::getDebugMode() const
{
    return debugMode;
}

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

    dynamics_world->setDebugDrawer(&debugDrawer);
    debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);

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

    btTriangleIndexVertexArray *mesh = new btTriangleIndexVertexArray(
        m->num_tris,
        reinterpret_cast<int *>(m->indices),
        12,
        m->num_verts,
        &m->verts[0],
        sizeof(btScalar) * 3);

    btVector3 localInertia(0, 0, 0);
    btCollisionShape *shape;

    if(mass > 0.f) {
        btConvexShape *tmpshape = new btConvexTriangleMeshShape(mesh);

        btShapeHull *hull = new btShapeHull(tmpshape);
        btScalar margin = tmpshape->getMargin();
        hull->buildHull(margin);
        tmpshape->setUserPointer(hull);

        btConvexHullShape *hullshape = new btConvexHullShape();

        btVector3 *points = const_cast<btVector3 *>(hull->getVertexPointer());

        for(int i = 0; i < hull->numVertices(); i++) {
            hullshape->addPoint(points[i]);
        }

        shape = hullshape;
        shape->calculateLocalInertia(mass, localInertia);
    } else {
        shape = new btBvhTriangleMeshShape(mesh, true);
    }

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        motionstate,
        shape,
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
    //dynamics_world->debugDrawWorld();
}
