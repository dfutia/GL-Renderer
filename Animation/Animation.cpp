#include <print>

#include "Animation.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Animator::Animator(const Skeleton& skeleton)
    : skeleton(skeleton)
{
    int boneCount = skeleton.bones.size();
    localTransforms.resize(boneCount, glm::mat4(1.0f));
    worldTransforms.resize(boneCount, glm::mat4(1.0f));
    finalMatrices.resize(boneCount, glm::mat4(1.0f));

    std::vector<bool> processed(boneCount, false);
    processOrder.reserve(boneCount);

    while (processOrder.size() < boneCount)
    {
        for (int i = 0; i < boneCount; i++)
        {
            if (processed[i])
                continue;

            int parent = skeleton.bones[i].parentIndex;
            if (parent == -1 || processed[parent])
            {
                processOrder.push_back(i);
                processed[i] = true;
            }
        }
    }
}

void Animator::Play(const Animation* animation)
{
    currentAnimation = animation;
    currentTime = 0.0f;
}

void Animator::Update(float deltaTime)
{
    if (!currentAnimation)
        return;

    currentTime += deltaTime * currentAnimation->ticksPerSecond;
    if (currentTime > currentAnimation->duration)
        currentTime = fmod(currentTime, currentAnimation->duration);

    // Step 1: Compute local transform for each bone from keyframes
    for (int i = 0; i < skeleton.bones.size(); i++)
    {
        const BoneAnimation* channel = currentAnimation->FindChannel(skeleton.bones[i].name);

        if (channel)
        {
            glm::vec3 position = InterpolatePosition(*channel, currentTime);
            glm::quat rotation = InterpolateRotation(*channel, currentTime);
            glm::vec3 scale = InterpolateScale(*channel, currentTime);

            glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
            glm::mat4 R = glm::toMat4(rotation);
            glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

            localTransforms[i] = T * R * S;
        }
        else
        {
            // Use the default bind pose transform instead of identity
            localTransforms[i] = skeleton.bones[i].localBindTransform;
        }
    }

    // Step 2: Walk hierarchy in correct order to build world transforms
    for (int i : processOrder)
    {
        int parent = skeleton.bones[i].parentIndex;
        if (parent == -1)
            worldTransforms[i] = localTransforms[i];
        else
            worldTransforms[i] = worldTransforms[parent] * localTransforms[i];
    }

    // Step 3: Multiply by offset matrix to get final matrices
    for (int i = 0; i < skeleton.bones.size(); i++)
    {
        finalMatrices[i] = worldTransforms[i] * skeleton.bones[i].offsetMatrix;
    }
}

glm::vec3 Animator::InterpolatePosition(const BoneAnimation& channel, float time)
{
    auto& keys = channel.positionKeys;
    if (keys.size() == 1)
        return keys[0].value;

    int index = 0;
    for (int i = 0; i < keys.size() - 1; i++)
    {
        if (time < keys[i + 1].time)
        {
            index = i;
            break;
        }
    }

    float t1 = keys[index].time;
    float t2 = keys[index + 1].time;
    float factor = (time - t1) / (t2 - t1);

    return glm::mix(keys[index].value, keys[index + 1].value, factor);
}

glm::quat Animator::InterpolateRotation(const BoneAnimation& channel, float time)
{
    auto& keys = channel.rotationKeys;
    if (keys.size() == 1)
        return keys[0].value;

    int index = 0;
    for (int i = 0; i < keys.size() - 1; i++)
    {
        if (time < keys[i + 1].time)
        {
            index = i;
            break;
        }
    }

    float t1 = keys[index].time;
    float t2 = keys[index + 1].time;
    float factor = (time - t1) / (t2 - t1);

    return glm::slerp(keys[index].value, keys[index + 1].value, factor);
}

glm::vec3 Animator::InterpolateScale(const BoneAnimation& channel, float time)
{
    auto& keys = channel.scaleKeys;
    if (keys.size() == 1)
        return keys[0].value;

    int index = 0;
    for (int i = 0; i < keys.size() - 1; i++)
    {
        if (time < keys[i + 1].time)
        {
            index = i;
            break;
        }
    }

    float t1 = keys[index].time;
    float t2 = keys[index + 1].time;
    float factor = (time - t1) / (t2 - t1);

    return glm::mix(keys[index].value, keys[index + 1].value, factor);
}