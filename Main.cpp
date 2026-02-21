#include <SDL.h>

#include "PCH.h"

#include "Platform/Window.h"
#include "Platform/File.h"

#include "Physics/PhysicsWorld.h"

#include "Actors/Actor.h"
#include "Actors/TransformComponent.h"
#include "Actors/PhysicsComponent.h"
#include "Actors/ModelComponent.h"
#include "Actors/SkinnedModelComponent.h"

#include "Rendering/Skybox.h"
#include "Rendering/Model.h"
#include "Rendering/Camera.h"
#include "Rendering/Mesh.h"
#include "Rendering/Material.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/VertexBuffer.h"

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

float cubeVerticesWithNormalsAndUVs[] = {
	// positions          // normals           // texcoords
	// Back face
	-1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	// Front face
	-1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	 1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	-1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
	// Left face
	-1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	// Right face
	 1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 1.0f,  1.0f, -1.0f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 // Bottom face
	 -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	  1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	  1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	  1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 -1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	 -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 // Top face
	 -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	  1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	  1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	  1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 -1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
};

float skyboxVertices[] = {
	-1,  1, -1,  -1, -1, -1,   1, -1, -1,   1, -1, -1,   1,  1, -1,  -1,  1, -1,
	-1, -1,  1,  -1, -1, -1,  -1,  1, -1,  -1,  1, -1,  -1,  1,  1,  -1, -1,  1,
	 1, -1, -1,   1, -1,  1,   1,  1,  1,   1,  1,  1,   1,  1, -1,   1, -1, -1,
	-1, -1,  1,  -1,  1,  1,   1,  1,  1,   1,  1,  1,   1, -1,  1,  -1, -1,  1,
	-1,  1, -1,   1,  1, -1,   1,  1,  1,   1,  1,  1,  -1,  1,  1,  -1,  1, -1,
	-1, -1, -1,  -1, -1,  1,   1, -1, -1,   1, -1, -1,  -1, -1,  1,   1, -1,  1
};

