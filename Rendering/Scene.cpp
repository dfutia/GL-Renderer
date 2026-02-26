#include "PCH.h"
#include "Scene.h"

#include "Actors/Actor.h"
#include "Actors/TransformComponent.h"
#include "Actors/ModelComponent.h"
#include "Actors/SkinnedModelComponent.h"
#include "Actors/RenderComponent.h"
#include "Physics/PhysicsWorld.h"

Actor* Scene::CreateActor()
{
    actors.push_back(std::make_unique<Actor>());
    return actors.back().get();
}

void Scene::DestroyActor(Actor* actor)
{
    actors.erase(
        std::remove_if(actors.begin(), actors.end(),
            [actor](const std::unique_ptr<Actor>& a) {
                return a.get() == actor;
            }),
        actors.end());
}

void Scene::Update(float deltaTime)
{
    for (auto& actor : actors)
    {
        actor->Update(deltaTime);
    }

    //if (physics)
    //{
    //    physics->Update(deltaTime);
    //}
}

void Scene::GatherRenderables(RenderBatch& batch, const glm::vec3& cameraPosition)
{
    batch.Clear();

    for (auto& actor : actors)
    {
        auto* transform = actor->GetComponent<TransformComponent>();
        if (!transform)
            continue;

        auto* render = actor->GetComponent<RenderComponent>();
        if (render && !render->visible)
            continue;

        float depth = glm::length(transform->position - cameraPosition);
        int queue = render ? render->renderQueue : RenderQueue::Opaque;
        bool castsShadows = render ? render->castsShadows : true;

        if (auto* model = actor->GetComponent<ModelComponent>())
        {
            Renderable r;
            r.modelMatrix = transform->GetMatrix();
            r.normalMatrix = transform->GetNormalMatrix();
            r.vao = model->mesh.vao;
            r.indexCount = model->mesh.IndexCount();
            r.vertexCount = model->mesh.VertexCount();
            r.material = &model->material;
            r.shader = render ? render->shader : "phong";
            r.shadowShader = render ? render->shadowShader : "depth";
            r.queue = queue;
            r.depth = depth;
            r.castsShadows = castsShadows;

            batch.Add(r);
        }

        if (auto* skinned = actor->GetComponent<SkinnedModelComponent>())
        {
            Renderable r;
            r.modelMatrix = transform->GetMatrix();
            r.normalMatrix = transform->GetNormalMatrix();
            r.vao = skinned->mesh.vao;
            r.indexCount = skinned->mesh.IndexCount();
            r.vertexCount = skinned->mesh.VertexCount();
            r.material = &skinned->material;
            r.shader = "skinned_phong";
            r.shadowShader = "skinned_depth";
            r.queue = queue;
            r.depth = depth;
            r.castsShadows = castsShadows;
            r.boneMatrices = const_cast<std::vector<glm::mat4>*>(
                &skinned->GetBoneMatrices());

            batch.Add(r);
        }
    }

    batch.Sort();
}