#include "Control.h"

Control::Control() {

}

void Control::setPosition(const glm::vec2& position) {
    this->position = position;
}

void Control::setWidth(int width) {
    this->width = width;
}

void Control::setHeght(int height) {
    this->heght = height;
}
