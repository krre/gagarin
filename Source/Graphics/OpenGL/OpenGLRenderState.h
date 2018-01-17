#pragma once
#include "Graphics/Render/RenderState.h"

namespace Origin {

class OpenGLRenderState : public RenderState {

public:
    OpenGLRenderState();
    ~OpenGLRenderState();
    void create() override;

private:

};

} // Origin
