#pragma once
#include "../Core/Object.h"

enum class ComponentType {
    Camera,
    Empty,
    Input,
    Node,
    Render,
    Transform,
    Movement,
    Mesh,
    Material
};

class Component : public Object {

public:
    Component();
    ComponentType getType() const { return type; }
    bool enable = true;
    bool dirty = false;

protected:
    ComponentType type = ComponentType::Empty;
};
