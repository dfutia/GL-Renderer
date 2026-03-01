#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include "PCH.h"

#include "Platform/Window.h"
#include "Platform/File.h"
#include "Platform/DeltaTime.h"
#include "Platform/Benchmark.h"

#include "Physics/PhysicsWorld.h"

#include "Actors/Actor.h"
#include "Actors/TransformComponent.h"
#include "Actors/PhysicsComponent.h"
#include "Actors/ModelComponent.h"
#include "Actors/SkinnedModelComponent.h"
#include "Actors/RenderComponent.h"

#include "Rendering/Skybox.h"
#include "Rendering/Model.h"
#include "Rendering/Camera.h"
#include "Rendering/Mesh.h"
#include "Rendering/Material.h"
#include "Rendering/Light.h"
#include "Rendering/ShaderLibrary.h"
#include "Rendering/Primitives.h"
#include "Rendering/Renderer.h"
#include "Rendering/Scene.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/VertexBuffer.h"

void DrawPropertyInspector(ActorComponent* component)
{
	PropertyRegistry registry;
	component->RegisterProperties(registry);

	for (const auto& prop : registry.GetProperties())
	{
		switch (prop.type)
		{
		case PropertyType::Int:
			ImGui::DragInt(prop.name.c_str(), (int*)prop.data, (int)prop.speed, (int)prop.min, (int)prop.max);
			break;
		case PropertyType::Float:
			ImGui::DragFloat(prop.name.c_str(), (float*)prop.data, prop.speed, prop.min, prop.max);
			break;
		case PropertyType::Bool:
			ImGui::Checkbox(prop.name.c_str(), (bool*)prop.data);
			break;
		case PropertyType::Vec2:
			ImGui::DragFloat2(prop.name.c_str(), &((glm::vec2*)prop.data)->x, prop.speed, prop.min, prop.max);
			break;
		case PropertyType::Vec3:
			ImGui::DragFloat3(prop.name.c_str(), &((glm::vec3*)prop.data)->x, prop.speed, prop.min, prop.max);
			break;
		case PropertyType::Vec4:
			ImGui::DragFloat4(prop.name.c_str(), &((glm::vec4*)prop.data)->x, prop.speed, prop.min, prop.max);
			break;
		case PropertyType::Color3:
			ImGui::ColorEdit3(prop.name.c_str(), &((glm::vec3*)prop.data)->x);
			break;
		case PropertyType::Color4:
			ImGui::ColorEdit4(prop.name.c_str(), &((glm::vec4*)prop.data)->x);
			break;
		case PropertyType::String:
		{
			std::string* str = (std::string*)prop.data;
			char buffer[256];
			strncpy_s(buffer, str->c_str(), sizeof(buffer));
			if (ImGui::InputText(prop.name.c_str(), buffer, sizeof(buffer)))
				*str = buffer;
		}
		break;
		}
	}

	// Let component add custom UI
	component->OnInspectorGUI();
}

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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(window.GetSDLWindow(), window.GetGLContext());
	ImGui_ImplOpenGL3_Init("#version 420");
	ImGui::StyleColorsDark();

	GraphicsDevice graphics;
	ShaderLibrary shaders;
	Renderer renderer(graphics, shaders, 1280, 720, 8);
	PhysicsWorld physics;
	Scene scene;
	scene.physics = &physics;

	auto woodTexture = LoadTexture((GetMediaPath() / "Images/wood.png").string());
	auto containerTexture = LoadTexture((GetMediaPath() / "Images/container.jpg").string());

	Model mannequin = LoadModel((GetMediaPath() / "Models/mannequin.fbx").string());
	SkinnedModel mannequinSkinned = LoadSkinnedModel((GetMediaPath() / "Models/Hip Hop Dancing.fbx").string());
	std::println("{}", mannequinSkinned.DebugInfo());

	std::vector<Animation> walkAnim = LoadAnimations((GetMediaPath() / "Models/Walking.fbx").string());

	shaders.Load("unlit", GetMediaPath() / "Shaders/mesh.vert", GetMediaPath() / "Shaders/unlit.frag");
	shaders.Load("phong", GetMediaPath() / "Shaders/mesh.vert", GetMediaPath() / "Shaders/phong.frag");
	shaders.Load("skinned_phong", GetMediaPath() / "Shaders/mesh.vert", GetMediaPath() / "Shaders/phong.frag", { "SKINNED" });
	shaders.Load("depth", GetMediaPath() / "Shaders/depth.vert", GetMediaPath() / "Shaders/depth.frag");
	shaders.Load("skinned_depth", GetMediaPath() / "Shaders/depth.vert", GetMediaPath() / "Shaders/depth.frag", { "SKINNED" });
	shaders.Load("blit", GetMediaPath() / "Shaders/blit.vert", GetMediaPath() / "Shaders/blit.frag");

	// Camera
	Camera camera;
	camera.position = glm::vec3(0.0f, 100.0f, 300.0f);
	camera.aspectRatio = window.GetAspectRatio();
	scene.mainCamera = &camera;

	// light
	DirectionalLight light;
	light.direction = glm::normalize(glm::vec3(-200.0f, -300.0f, -200.0f));
	light.color = glm::vec3(1.0f);
	light.intensity = 1.0f;
	scene.mainLight = &light;

	Skybox skybox = LoadSkybox({
		(GetMediaPath() / "Skybox/right.jpg").string(),
		(GetMediaPath() / "Skybox/left.jpg").string(),
		(GetMediaPath() / "Skybox/top.jpg").string(),
		(GetMediaPath() / "Skybox/bottom.jpg").string(),
		(GetMediaPath() / "Skybox/front.jpg").string(),
		(GetMediaPath() / "Skybox/back.jpg").string()
	});
	scene.skybox = &skybox;

	Mesh cubeMesh = Primitives::CreateCube();

	Material cubeMaterial = Material::CreateDefault();
	cubeMaterial.SetTexture(Material::DIFFUSE, containerTexture);

	Material floorMaterial = Material::CreateDefault();
	floorMaterial.SetTexture(Material::DIFFUSE, woodTexture);

	for (int i = 0; i < 10; i++)
	{
		auto cube = scene.CreateActor();

		auto* transform = cube->AddComponent<TransformComponent>();
		transform->position = glm::vec3(i * 30.0f - 135.0f, 0.0f, 0.0f);
		transform->rotation = glm::vec3(i * 15.0f, i * 25.0f, 0.0f);
		transform->scale = glm::vec3(5.0f);

		cube->AddComponent<ModelComponent>(cubeMesh, cubeMaterial);

		auto* phys = cube->AddComponent<PhysicsComponent>(physics);
		phys->SetMass(1.0f);
		phys->SetBoxShape(glm::vec3(5.0f, 5.0f, 5.0f));
	}

	auto floor = scene.CreateActor();

	auto* floorTransform = floor->AddComponent<TransformComponent>();
	floorTransform->position = glm::vec3(0.0f, -50.0f, 0.0f);
	floorTransform->scale = glm::vec3(500.0f, 1.0f, 500.0f);

	floor->AddComponent<ModelComponent>(cubeMesh, floorMaterial);

	auto* floorPhys = floor->AddComponent<PhysicsComponent>(physics);
	floorPhys->SetMass(0.0f);
	floorPhys->SetBoxShape(glm::vec3(500.0f, 1.0f, 500.0f));

	auto mannequinActor = scene.CreateActor();

	mannequinActor->AddComponent<TransformComponent>();
	auto* skinned = mannequinActor->AddComponent<SkinnedModelComponent>(mannequinSkinned.mesh, mannequinSkinned.material);
	mannequinActor->AddComponent<RenderComponent>();

	skinned->animator.Play(&walkAnim[0]); // Play first animation

	renderer.SetCamera(&camera);
	renderer.SetLight(&light);
	renderer.SetSkybox(&skybox);
	renderer.SetViewport(window.GetWidth(), window.GetHeight());

	bool mouseCaptured = true;
	SDL_SetRelativeMouseMode(SDL_TRUE);

	RenderBatch batch;
	DeltaTime deltaTime;
	SDL_Event event;
	bool running = true;
	while (running)
	{
		float dt = deltaTime.Update();

		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				if (mouseCaptured)
				{
					camera.ProcessMouseMovement((float)event.motion.xrel, -(float)event.motion.yrel);
				}
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					mouseCaptured = !mouseCaptured;
					SDL_SetRelativeMouseMode(mouseCaptured ? SDL_TRUE : SDL_FALSE);
				}
			}
			else if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					int newWidth = event.window.data1;
					int newHeight = event.window.data2;
					window.OnResize(newWidth, newHeight);
					renderer.SetViewport(window.GetWidth(), window.GetHeight());
					camera.aspectRatio = (float)newWidth / (float)newHeight;
				}
			}
		}

		if (mouseCaptured)
		{
			const Uint8* keystate = SDL_GetKeyboardState(NULL);
			camera.ProcessKeyboard(keystate, dt);
		}

		{
			BENCHMARK_SCOPE("Physics");
			physics.Update(dt);
		}

		scene.Update(dt);

		scene.GatherRenderables(batch, camera.position);
		renderer.Render(batch);
		//renderer.Blit(renderer.GetResult(), *shaders.Get("blit"));
		//batch.Clear();

		// =====================
		// Imgui
		// =====================
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Actor Hierarchy
		ImGui::Begin("Actors");
		static Actor* selectedActor = nullptr;
		int index = 0;
		scene.ForEachActor([&](Actor* actor) {
			std::string label = "Actor " + std::to_string(index);
			if (ImGui::Selectable(label.c_str(), selectedActor == actor))
				selectedActor = actor;
			index++;
			});
		ImGui::End();

		// Inspector
		ImGui::Begin("Inspector");
		if (selectedActor)
		{
			selectedActor->ForEachComponent([](ActorComponent* component)
				{
					if (ImGui::CollapsingHeader(component->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
					{
						DrawPropertyInspector(component);
					}
				});
		}
		else
		{
			ImGui::Text("No actor selected");
		}
		ImGui::End();

		// Light
		ImGui::Begin("Light");
		ImGui::DragFloat3("Direction", &light.direction.x, 0.01f);
		if (ImGui::Button("Normalize"))
			light.direction = glm::normalize(light.direction);
		ImGui::ColorEdit3("Color", &light.color.x);
		ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f, 10.0f);

		if (ImGui::CollapsingHeader("Shadows"))
		{
			ImGui::DragFloat("Near Plane", &light.shadowNearPlane, 0.1f, 0.1f, 100.0f);
			ImGui::DragFloat("Far Plane", &light.shadowFarPlane, 1.0f, 100.0f, 5000.0f);
			ImGui::DragFloat("Ortho Size", &light.shadowOrthoSize, 1.0f, 10.0f, 2000.0f);
			ImGui::DragFloat("Distance", &light.shadowDistance, 1.0f, 100.0f, 2000.0f);
		}
		ImGui::End();

		// Camera
		ImGui::Begin("Camera");
		ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
		ImGui::Text("Yaw: %.2f", camera.yaw);
		ImGui::Text("Pitch: %.2f", camera.pitch);
		ImGui::DragFloat("Move Speed", &camera.moveSpeed, 1.0f, 1.0f, 1000.0f);
		ImGui::DragFloat("Sensitivity", &camera.mouseSensitivity, 0.01f, 0.01f, 1.0f);
		ImGui::Separator();
		ImGui::DragFloat("FOV", &camera.fov, 0.5f, 1.0f, 120.0f);
		ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("Far Plane", &camera.farPlane, 10.0f, 100.0f, 50000.0f);
		ImGui::End();

		// Performance
		ImGui::Begin("Performance");
		ImGui::Text("FPS: %.1f", deltaTime.GetFPS());
		ImGui::Text("Frame Time: %.3f ms", deltaTime.GetMS());
		ImGui::End();

		ImGui::Begin("Profiler");
		for (const auto& [name, result] : Benchmarker::Instance().GetResults())
		{
			ImGui::Text("%s: %.2f us (avg: %.2f, min: %.2f, max: %.2f)",
				name.c_str(),
				result.lastTime,
				result.avgTime,
				result.minTime,
				result.maxTime);
		}
		if (ImGui::Button("Reset Stats"))
			Benchmarker::Instance().ResetAll();
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.SwapBuffers();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return 0;
}






