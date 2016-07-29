#include "camera.hpp"

#include <glm/gtc/matrix_access.hpp>

#include <engine.hpp>

Camera::Camera()
{
    pos = glm::vec3(0.f, 0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);

    yaw = 0;
    pitch = 0;
    character = nullptr;
}

Camera::~Camera()
{

}

void Camera::attachCharacter(btKinematicCharacterController *c)
{
    character = c;
    // character->setFallSpeed(55);
    // character->setJumpSpeed(20);
    //character->setMaxJumpHeight(20);
}

void Camera::detachCharacter()
{
    character = nullptr;
}

void Camera::update(glm::mat4 projMat, glm::mat4 viewMat)
{
    if (character) {
        // character->setJumpSpeed(10);
        // character->setFallSpeed(55);
        //character->setMaxJumpHeight(30);

        vel *= engine.getTick();
        character->setWalkDirection(
            btVector3(
                vel.x,
                vel.y,
                vel.z
            )
        );

        // character->setVelocityForTimeInterval(
        //     btVector3(
        //         vel.x,
        //         vel.y,
        //         vel.z
        //     ), engine.getTick()
        // );

        // engine.physics.updateCharacter(character);

        btTransform trans;
        trans = character->getGhostObject()->getWorldTransform();

        pos = glm::vec3(
                  trans.getOrigin().x(),
                  trans.getOrigin().y() + 0.5f,
                  trans.getOrigin().z()
              );

    } else {
        pos += vel * engine.getTick();
    }

    vel = glm::vec3(0.f, 0.f, 0.f);

    glm::mat4 mpv = projMat * viewMat;

    glm::vec4 rowX = glm::row(mpv, 0);
    glm::vec4 rowY = glm::row(mpv, 1);
    glm::vec4 rowZ = glm::row(mpv, 2);
    glm::vec4 rowW = glm::row(mpv, 3);

    frustum[FRUSTUM_RIGHT]  = glm::normalize(rowW + rowX);
    frustum[FRUSTUM_LEFT]   = glm::normalize(rowW - rowX);
    frustum[FRUSTUM_TOP]    = glm::normalize(rowW + rowY);
    frustum[FRUSTUM_BOTTOM] = glm::normalize(rowW - rowY);
    frustum[FRUSTUM_FAR]    = glm::normalize(rowW + rowZ);
    frustum[FRUSTUM_NEAR]   = glm::normalize(rowW - rowZ);

    for (int i = 0; i < 6; i++) {
        float len = glm::length(frustum[i].xyz());
        frustum[i] /= len;
    }
}

void Camera::jump()
{
    if (character) {
        character->jump();
    }
}

void Camera::moveForward(const float &speed)
{
    float yrad = yaw * RAD;
    //float xrad = pitch * RAD;

    glm::vec3 cam = glm::vec3(0.f, 0.f, 0.f);

    cam.x += sinf(yrad);
    //cam.y -= sinf(xrad);
    cam.z -= cosf(yrad);

    vel += glm::normalize(cam) * speed;
}

void Camera::moveBackwards(const float &speed)
{
    float yrad = yaw * RAD;
    //float xrad = pitch * RAD;

    glm::vec3 cam = glm::vec3(0.f, 0.f, 0.f);

    cam.x -= sinf(yrad);
    //cam.y += sinf(xrad);
    cam.z += cosf(yrad);

    vel += glm::normalize(cam) * speed;
}

void Camera::moveLeft(const float &speed)
{
    float yrad = yaw * RAD;

    glm::vec3 cam = glm::vec3(0.f, 0.f, 0.f);

    cam.x -= cosf(yrad);
    cam.z -= sinf(yrad);

    vel += glm::normalize(cam) * speed;
}

void Camera::moveRight(const float &speed)
{
    float yrad = yaw * RAD;

    glm::vec3 cam = glm::vec3(0.f, 0.f, 0.f);

    cam.x += cosf(yrad);
    cam.z += sinf(yrad);

    vel += glm::normalize(cam) * speed;
}

int Camera::pointInFrustum(const glm::vec3 &point)
{
    for (int i = 0; i < 6; i++) {
        if (pointDistance(i, point) <= 0) {
            return FRUSTUM_OUTSIDE;
        }
    }

    return FRUSTUM_INSIDE;
}

int Camera::sphereInFrustum(const glm::vec3 &point, float radius)
{
    for (int i = 0; i < 6; i++) {
        float dist = pointDistance(i, point);

        if (dist < -radius) {
            return FRUSTUM_OUTSIDE;
        }

        if (fabs(dist) < radius) {
            return FRUSTUM_INTERSECT;
        }
    }

    return FRUSTUM_INSIDE;
}

int Camera::cubeInFrustum(const glm::vec3 &point, float scale)
{
    return rectInFrustum(point, glm::vec3(scale, scale, scale));
}

int Camera::rectInFrustum(const glm::vec3 &point, const glm::vec3 &scale)
{
    int cull = 0;

    for (int i = 0; i < 6; i++) {
        int cube = 0;

        if (pointDistance(i, glm::vec3(
                              point.x - scale.x,
                              point.y - scale.y,
                              point.z - scale.z
                          )) > 0) { // 0 0 0
            cube++;
        }

        if (pointDistance(i, glm::vec3(
                              point.x + scale.x,
                              point.y - scale.y,
                              point.z - scale.z
                          )) > 0) { // 1 0 0
            cube++;
        }

        if (pointDistance(i, glm::vec3(
                              point.x - scale.x,
                              point.y + scale.y,
                              point.z - scale.z
                          )) > 0) { // 0 1 0
            cube++;
        }

        if (pointDistance(i, glm::vec3(
                              point.x + scale.x,
                              point.y + scale.y,
                              point.z - scale.z
                          )) > 0) { // 1 1 0
            cube++;
        }

        if (pointDistance(i, glm::vec3(
                              point.x - scale.x,
                              point.y - scale.y,
                              point.z + scale.z
                          )) > 0) { // 0 0 1
            cube++;
        }

        if (pointDistance(i, glm::vec3(
                              point.x + scale.x,
                              point.y - scale.y,
                              point.z + scale.z
                          )) > 0) { // 1 0 1
            cube++;
        }

        if (pointDistance(i, glm::vec3(
                              point.x - scale.x,
                              point.y + scale.y,
                              point.z + scale.z
                          )) > 0) { // 0 1 1
            cube++;
        }

        if (pointDistance(i, glm::vec3(
                              point.x + scale.x,
                              point.y + scale.y,
                              point.z + scale.z
                          )) > 0) { // 1 1 1
            cube++;
        }

        if (cube == 0) {
            return FRUSTUM_OUTSIDE;
        }

        if (cube == 8) {
            cull++;
        }
    }

    return ((cull == 6) ? FRUSTUM_INSIDE : FRUSTUM_INTERSECT);
}

float Camera::pointDistance(int i, const glm::vec3 &point)
{
    return glm::dot(frustum[i].xyz(), point) + frustum[i].w;
}
