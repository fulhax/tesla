#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <glm/glm.hpp>

#include "script.hpp"
#include "physics.hpp"

enum {
    FRUSTUM_OUTSIDE = 0,
    FRUSTUM_INTERSECT,
    FRUSTUM_INSIDE
};

class Camera
{
    enum {
        FRUSTUM_TOP = 0,
        FRUSTUM_BOTTOM,
        FRUSTUM_LEFT,
        FRUSTUM_RIGHT,
        FRUSTUM_NEAR,
        FRUSTUM_FAR
    };

    float pointDistance(int i, const glm::vec3 &point);
    glm::vec4 frustum[6];
    btKinematicCharacterController* character;
    glm::vec3 vel;
public:
    glm::vec3 pos;

    float yaw;
    float pitch;

    Camera();
    virtual ~Camera();

    void update(glm::mat4 projMat, glm::mat4 viewMat);

    int pointInFrustum(const glm::vec3 &point);
    int sphereInFrustum(const glm::vec3 &point, float radius);
    int cubeInFrustum(const glm::vec3 &point, float scale);
    int rectInFrustum(const glm::vec3 &point, const glm::vec3 &scale);

    void attachCharacter(btKinematicCharacterController *c);
    void detachCharacter();

    void moveForward(const float &speed);
    void moveBackwards(const float &speed);
    void moveLeft(const float &speed);
    void moveRight(const float &speed);
};

#endif // CAMERA_HPP_
