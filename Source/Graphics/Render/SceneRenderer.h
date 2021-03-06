#pragma once
#include "Renderer.h"

namespace Origin {

class View3D;
class OctreeRenderer;
class View3DBatch;

class SceneRenderer : public Renderer {

public:
    SceneRenderer(Object* parent = nullptr);
    ~SceneRenderer();

    void writeCommandBuffer(Vulkan::CommandBuffer* commandBuffer, Vulkan::Framebuffer* framebuffer) override;

    static SceneRenderer* get();

    void draw() override;
    bool getActive() const override;

    void addView(View3D* view);
    void clearViews();

private:
    std::vector<View3D*> views;
    std::vector<View3DBatch> batches;
    OctreeRenderer* octreeRenderer;
};

} // Origin
