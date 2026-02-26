#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>

#include "Rendering/RenderQueue.h"

class Actor;
class Camera;
class PhysicsWorld;
struct DirectionalLight;
struct Skybox;

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    Actor* CreateActor();
    void DestroyActor(Actor* actor);

    template<typename Func>
    void ForEachActor(Func&& func)
    {
        for (auto& actor : actors)
            func(actor.get());
    }

    void Update(float deltaTime);
    void GatherRenderables(RenderBatch& batch, const glm::vec3& cameraPosition);

    Camera* mainCamera = nullptr;
    DirectionalLight* mainLight = nullptr;
    Skybox* skybox = nullptr;
    PhysicsWorld* physics = nullptr;

private:
    std::vector<std::unique_ptr<Actor>> actors;
};

#endif