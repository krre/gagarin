#pragma once
#include "../Component.h"
#include <glm/ext.hpp>

using namespace glm;

class TransformComponent : public Component {

public:
    TransformComponent();
    mat4 localMatrix;
    mat4 worldMatrix;
    vec3 position;
    quat rotation;
    float scale = 1.0;
    float yaw = 0;
    float pitch = 0;
    bool dirty = false;
};
