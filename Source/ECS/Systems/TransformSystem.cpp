#include "TransformSystem.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/EntityManager.h"
#include <glm/gtx/matrix_decompose.hpp>

TransformSystem::TransformSystem() {
    type = SystemType::Transform;
}

void TransformSystem::process(float dt) {
    for (auto& entity: entityManager->getEntities()) {
        update(entity.second.get());
    }
}

void TransformSystem::update(Entity* entity) {
    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    if (tc && tc->dirty) {
        glm::mat4 translationMatrix = glm::translate(tc->position);
        glm::mat4 rotationMatrix = glm::toMat4(tc->rotation);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(tc->scale));
        tc->objectToWorld = translationMatrix * rotationMatrix * scaleMatrix;
        tc->worldToObject = glm::inverse(tc->objectToWorld);
        tc->dirty = false;
    }
}

void TransformSystem::setPosition(Entity* entity, const glm::vec3& position) {
    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    tc->position = position;
    tc->dirty = true;
}

void TransformSystem::setRotation(Entity* entity, float angle, const glm::vec3& axis) {
    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    tc->rotation = glm::toQuat(glm::rotate(glm::mat4(1.0f), angle, axis));
    tc->dirty = true;
}

void TransformSystem::setRotation(Entity* entity, const glm::quat& rotation) {
    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    tc->rotation = rotation;
    tc->dirty = true;
}

void TransformSystem::setScale(Entity* entity, float scale) {
    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    tc->scale = scale;
    tc->dirty = true;
}

void TransformSystem::setPitch(Entity* entity, float pitch) {
    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    tc->pitch = pitch;
    tc->dirty = true;
}

void TransformSystem::translate(Entity* entity, const glm::vec3& delta, bool local) {
    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    if (local) {
        tc->position += tc->rotation * delta;
    } else {
        tc->position += delta;
    }

    tc->dirty = true;
}

void TransformSystem::rotate(Entity* entity, const glm::quat& delta) {
    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    tc->rotation *= delta;
    tc->dirty = true;
}

void TransformSystem::scale(Entity* entity, float delta) {
    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    tc->scale *= delta;
    tc->dirty = true;
}

void TransformSystem::lookAt(Entity* entity, const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
    glm::mat4 m = glm::lookAt(eye, center, up);

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(glm::inverse(m), scale, rotation, translation, skew, perspective);

    TransformComponent* tc = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
    tc->position = translation;
    tc->rotation = rotation;
    tc->dirty = true;
}
