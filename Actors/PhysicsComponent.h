#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "ActorComponent.h"

class PhysicsWorld;

class PhysicsComponent : public ActorComponent
{
public:
    PhysicsComponent(PhysicsWorld& world);
    ~PhysicsComponent();

    // Shape configuration (call one of these to create the body)
    void SetBoxShape(const glm::vec3& halfExtents);
    void SetSphereShape(float radius);
    void SetCapsuleShape(float radius, float height);
    void SetStaticPlaneShape(const glm::vec3& normal, float constant);

    // Mass (set before calling a shape function, default is 1.0)
    void SetMass(float mass) { this->mass = mass; }

    // Runtime physics control
    void ApplyForce(const glm::vec3& force);
    void ApplyImpulse(const glm::vec3& impulse);
    void SetLinearVelocity(const glm::vec3& velocity);
    void SetKinematic(bool kinematic);

    btRigidBody* GetRigidBody() { return rigidBody; }

private:
    void CreateRigidBody();

    PhysicsWorld& world;
    btRigidBody* rigidBody = nullptr;
    btCollisionShape* shape = nullptr;
    btMotionState* motionState = nullptr;
    float mass = 1.0f;
};

#endif 