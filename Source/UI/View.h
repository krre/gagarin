#pragma once
#include "Control.h"

namespace Vulkan {
    class Device;
}

class Scene;

class View : public Control {

public:
    View();
    ~View();
    void update(float dt);
    void draw(float dt);
    void render();
    Scene* getScene() const { return scene.get(); }
    void resize(uint32_t width, uint32_t height);
    const std::vector<View*> getInnerViews();

protected:
    Vulkan::Device* device;

private:
    std::vector<View*> innerViews;
    std::unique_ptr<Scene> scene;
};
