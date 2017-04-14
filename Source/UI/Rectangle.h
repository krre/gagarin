#pragma once
#include "Control.h"
#include "../Graphics/Color.h"

class Rectangle : public Control {

public:
    Rectangle(const Size& size);
    Rectangle(const Pos2& position, const Size& size);

    void setColor(const Color& color);
    const Color& getColor() const { return color; }

    void setBorderColor(const Color& borderColor);
    const Color& getBorderColor() const { return borderColor; }

    void setBorderWidth(uint32_t borderWidth);
    uint32_t getBorderWidth() const { return borderWidth; }

    void draw(float dt) override;
    void update(float dt) override;

private:
    Color color = { 1.0, 1.0, 1.0 };
    Color borderColor = { 0.5, 0.5, 0.5 };
    uint32_t borderWidth = 0;
};
