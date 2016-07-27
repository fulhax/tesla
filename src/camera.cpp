#include "camera.hpp"

#include <glm/gtc/matrix_access.hpp>

#include <engine.hpp>

Camera::Camera()
{
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    yaw = 0;
    pitch = 0;
}

Camera::~Camera()
{

}

void Camera::update(glm::mat4 projMat, glm::mat4 viewMat)
{
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

void Camera::moveForward(const float &speed)
{
    float yrad = yaw * RAD;
    float xrad = pitch * RAD;

    float s = speed * engine.getTick();

    pos.x += sinf(yrad) * s;
    pos.y -= sinf(xrad) * s;
    pos.z -= cosf(yrad) * s;

    printf("camera: %f,%f,%f, %f\n", pos.x, pos.y, pos.z, s);
}

void Camera::moveBackwards(const float &speed)
{
    float yrad = yaw * RAD;
    float xrad = pitch * RAD;

    float s = speed * engine.getTick();

    pos.x -= sinf(yrad) * s;
    pos.y += sinf(xrad) * s;
    pos.z += cosf(yrad) * s;
}

void Camera::moveLeft(const float &speed)
{
    float yrad = yaw * RAD;

    float s = speed * engine.getTick();

    pos.x -= cosf(yrad) * s;
    pos.z -= sinf(yrad) * s;
}

void Camera::moveRight(const float &speed)
{
    float yrad = yaw * RAD;

    float s = speed * engine.getTick();

    pos.x += cosf(yrad) * s;
    pos.z += sinf(yrad) * s;
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
