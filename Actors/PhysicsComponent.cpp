#include <glm/gtc/type_ptr.hpp>

#include "PhysicsComponent.h"
#include "Physics/PhysicsWorld.h"
#include "Actor.h"
#include "TransformComponent.h"

// Custom motion state that syncs with TransformComponent
class ActorMotionState : public btMotionState
{
public:
    ActorMotionState(Actor* actor) : actor(actor) {}

    void getWorldTransform(btTransform& worldTrans) const override
    {
        auto* transform = actor->GetComponent<TransformComponent>();
        if (transform)
        {
            worldTrans.setOrigin(btVector3(
                transform->position.x,
                transform->position.y,
                transform->position.z
            ));

            btQuaternion rotation;
            rotation.setEulerZYX(
                glm::radians(transform->rotation.z),
                glm::radians(transform->rotation.y),
                glm::radians(transform->rotation.x)
            );
            worldTrans.setRotation(rotation);
        }
        else
        {
            worldTrans.setIdentity();
        }
    }

    void setWorldTransform(const btTransform& worldTrans) override
    {
        auto* transform = actor->GetComponent<TransformComponent>();
        if (transform)
        {
            btVector3 pos = worldTrans.getOrigin();
            transform->position = glm::vec3(pos.x(), pos.y(), pos.z());

            btQuaternion rot = worldTrans.getRotation();
            btScalar yaw, pitch, roll;
            rot.getEulerZYX(yaw, pitch, roll);
            transform->rotation = glm::vec3(
                glm::degrees(roll),
                glm::degrees(pitch),
                glm::degrees(yaw)
            );
        }
    }

private:
    Actor* actor;
};

PhysicsComponent::PhysicsComponent(PhysicsWorld& world)
    : world(world)
{
}

PhysicsComponent::~PhysicsComponent()
{
    if (rigidBody)
    {
        world.RemoveRigidBody(rigidBody);
        delete rigidBody;
    }
    delete motionState;
    delete shape;
}

void PhysicsComponent::SetBoxShape(const glm::vec3& halfExtents)
{
    delete shape;
    shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
    CreateRigidBody();
}

void PhysicsComponent::SetSphereShape(float radius)
{
    delete shape;
    shape = new btSphereShape(radius);
    CreateRigidBody();
}

void PhysicsComponent::SetCapsuleShape(float radius, float height)
{
    delete shape;
    shape = new btCapsuleShape(radius, height);
    CreateRigidBody();
}

void PhysicsComponent::SetStaticPlaneShape(const glm::vec3& normal, float constant)
{
    delete shape;
    shape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), constant);
    mass = 0.0f; // Static objects have zero mass
    CreateRigidBody();
}

void PhysicsComponent::CreateRigidBody()
{
    if (!shape || !owner)
        return;

    // Clean up existing body
    if (rigidBody)
    {
        world.RemoveRigidBody(rigidBody);
        delete rigidBody;
        delete motionState;
    }

    // Create motion state
    motionState = new ActorMotionState(owner);

    // Calculate inertia
    btVector3 localInertia(0, 0, 0);
    if (mass > 0.0f)
        shape->calculateLocalInertia(mass, localInertia);

    // Create rigid body
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    rigidBody = new btRigidBody(rbInfo);

    // Add to world
    world.AddRigidBody(rigidBody);
}

void PhysicsComponent::ApplyForce(const glm::vec3& force)
{
    if (rigidBody)
    {
        rigidBody->activate();
        rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
    }
}

void PhysicsComponent::ApplyImpulse(const glm::vec3& impulse)
{
    if (rigidBody)
    {
        rigidBody->activate();
        rigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
    }
}

void PhysicsComponent::SetLinearVelocity(const glm::vec3& velocity)
{
    if (rigidBody)
    {
        rigidBody->activate();
        rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
    }
}

void PhysicsComponent::SetKinematic(bool kinematic)
{
    if (rigidBody)
    {
        if (kinematic)
        {
            rigidBody->setCollisionFlags(
                rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT
            );
            rigidBody->setActivationState(DISABLE_DEACTIVATION);
        }
        else
        {
            rigidBody->setCollisionFlags(
                rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT
            );
            rigidBody->setActivationState(ACTIVE_TAG);
        }
    }
}