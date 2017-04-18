#pragma once
#include "ECS/System.h"
#include "ECS/Entity.h"

class NodeSystem : public System {

public:
    NodeSystem();
    void process(float dt) override;

    void addChild(EntityId parent, EntityId child);
    void removeChild(EntityId parent, EntityId child);
    void removeChildren(EntityId parent);
};
