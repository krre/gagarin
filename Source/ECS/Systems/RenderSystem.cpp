#include "RenderSystem.h"
#include "../../Debug/Console.h"
#include "../../Debug/DebugHUD.h"
#include "../Core/App.h"
#include "../UI/Toast.h"

RenderSystem::RenderSystem() {
    type = SystemType::Render;
}

void RenderSystem::process(float dt) {
    App::getInstance()->getViewport()->render();

    if (Console::getInstance()->getVisible()) {
        Console::getInstance()->render(dt);
    }

    if (DebugHUD::getInstance()->getVisible()) {
        DebugHUD::getInstance()->render(dt);
    }

    if (Toast::getInstance()->getVisible()) {
        Toast::getInstance()->render(dt);
    }
}
