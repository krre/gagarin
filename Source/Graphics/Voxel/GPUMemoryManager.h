#pragma once
#include "../../Core/Common.h"
#include "../../Graphics/OpenGL/ShaderGroup.h"
#include "../../ECS/Entity.h"
#include <GL/glew.h>

constexpr int MEMORY_SIZE = (1 << 20) * 100; // 100 MB

class GPUMemoryManager {

public:
    GPUMemoryManager();
    void addEntity(const Entity* entity);
    void updateEntityOctree(const Entity* entity);
    void updateEntityTransform(const Entity* entity);
    void removeEntity(const Entity* entity);

    void beginBatch();
    void endBatch();

    void bind();
    void release();
    void use();

private:
    ShaderGroup* voxelShaderGroup;
    GLuint program;
    bool batch = false;
    GLuint octreesTbo;
    GLuint octreesTexture;
};
