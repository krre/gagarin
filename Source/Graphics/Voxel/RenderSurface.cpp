#include "RenderSurface.h"
#include "../../Resource/ResourceManager.h"
#include "Octree.h"
#include "../../Core/App.h"
#include "../../ECS/Components/Components.h"
#include "../../ECS/Systems/CameraSystem.h"
#include "../../ECS/Engine.h"
#include "../../Core/Common.h"
#include "../../Core/Utils.h"
#include <glm/gtx/matrix_decompose.hpp>

RenderSurface::RenderSurface() {
    voxelShaderGroup = ResourceManager::getInstance()->getShaderGroup("VoxelShaderGroup");
    program = voxelShaderGroup->getProgram();
    voxelShaderGroup->bind();

    glm::vec4 bgColor = App::getInstance()->getViewport()->getBackgroundColor();

    glUniform1i(glGetUniformLocation(program, "pageBytes"), pageBytes);
    glUniform1i(glGetUniformLocation(program, "blockInfoEnd"), blockInfoEnd);
    glUniform3fv(glGetUniformLocation(program, "backgroundColor"), 1, &bgColor[0]);
    glUniform1i(glGetUniformLocation(program, "objectStride"), OBJECT_STRIDE);
    glUniform1i(glGetUniformLocation(program, "objects"), 0);
    glUniform1i(glGetUniformLocation(program, "octrees"), 1);

    GLfloat vertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,

        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);


    // Objects buffer
    glGenBuffers(1, &objectsTbo);
    glBindBuffer(GL_TEXTURE_BUFFER, objectsTbo);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4) * OBJECT_STRIDE * MAX_OCTREE_COUNT, NULL, GL_DYNAMIC_DRAW);

    glGenTextures(1, &objectsTexture);
    glBindTexture(GL_TEXTURE_BUFFER, objectsTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, objectsTbo);

    // Octrees buffer
    glGenBuffers(1, &octreesTbo);
    glBindBuffer(GL_TEXTURE_BUFFER, octreesTbo);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(uint32_t) * MAX_OCTREE_COUNT * 10, NULL, GL_DYNAMIC_DRAW);

    glGenTextures(1, &octreesTexture);
    glBindTexture(GL_TEXTURE_BUFFER, octreesTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA8UI, octreesTbo);
}

void RenderSurface::draw(float dt) {
    int width = App::getInstance()->getWidth();
    int height = App::getInstance()->getHeight();

    Entity* currentCamera = App::getInstance()->getViewport()->getCurrentCamera().get();
    CameraComponent* cameraComp = static_cast<CameraComponent*>(currentCamera->components[ComponentType::Camera].get());
    TransformComponent* cameraTransform = static_cast<TransformComponent*>(currentCamera->components[ComponentType::Transform].get());

    TransformComponent* octreeTransform;

    TransformComponent* lightTransform;
    glm::vec3 lightColor = glm::vec3(0.0);
    glm::vec3 lightPos = glm::vec3(0.0);

    // TODO: Replace by family
    for (auto entity : Engine::getInstance()->getEntities()) {
        OctreeComponent* octreeComp = static_cast<OctreeComponent*>(entity->components[ComponentType::Octree].get());
        if (octreeComp) {
            octreeTransform = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
        }

        LightComponent* lightComp = static_cast<LightComponent*>(entity->components[ComponentType::Light].get());
        if (lightComp) {
            lightTransform = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
            lightColor = lightComp->color;
            lightPos = glm::vec3(lightTransform->objectToWorld[3]);
        }
    }

    // TODO: Replace by family
    std::vector<glm::vec4> objects;
    int objectCount = 0;
    for (auto entity : Engine::getInstance()->getEntities()) {
        OctreeComponent* octreeComp = static_cast<OctreeComponent*>(entity->components[ComponentType::Octree].get());
        if (octreeComp) {
            TransformComponent* octreeTransform = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
            objects.push_back(octreeTransform->objectToWorld[0]);
            objects.push_back(octreeTransform->objectToWorld[1]);
            objects.push_back(octreeTransform->objectToWorld[2]);
            objects.push_back(octreeTransform->objectToWorld[3]);

            glm::mat4 cameraToOctree = octreeTransform->worldToObject * cameraTransform->objectToWorld;

            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(cameraToOctree, scale, rotation, translation, skew, perspective);

            objects.push_back(glm::vec4(translation.x, translation.y, translation.z, 1.0));

            glm::vec3 up = cameraComp->up * rotation;
            glm::vec3 look = cameraComp->look * rotation;
            glm::vec3 right = cameraComp->right * rotation;

            // Ray calculation is based on Johns Hopkins presentation:
            // http://www.cs.jhu.edu/~cohen/RendTech99/Lectures/Ray_Casting.bw.pdf
            glm::vec3 h0 = look - up * glm::tan(cameraComp->fov); // min height vector
            glm::vec3 h1 = look + up * glm::tan(cameraComp->fov); // max height vector
            glm::vec3 stepH = (h1 - h0) / height;
            h0 += stepH / 2;

            glm::vec3 w0 = look - right * glm::tan(cameraComp->fov) * width / height; // min width vector
            glm::vec3 w1 = look + right * glm::tan(cameraComp->fov) * width / height; // max width vector
            glm::vec3 stepW = (w1 - w0) / width;
            w0 += stepW / 2;

            glm::vec3 startCornerPos = w0 + h0;

            objects.push_back(glm::vec4(startCornerPos.x, startCornerPos.y, startCornerPos.z, 0.0));
            objects.push_back(glm::vec4(stepW.x, stepW.y, stepW.z, 0.0));
            objects.push_back(glm::vec4(stepH.x, stepH.y, stepH.z, 0.0));

            objectCount++;
        }
    }

    glBindBuffer(GL_TEXTURE_BUFFER, objectsTbo);
    glBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(glm::vec4) * objects.size(), objects.data());

    voxelShaderGroup->bind();

    glUniform3fv(glGetUniformLocation(program, "lightColor"), 1, &lightColor[0]);
    glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, &lightPos[0]);
    glUniform1f(glGetUniformLocation(program, "ambientStrength"), 0.1f);
    glUniform1i(glGetUniformLocation(program, "objectCount"), objectCount);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, objectsTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, octreesTexture);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderSurface::sendOctreeToGPU(int offset, const std::vector<uint32_t>& data) {
    glBindBuffer(GL_TEXTURE_BUFFER, octreesTbo);
    glBufferSubData(GL_TEXTURE_BUFFER, offset, sizeof(uint32_t) * data.size(), data.data());

}
