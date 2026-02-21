#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <memory>

#include <btBulletDynamicsCommon.h>

class PhysicsWorld
{
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void Update(float deltaTime);

    void SetGravity(float x, float y, float z);

    btDiscreteDynamicsWorld* GetWorld() { return dynamicsWorld; }

    void AddRigidBody(btRigidBody* body);
    void RemoveRigidBody(btRigidBody* body);
private:
    btDefaultCollisionConfiguration* collisionConfig;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
};

#endif 