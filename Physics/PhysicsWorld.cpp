#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld()
{
    collisionConfig = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfig);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(
        dispatcher, broadphase, solver, collisionConfig
    );

    dynamicsWorld->setGravity(btVector3(0, -9.81f * 100.0f, 0)); // Scaled for your scene
}

PhysicsWorld::~PhysicsWorld()
{
    // Remove all rigid bodies
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
            delete body->getMotionState();
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    delete dynamicsWorld;
    delete solver;
    delete broadphase;
    delete dispatcher;
    delete collisionConfig;
}

void PhysicsWorld::Update(float deltaTime)
{
    dynamicsWorld->stepSimulation(deltaTime, 10);
}

void PhysicsWorld::SetGravity(float x, float y, float z)
{
    dynamicsWorld->setGravity(btVector3(x, y, z));
}

void PhysicsWorld::AddRigidBody(btRigidBody* body)
{
    dynamicsWorld->addRigidBody(body);
}

void PhysicsWorld::RemoveRigidBody(btRigidBody* body)
{
    dynamicsWorld->removeRigidBody(body);
}