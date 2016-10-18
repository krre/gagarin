#include "PhisicsSystem.h"
#include "../Components/PhisicsComponent.h"

PhisicsSystem::PhisicsSystem() {
    type = SystemType::Phisics;

    collisionConfiguration.reset(new btDefaultCollisionConfiguration);
    dispatcher.reset(new btCollisionDispatcher(collisionConfiguration.get()));
    overlappingPairCache.reset(new btDbvtBroadphase);
    solver.reset(new btSequentialImpulseConstraintSolver);

    dynamicsWorld.reset(new btDiscreteDynamicsWorld(dispatcher.get(), overlappingPairCache.get(), solver.get(), collisionConfiguration.get()));
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void PhisicsSystem::process(float dt) {
    dynamicsWorld->stepSimulation(dt, 10);
}

void PhisicsSystem::addRigidBody(Entity* entity) {
    PhisicsComponent* pc = static_cast<PhisicsComponent*>(entity->components[ComponentType::Phisics].get());
    dynamicsWorld->addRigidBody(pc->rigidBody.get());
}
