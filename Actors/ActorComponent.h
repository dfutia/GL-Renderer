#ifndef ACTOR_COMPONENT_H
#define ACTOR_COMPONENT_H

class Actor;

class ActorComponent
{
public:
	ActorComponent() = default;
	virtual ~ActorComponent() = default;

	Actor* GetOwner() const { return owner; }
protected:
	Actor* owner = nullptr;
	friend class Actor;
};

#endif 