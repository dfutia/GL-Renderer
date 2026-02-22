#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    glm::vec3 position = glm::vec3(0.0f, 100.0f, 300.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

    float yaw = -90.0f;   // Looking along -Z initially
    float pitch = 0.0f;

    float moveSpeed = 200.0f;
    float mouseSensitivity = 0.1f;

    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 10000.0f;

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 GetProjectionMatrix(float aspectRatio) const
    {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    void ProcessKeyboard(const Uint8* keystate, float deltaTime)
    {
        float velocity = moveSpeed * deltaTime;

        if (keystate[SDL_SCANCODE_W])
            position += front * velocity;
        if (keystate[SDL_SCANCODE_S])
            position -= front * velocity;
        if (keystate[SDL_SCANCODE_A])
            position -= right * velocity;
        if (keystate[SDL_SCANCODE_D])
            position += right * velocity;
        if (keystate[SDL_SCANCODE_SPACE])
            position += up * velocity;
        if (keystate[SDL_SCANCODE_LSHIFT])
            position -= up * velocity;
    }

    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Clamp pitch to avoid flipping
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        UpdateVectors();
    }

private:
    void UpdateVectors()
    {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(right, front));
    }
};

#endif