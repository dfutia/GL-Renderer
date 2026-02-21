#ifndef ACTOR_H
#define ACTOR_H

#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "ActorComponent.h"

class Actor
{
public:
	Actor() = default;
	~Actor() = default;

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of<ActorComponent, T>::value, "T must derive from Component");

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        ptr->owner = this;

        components[std::type_index(typeid(T))] = std::move(component);
        return ptr;
    }

    template<typename T>
    T* GetComponent()
    {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end())
            return static_cast<T*>(it->second.get());
        return nullptr;
    }

    template<typename T>
    bool HasComponent() const
    {
        return components.find(std::type_index(typeid(T))) != components.end();
    }

    template<typename T>
    void RemoveComponent()
    {
        components.erase(std::type_index(typeid(T)));
    }
private:
    std::unordered_map<std::type_index, std::unique_ptr<ActorComponent>> components;
};

#endif 