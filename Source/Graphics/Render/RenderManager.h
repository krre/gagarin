#pragma once
#include "Core/Singleton.h"
#include "Graphics/Color.h"

class RenderWindow;

class RenderManager : public Singleton<RenderManager> {

public:
    RenderManager();
    virtual std::shared_ptr<RenderWindow> createRenderWindow() = 0;
};
