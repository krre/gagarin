#pragma once
#include "../Core/Singleton.h"
#include "../Graphics/Drawable.h"
#include "../UI/Text.h"
#include <SDL.h>

class Console : public Singleton<Console>, public Drawable {

public:
    Console();
    void draw(float dt) override;
    void setVisible(bool visible) override;

private:
    void keyPress(const SDL_KeyboardEvent& event);
    void execute();
    Text cmdLine;
    int keyPressId = 0;
};
