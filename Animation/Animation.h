#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <map>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Bone
{
    std::string name;
    int parentIndex = -1;
    glm::mat4 offsetMatrix{ 1.0f };
    glm::mat4 localBindTransform{ 1.0f }; // default local transform from the node tree
};

struct Skeleton
{
    std::vector<Bone> bones;
    std::unordered_map<std::string, int> boneNameToIndex;

    int FindBone(const std::string& name) const
    {
        auto it = boneNameToIndex.find(name);
        return it != boneNameToIndex.end() ? it->second : -1;
    }
};

struct PositionKey
{
    float time;
    glm::vec3 value;
};

struct RotationKey
{
    float time;
    glm::quat value;
};

struct ScaleKey
{
    float time;
    glm::vec3 value;
};

struct BoneAnimation
{
    std::string boneName;
    std::vector<PositionKey> positionKeys;
    std::vector<RotationKey> rotationKeys;
    std::vector<ScaleKey> scaleKeys;
};

struct Animation
{
    std::string name;
    float duration;
    float ticksPerSecond;
    std::vector<BoneAnimation> channels;
    std::unordered_map<std::string, int> boneNameToChannel;

    const BoneAnimation* FindChannel(const std::string& boneName) const
    {
        // Try exact match first
        auto it = boneNameToChannel.find(boneName);
        if (it != boneNameToChannel.end())
            return &channels[it->second];

        // Try removing the "1" from "mixamorig1:" -> "mixamorig:"
        std::string altName = boneName;
        size_t pos = altName.find("mixamorig1:");
        if (pos != std::string::npos)
        {
            altName.replace(pos, 11, "mixamorig:");
            it = boneNameToChannel.find(altName);
            if (it != boneNameToChannel.end())
                return &channels[it->second];
        }

        return nullptr;
    }
};

class Animator
{
public:
    Animator(const Skeleton& skeleton);

    void Play(const Animation* animation);
    void Update(float deltaTime);

    const std::vector<glm::mat4>& GetFinalBoneMatrices() const { return finalMatrices; }
private:
    Skeleton skeleton;
    const Animation* currentAnimation = nullptr;
    float currentTime = 0.0f;

    std::vector<int> processOrder;
    std::vector<glm::mat4> localTransforms;
    std::vector<glm::mat4> worldTransforms;
    std::vector<glm::mat4> finalMatrices;

    glm::vec3 InterpolatePosition(const BoneAnimation& channel, float time);
    glm::quat InterpolateRotation(const BoneAnimation& channel, float time);
    glm::vec3 InterpolateScale(const BoneAnimation& channel, float time);
};
#endif 