float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

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

	PhysicsWorld physics;

	Texture woodTexture = LoadTexture((GetMediaPath() / "Images/wood.png").string());
	Texture containerTexture = LoadTexture((GetMediaPath() / "Images/container.jpg").string());

	Model mannequin = LoadModel((GetMediaPath() / "Models/mannequin.fbx").string());

	ShaderProgram phongShader(
		Shader(Shader::Vertex, ReadTextFile(GetMediaPath() / "Shaders/static.vert")),
		Shader(Shader::Fragment, ReadTextFile(GetMediaPath() / "Shaders/phong_shadow.frag"))
	);

	ShaderProgram depthShader(
		Shader(Shader::Vertex, ReadTextFile(GetMediaPath() / "Shaders/depth.vert")),
		Shader(Shader::Fragment, ReadTextFile(GetMediaPath() / "Shaders/depth.frag"))
	);

	Skybox skybox = LoadSkybox({
	(GetMediaPath() / "Skybox/right.jpg").string(),
	(GetMediaPath() / "Skybox/left.jpg").string(),
	(GetMediaPath() / "Skybox/top.jpg").string(),
	(GetMediaPath() / "Skybox/bottom.jpg").string(),
	(GetMediaPath() / "Skybox/front.jpg").string(),
	(GetMediaPath() / "Skybox/back.jpg").string()
		});

	Mesh cubeMesh = CreateMesh(cubeVerticesWithNormalsAndUVs, 36, sizeof(float) * 8);
	Material cubeMaterial = Material::CreateDefault();
	cubeMaterial.SetTexture(Material::Diffuse, containerTexture);

	Material floorMaterial = Material::CreateDefault();
	floorMaterial.SetTexture(Material::Diffuse, woodTexture);

	std::vector<std::unique_ptr<Actor>> actors;

	for (int i = 0; i < 10; i++)
	{
		auto cube = std::make_unique<Actor>();

		auto* transform = cube->AddComponent<TransformComponent>();
		transform->position = glm::vec3(i * 30.0f - 135.0f, 0.0f, 0.0f);
		transform->rotation = glm::vec3(i * 15.0f, i * 25.0f, 0.0f);
		transform->scale = glm::vec3(5.0f);

		cube->AddComponent<ModelComponent>(cubeMesh, cubeMaterial);

		auto* phys = cube->AddComponent<PhysicsComponent>(physics);
		phys->SetMass(1.0f);
		phys->SetBoxShape(glm::vec3(5.0f, 5.0f, 5.0f));

		actors.push_back(std::move(cube));
	}

	auto floor = std::make_unique<Actor>();

	auto* floorTransform = floor->AddComponent<TransformComponent>();
	floorTransform->position = glm::vec3(0.0f, -50.0f, 0.0f);
	floorTransform->scale = glm::vec3(500.0f, 1.0f, 500.0f);

	floor->AddComponent<ModelComponent>(cubeMesh, floorMaterial);

	auto* floorPhys = floor->AddComponent<PhysicsComponent>(physics);
	floorPhys->SetMass(0.0f);
	floorPhys->SetBoxShape(glm::vec3(500.0f, 1.0f, 500.0f));

	actors.push_back(std::move(floor));

	auto mannequinActor = std::make_unique<Actor>();

	mannequinActor->AddComponent<TransformComponent>();
	mannequinActor->AddComponent<ModelComponent>(mannequin.mesh, mannequin.material);

	actors.push_back(std::move(mannequinActor));

	FrameBuffer sceneFBO(window.GetWidth(), window.GetHeight(), FrameBuffer::ColorAndDepth, 32, 24);
	FrameBuffer shadowMap(SHADOW_WIDTH, SHADOW_HEIGHT, FrameBuffer::DepthOnly, 0, 24);

	// Camera
	Camera camera;
	camera.position = glm::vec3(0.0f, 100.0f, 300.0f);

	// light
	glm::vec3 lightPosition = glm::vec3(200.0f, 300.0f, 200.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float lightIntensity = 1.0f;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	Uint64 lastTime = SDL_GetPerformanceCounter();
	SDL_Event event;
	bool running = true;
	while (running)
	{
		Uint64 currentTime = SDL_GetPerformanceCounter();
		float deltaTime = (currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
		lastTime = currentTime;

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				camera.ProcessMouseMovement((float)event.motion.xrel, -(float)event.motion.yrel);
			}
			else if (event.type == SDL_KEYDOWN)
			{
				// Press Escape to release mouse / quit
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
			}
		}

		const Uint8* keystate = SDL_GetKeyboardState(NULL);
		camera.ProcessKeyboard(keystate, deltaTime);

		physics.Update(deltaTime);

		// view: where the camera is and what it's looking at
		glm::mat4 view = camera.GetViewMatrix();

		// projection: the lens of the camera (perspective/FOV or orthographic, near/far clip planes)
		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			window.GetAspectRatio(),
			0.1f,
			10000.0f
		);

		// =====================
		// PASS 1: Shadow map
		// =====================
		float nearPlane = 1.0f;
		float farPlane = 1500.0f;
		float orthoSize = 600.0f;

		glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
		glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		graphics.SetViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		graphics.BindFrameBuffer(shadowMap);
		graphics.Clear(false, true, false);
		graphics.SetDepthTest(true);

		graphics.BindShader(depthShader);
		graphics.SetUniform("lightSpaceMatrix", lightSpaceMatrix);

		for (auto& actor : actors)
		{
			auto* transform = actor->GetComponent<TransformComponent>();
			auto* model = actor->GetComponent<ModelComponent>();

			if (!transform || !model)
				continue;

			graphics.BindResource(ResourceType::VERTEX_BUFFER, *model->mesh.vao);
			graphics.SetUniform("modelMatrix", transform->GetMatrix());
			graphics.DrawNonIndexed(model->mesh.VertexCount());
		}

		// =====================
		// PASS 2: Scene with shadows
		// =====================
		graphics.BindFrameBuffer(0);
		graphics.SetViewport(0, 0, window.GetWidth(), window.GetHeight());
		graphics.SetClearColor(0.0f, 0.0f, 0.0f);
		graphics.Clear(true, true, false);

		graphics.BindShader(phongShader);
		graphics.SetUniform("viewMatrix", view);
		graphics.SetUniform("projectionMatrix", projection);
		graphics.SetUniform("lightSpaceMatrix", lightSpaceMatrix);
		graphics.SetUniform("light.position", lightPosition);
		graphics.SetUniform("light.color", lightColor);
		graphics.SetUniform("light.intensity", lightIntensity);
		graphics.SetUniform("viewPos", camera.position);

		for (auto& actor : actors)
		{
			auto* transform = actor->GetComponent<TransformComponent>();
			auto* model = actor->GetComponent<ModelComponent>();

			if (!transform || !model)
				continue;

			model->material.SetTexture(Material::Shadow, shadowMap.GetDepthTexture());
			graphics.BindMaterial(model->material);
			graphics.BindResource(ResourceType::VERTEX_BUFFER, *model->mesh.vao);
			graphics.SetUniform("modelMatrix", transform->GetMatrix());
			graphics.SetUniform("normalMatrix", transform->GetNormalMatrix());
			graphics.DrawNonIndexed(model->mesh.VertexCount());
		}

		// =====================
		// Skybox
		// =====================
		graphics.DrawSkybox(skybox, view, projection);

		window.SwapBuffers();
	}

	return 0;
}
