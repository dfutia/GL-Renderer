#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include "PCH.h"

#include "Platform/Window.h"
#include "Platform/File.h"

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

#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/VertexBuffer.h"

//const unsigned int SHADOW_WIDTH = 2048;
//const unsigned int SHADOW_HEIGHT = 2048;
const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

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

float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

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
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

	GraphicsDevice graphics;
	PhysicsWorld physics;
	ShaderLibrary shaders;

	Texture woodTexture = LoadTexture((GetMediaPath() / "Images/wood.png").string());
	Texture containerTexture = LoadTexture((GetMediaPath() / "Images/container.jpg").string());

	Model mannequin = LoadModel((GetMediaPath() / "Models/mannequin.fbx").string());
	SkinnedModel mannequinSkinned = LoadSkinnedModel((GetMediaPath() / "Models/Hip Hop Dancing.fbx").string());

	std::vector<Animation> walkAnim = LoadAnimations((GetMediaPath() / "Models/Walking.fbx").string());

	shaders.Load("unlit", GetMediaPath() / "Shaders/mesh.vert", GetMediaPath() / "Shaders/unlit.frag");
	shaders.Load("phong", GetMediaPath() / "Shaders/mesh.vert", GetMediaPath() / "Shaders/phong.frag");
	shaders.Load("skinned_phong", GetMediaPath() / "Shaders/mesh.vert", GetMediaPath() / "Shaders/phong.frag", { "SKINNED" });
	shaders.Load("depth", GetMediaPath() / "Shaders/depth.vert", GetMediaPath() / "Shaders/depth.frag");
	shaders.Load("skinned_depth", GetMediaPath() / "Shaders/depth.vert", GetMediaPath() / "Shaders/depth.frag", { "SKINNED" });

	std::vector<std::unique_ptr<Actor>> actors;

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
	cubeMaterial.SetTexture(Material::DIFFUSE, containerTexture);

	Material floorMaterial = Material::CreateDefault();
	floorMaterial.SetTexture(Material::DIFFUSE, woodTexture);

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
	auto* skinned = mannequinActor->AddComponent<SkinnedModelComponent>(mannequinSkinned.mesh, mannequinSkinned.material);
	mannequinActor->AddComponent<RenderComponent>();

	skinned->animator.Play(&walkAnim[0]); // Play first animation

	actors.push_back(std::move(mannequinActor));

	FrameBuffer sceneFBO(window.GetWidth(), window.GetHeight(), FrameBuffer::ColorAndDepth, 32, 24);
	FrameBuffer shadowMap(SHADOW_WIDTH, SHADOW_HEIGHT, FrameBuffer::DepthOnly, 0, 24);

	// Camera
	Camera camera;
	camera.position = glm::vec3(0.0f, 100.0f, 300.0f);

	// light
	DirectionalLight light;
	light.direction = glm::normalize(glm::vec3(-200.0f, -300.0f, -200.0f));
	light.color = glm::vec3(1.0f);
	light.intensity = 1.0f;

	bool mouseCaptured = true;
	SDL_SetRelativeMouseMode(SDL_TRUE);
	Uint64 lastTime = SDL_GetPerformanceCounter();
	SDL_Event event;
	bool running = true;
	while (running)
	{
		Uint64 currentTime = SDL_GetPerformanceCounter();
		float deltaTime = (currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
		lastTime = currentTime;

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = camera.GetProjectionMatrix(window.GetAspectRatio());

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
		}

		if (mouseCaptured)
		{
			const Uint8* keystate = SDL_GetKeyboardState(NULL);
			camera.ProcessKeyboard(keystate, deltaTime);
		}

		for (auto& actor : actors)
		{
			actor->Update(deltaTime);
		}

		physics.Update(deltaTime);

		// view: where the camera is and what it's looking at
		//glm::mat4 view = camera.GetViewMatrix();

		// projection: the lens of the camera (perspective/FOV or orthographic, near/far clip planes)
		//glm::mat4 projection = camera.GetProjectionMatrix(window.GetAspectRatio());

		// =====================
		// PASS 1: Shadow map
		// =====================
		glm::mat4 lightProjection = glm::ortho(
			-light.shadowOrthoSize, light.shadowOrthoSize,
			-light.shadowOrthoSize, light.shadowOrthoSize,
			light.shadowNearPlane, light.shadowFarPlane
		);
		glm::vec3 lightPos = -light.direction * light.shadowDistance;
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		light.lightSpaceMatrix = lightProjection * lightView;
		light.shadowMap = &shadowMap.GetDepthTexture();

		graphics.SetViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		graphics.BindFrameBuffer(shadowMap);
		graphics.Clear(false, true, false);
		graphics.SetDepthTest(true);

		for (auto& actor : actors)
		{
			auto* transform = actor->GetComponent<TransformComponent>();
			auto* model = actor->GetComponent<ModelComponent>();
			auto* render = actor->GetComponent<RenderComponent>();

			if (!transform || !model)
				continue;

			// Skip objects that don't cast shadows
			if (render && !render->castsShadows)
				continue;

			// Shadow pass uses depth shader, NOT the object's rendering shader
			std::string shadowShaderName = (render && !render->shadowShader.empty())
				? render->shadowShader
				: "depth";
			ShaderProgram* depthShader = shaders.Get(shadowShaderName);

			if (!depthShader)
				continue;

			graphics.BindShader(*depthShader);

			// Depth shader only needs these two uniforms
			graphics.SetUniform("lightSpaceMatrix", light.lightSpaceMatrix);
			graphics.SetUniform("modelMatrix", transform->GetMatrix());

			graphics.BindResource(ResourceType::VERTEX_BUFFER, *model->mesh.vao);
			if (model->mesh.IndexCount() > 0)
				graphics.DrawIndexed(model->mesh.IndexCount());
			else
				graphics.DrawNonIndexed(model->mesh.VertexCount());
		}

		for (auto& actor : actors)
		{
			auto* transform = actor->GetComponent<TransformComponent>();
			auto* skinned = actor->GetComponent<SkinnedModelComponent>();
			auto* render = actor->GetComponent<RenderComponent>();

			if (!transform || !skinned)
				continue;

			if (render && !render->castsShadows)
				continue;

			ShaderProgram* depthShader = shaders.Get("skinned_depth");
			if (!depthShader)
				continue;

			graphics.BindShader(*depthShader);
			graphics.SetUniform("lightSpaceMatrix", light.lightSpaceMatrix);
			graphics.SetUniform("modelMatrix", transform->GetMatrix());

			const auto& bones = skinned->GetBoneMatrices();
			for (size_t i = 0; i < bones.size(); i++)
			{
				graphics.SetUniform("bones[" + std::to_string(i) + "]", bones[i]);
			}

			graphics.BindResource(ResourceType::VERTEX_BUFFER, *skinned->mesh.vao);
			graphics.DrawIndexed(skinned->mesh.IndexCount());
		}
		// =====================
		// PASS 2: Scene with shadows
		// =====================
		graphics.BindFrameBuffer(0);
		graphics.SetViewport(0, 0, window.GetWidth(), window.GetHeight());
		graphics.SetClearColor(0.0f, 0.0f, 0.0f);
		graphics.Clear(true, true, false);
		graphics.SetLight(light);

		for (auto& actor : actors)
		{
			auto* transform = actor->GetComponent<TransformComponent>();
			auto* model = actor->GetComponent<ModelComponent>();
			auto* render = actor->GetComponent<RenderComponent>();

			if (!transform || !model)
				continue;

			// Get shader - use RenderComponent if present, otherwise default
			std::string shaderName = render ? render->shader : "phong";
			ShaderProgram* shader = shaders.Get(shaderName);

			if (!shader)
				continue;

			graphics.BindShader(*shader);

			// Set standard uniforms
			graphics.SetUniform("modelMatrix", transform->GetMatrix());
			graphics.SetUniform("normalMatrix", transform->GetNormalMatrix());
			graphics.SetUniform("viewMatrix", view);
			graphics.SetUniform("projectionMatrix", projection);
			graphics.SetUniform("viewPos", camera.position);

			// Set custom uniforms from RenderComponent
			if (render)
			{
				render->ApplyUniforms(graphics);
			}

			graphics.SetLight(light);
			graphics.BindMaterial(model->material);
			graphics.BindResource(ResourceType::VERTEX_BUFFER, *model->mesh.vao);
			if (model->mesh.IndexCount() > 0)
				graphics.DrawIndexed(model->mesh.IndexCount());
			else
				graphics.DrawNonIndexed(model->mesh.VertexCount());
		}

		for (auto& actor : actors)
		{
			auto* transform = actor->GetComponent<TransformComponent>();
			auto* skinned = actor->GetComponent<SkinnedModelComponent>();

			if (!transform || !skinned)
				continue;

			ShaderProgram* shader = shaders.Get("skinned_phong");
			if (!shader)
				continue;

			graphics.BindShader(*shader);

			graphics.SetUniform("modelMatrix", transform->GetMatrix());
			graphics.SetUniform("normalMatrix", transform->GetNormalMatrix());
			graphics.SetUniform("viewMatrix", view);
			graphics.SetUniform("projectionMatrix", projection);
			graphics.SetUniform("viewPos", camera.position);

			// Upload bone matrices
			const auto& bones = skinned->GetBoneMatrices();
			for (size_t i = 0; i < bones.size(); i++)
			{
				graphics.SetUniform("bones[" + std::to_string(i) + "]", bones[i]);
			}

			graphics.SetLight(light);
			graphics.BindMaterial(skinned->material);
			graphics.BindResource(ResourceType::VERTEX_BUFFER, *skinned->mesh.vao);
			graphics.DrawIndexed(skinned->mesh.IndexCount());
		}
		// =====================
		// Skybox
		// =====================
		graphics.DrawSkybox(skybox, view, projection);

		// =====================
		// Imgui
		// =====================
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Actor Hierarchy
		ImGui::Begin("Actors");
		static int selectedActor = -1;
		for (int i = 0; i < actors.size(); i++)
		{
			std::string label = "Actor " + std::to_string(i);
			if (ImGui::Selectable(label.c_str(), selectedActor == i))
				selectedActor = i;
		}
		ImGui::End();

		// Inspector
		ImGui::Begin("Inspector");
		if (selectedActor >= 0 && selectedActor < actors.size())
		{
			Actor* actor = actors[selectedActor].get();

			actor->ForEachComponent([](ActorComponent* component)
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
		ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
		ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0f);
		ImGui::Text("Actors: %d", (int)actors.size());
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
