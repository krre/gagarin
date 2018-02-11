#pragma once
#include "Screen.h"

struct SDL_KeyboardEvent;

namespace Origin {

class GameScreen : public Screen {

public:
    GameScreen(const std::string& worldName);

private:
    void keyPressed(const SDL_KeyboardEvent& event);

};

} // Origin
