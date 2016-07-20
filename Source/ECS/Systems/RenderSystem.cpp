#include "RenderSystem.h"
#include "../../Debug/Console.h"
#include "../../Debug/DebugHUD.h"
#include "../Core/App.h"
#include "../UI/Toast.h"
#include "../ECS/Engine.h"
#include "../ECS/Components/Components.h"

RenderSystem::RenderSystem() {
    type = System::Type::Render;
    // Order important!
    // From bottom layer to top
    drawables.push_back(Console::getInstance());
    drawables.push_back(DebugHUD::getInstance());
    drawables.push_back(Toast::getInstance());
}

void RenderSystem::process(float dt) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // TODO: Replace by family
    for (auto entity : engine->getEntities()) {
        RenderComponent* renderComp = static_cast<RenderComponent*>(entity->components[Component::Type::Render].get());
        if (renderComp && renderComp->visible) {
            renderer.render(entity.get());
        }
    }

    for (auto drawable : drawables) {
        if (drawable->getVisible()) {
            drawable->draw(dt);
        }
    }
}
