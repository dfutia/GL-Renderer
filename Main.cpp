//Texture woodTexture = LoadTexture((GetMediaPath() / "Images/wood.png").string());
//Texture containerTexture = LoadTexture((GetMediaPath() / "Images/container.jpg").string());

#include <SDL.h>

#include "PCH.h"
#include "File.h"
#include "Window.h"
#include "Model.h"
#include "Texture.h"
#include "GraphicsDevice.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "ShaderProgram.h"

int main(int argc, char* argv[])
{
	Window::Config windowConfig;
	windowConfig.title = "OpenGL";
	windowConfig.width = 1280;
	windowConfig.height = 720;

	Window window(windowConfig);
	if (!window.Initialize())
	{
		return -1;
	}

	GraphicsDevice graphics;
	graphics.SetViewport(0, 0, window.GetWidth(), window.GetHeight());
	graphics.SetDepthTest(true);

	Model mannequinModel = LoadModel((GetMediaPath() / "Models/mannequin.fbx").string());

	SkinnedModel animatedModel = LoadSkinnedModel((GetMediaPath() / "Models/Hip Hop Dancing.fbx").string());
	Animator animator(animatedModel.mesh.skeleton);

	if (!animatedModel.mesh.animations.empty())
		animator.Play(&animatedModel.mesh.animations[0]);

	const std::string vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoord;
    layout (location = 3) in ivec4 aBoneIDs;
    layout (location = 4) in vec4 aBoneWeights;

    uniform mat4 uMVP;
    uniform mat4 uBoneMatrices[100];

    out vec3 vNormal;
    out vec2 vTexCoord;

    void main()
    {
        mat4 skinMatrix =
            aBoneWeights.x * uBoneMatrices[aBoneIDs.x] +
            aBoneWeights.y * uBoneMatrices[aBoneIDs.y] +
            aBoneWeights.z * uBoneMatrices[aBoneIDs.z] +
            aBoneWeights.w * uBoneMatrices[aBoneIDs.w];

        vec4 skinnedPos = skinMatrix * vec4(aPos, 1.0);
        gl_Position = uMVP * skinnedPos;

        vNormal = mat3(skinMatrix) * aNormal;
        vTexCoord = aTexCoord;
    }
)";

	// Basic vertex shader
	//const std::string vertexShaderSource = R"(
 //       #version 330 core
 //       layout (location = 0) in vec3 aPos;
 //       layout (location = 1) in vec3 aNormal;
 //       layout (location = 2) in vec2 aTexCoord;

 //       uniform mat4 uMVP;

 //       out vec3 vNormal;
 //       out vec2 vTexCoord;

 //       void main()
 //       {
 //           gl_Position = uMVP * vec4(aPos, 1.0);
 //           vNormal = aNormal;
 //           vTexCoord = aTexCoord;
 //       }
 //   )";

	// Basic fragment shader
	const std::string fragmentShaderSource = R"(
        #version 330 core
        in vec3 vNormal;
        in vec2 vTexCoord;

        out vec4 FragColor;

        void main()
        {
            // Simple lighting based on normal direction
            vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
            float diff = max(dot(normalize(vNormal), lightDir), 0.0);
            vec3 color = vec3(0.8) * (0.3 + 0.7 * diff); // ambient + diffuse
            FragColor = vec4(color, 1.0);
        }
    )";

	ShaderProgram shader(
		Shader(Shader::Vertex, vertexShaderSource),
		Shader(Shader::Fragment, fragmentShaderSource)
	);

	// Camera
	glm::vec3 cameraPosition = glm::vec3(0.0f, 100.0f, 300.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 100.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Model
	glm::vec3 modelPosition = glm::vec3(0.0f);
	glm::vec3 modelRotation = glm::vec3(0.0f);
	glm::vec3 modelScale = glm::vec3(1.0f);

	Uint64 lastTime = SDL_GetPerformanceCounter();
	SDL_Event event;
	while (true)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				return 0;
			}
		}

		Uint64 now = SDL_GetPerformanceCounter();
		float deltaTime = (float)(now - lastTime) / (float)SDL_GetPerformanceFrequency();
		lastTime = now;

		animator.Update(deltaTime);

		graphics.SetClearColor(0.0f, 0.0f, 0.0f);
		graphics.Clear(true, true, false);

		// view: where the camera is and what it's looking at
		glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

		// projection: the lens of the camera (perspective/FOV or orthographic, near/far clip planes)
		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			window.GetAspectRatio(),
			0.1f,
			10000.0f
		);

		// model: where the object is in the world (i.e translation, rotation, scale)
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, modelPosition);
		model = glm::rotate(model, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, modelScale);

		glm::mat4 mvp = projection * view * model;

		glUseProgram(shader);
		shader.SetUniform(shader.GetUniform("uMVP"), mvp);

		const auto& bones = animator.GetFinalBoneMatrices();
		for (int i = 0; i < (int)bones.size(); i++)
		{
			std::string name = "uBoneMatrices[" + std::to_string(i) + "]";
			shader.SetUniform(shader.GetUniform(name.c_str()), bones[i]);
		}

		graphics.BindResource(ResourceType::VERTEX_BUFFER, *animatedModel.mesh.vao);
		graphics.DrawIndexed(animatedModel.mesh.IndexCount());

		window.SwapBuffers();
	}

	return 0;
}
