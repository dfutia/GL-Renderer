#ifndef ACTOR_COMPONENT_H
#define ACTOR_COMPONENT_H

#include "Properties.h"

class Actor;

class ActorComponent
{
public:
	ActorComponent() = default;
	virtual ~ActorComponent() = default;

	virtual const char* GetName() const { return "Component"; }
	virtual void RegisterProperties(PropertyRegistry& registry) {}
	virtual void OnUpdate(float deltaTime) {}
	virtual void OnInspectorGUI() {}

	Actor* GetOwner() const { return owner; }
protected:
	Actor* owner = nullptr;
	friend class Actor;
};

#endif 