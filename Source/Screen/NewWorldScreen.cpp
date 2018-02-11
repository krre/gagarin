#include "NewWorldScreen.h"
#include "GameScreen.h"
#include "UI/Button.h"
#include "Core/Game.h"
#include "Core/Window.h"
#include "UI/LinearLayout.h"
#include "UI/TextEdit.h"
#include "World/World.h"

namespace Origin {

NewWorldScreen::NewWorldScreen() {
    layout = new LinearLayout(LinearLayout::Direction::Vertical, this);

    textEdit = new TextEdit();
    layout->addControl(textEdit);

    Button* buttonPlay = new Button("Play", this);
    buttonPlay->clicked.connect([&]() {
        if (!textEdit->getText().empty()) {
            Game::getWorld()->create(textEdit->getText());
            Game::getWindow()->setScreen(std::make_shared<GameScreen>(textEdit->getText()));
        }
    });
    layout->addControl(buttonPlay);

    buttonBack = new Button("Back", this);
    buttonBack->clicked.connect([&]() {
        Game::getWindow()->popScreen();
    });

    setActiveControl(textEdit);
}

void NewWorldScreen::resizeImpl(int width, int height) {
    layout->move((width - layout->getContentWidth()) / 2, (height - layout->getContentHeight()) / 2);
    buttonBack->move(width - buttonBack->getSize().width, height - buttonBack->getSize().height);
}

} // Origin
