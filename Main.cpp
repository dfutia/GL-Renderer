#include <SDL.h>

#include "PCH.h"
#include "File.h"
#include "Window.h"
#include "Model.h"
#include "Texture.h"
#include "Camera.h"
#include "GraphicsDevice.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "FrameBuffer.h"
#include "Material.h"
#include "Transform.h"
#include "Mesh.h"

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

struct Object
{
	Model* model;
	Transform transform;

	glm::mat4 GetWorldMatrix() const
	{
		glm::mat4 local = transform.GetMatrix();
		return local;
	}
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

	Texture woodTexture = LoadTexture((GetMediaPath() / "Images/wood.png").string());
	Texture containerTexture = LoadTexture((GetMediaPath() / "Images/container.jpg").string());

	Model mannequinModel = LoadModel((GetMediaPath() / "Models/mannequin.fbx").string());
	Object mannequinObject;
	mannequinObject.model = &mannequinModel;

	Mesh cubeMesh = CreateMesh(cubeVerticesWithNormalsAndUVs, 36, sizeof(float) * 8);

	Model cube;
	cube.mesh = cubeMesh;
	cube.material = Material::CreatePhongShadowMaterial();
	cube.material.SetTexture(Material::Diffuse, containerTexture);

	Object cubeObject;
	cubeObject.model = &cube;
	cubeObject.transform.scale = glm::vec3(10.0f);

	// Floor
	Model floor;
	floor.mesh = cubeMesh;  // reuse the same mesh
	floor.material = Material::CreatePhongShadowMaterial();
	floor.material.SetTexture(Material::Diffuse, woodTexture);

	Transform floorTransform;
	floorTransform.position = glm::vec3(0.0f, -50.0f, 0.0f);  // below the cubes
	floorTransform.scale = glm::vec3(500.0f, 1.0f, 500.0f);   // wide and thin

	VertexBuffer skyboxVBO(skyboxVertices, sizeof(skyboxVertices), VertexBuffer::StaticDraw);
	VertexArray skyboxVAO;
	skyboxVAO.BindAttribute(0, skyboxVBO, GL_FLOAT, 3, sizeof(float) * 3, 0);

	Texture cubemap = LoadCubemap({
		(GetMediaPath() / "Skybox/right.jpg").string(),
		(GetMediaPath() / "Skybox/left.jpg").string(),
		(GetMediaPath() / "Skybox/top.jpg").string(),
		(GetMediaPath() / "Skybox/bottom.jpg").string(),
		(GetMediaPath() / "Skybox/front.jpg").string(),
		(GetMediaPath() / "Skybox/back.jpg").string()
		});

	ShaderProgram skyboxShader(
		Shader(Shader::Vertex, ReadTextFile(GetMediaPath() / "Shaders/skybox.vert")),
		Shader(Shader::Fragment, ReadTextFile(GetMediaPath() / "Shaders/skybox.frag"))
	);

	VertexBuffer quadVBO(quadVertices, sizeof(quadVertices), VertexBuffer::StaticDraw);
	VertexArray quadVAO;
	unsigned int quadStride = sizeof(float) * 4;
	quadVAO.BindAttribute(0, quadVBO, GL_FLOAT, 2, quadStride, 0);
	quadVAO.BindAttribute(1, quadVBO, GL_FLOAT, 2, quadStride, sizeof(float) * 2);

	ShaderProgram postProcessShader(
		Shader(Shader::Vertex, ReadTextFile(GetMediaPath() / "Shaders/postprocess.vert")),
		Shader(Shader::Fragment, ReadTextFile(GetMediaPath() / "Shaders/postprocess.frag"))
	);

	ShaderProgram depthShader(
		Shader(Shader::Vertex, ReadTextFile(GetMediaPath() / "Shaders/depth.vert")),
		Shader(Shader::Fragment, ReadTextFile(GetMediaPath() / "Shaders/depth.frag"))
	);

	FrameBuffer sceneFBO(window.GetWidth(), window.GetHeight(), FrameBuffer::ColorAndDepth, 32, 24);
	FrameBuffer shadowMap(SHADOW_WIDTH, SHADOW_HEIGHT, FrameBuffer::DepthOnly, 0, 24);

	// Camera
	Camera camera;
	camera.position = glm::vec3(0.0f, 100.0f, 300.0f);

	// light
	glm::vec3 lightPosition = glm::vec3(200.0f, 300.0f, 200.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float lightIntensity = 1.0f;

	std::vector<Transform> cubeTransforms(10);
	for (int i = 0; i < 10; i++)
	{
		cubeTransforms[i].position = glm::vec3(i * 30.0f - 135.0f, 0.0f, 0.0f); // spread along X
		cubeTransforms[i].rotation = glm::vec3(i * 15.0f, i * 25.0f, 0.0f);     // vary rotations
		cubeTransforms[i].scale = glm::vec3(5.0f);
	}

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

		graphics.BindResource(ResourceType::SHADER_PROGRAM, depthShader);
		depthShader.SetUniform(depthShader.GetUniform("lightSpaceMatrix"), lightSpaceMatrix);

		// Render cubes to shadow map
		graphics.BindResource(ResourceType::VERTEX_BUFFER, *cubeObject.model->mesh.vao);
		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = cubeTransforms[i].GetMatrix();
			depthShader.SetUniform(depthShader.GetUniform("modelMatrix"), model);
			graphics.DrawNonIndexed(cubeObject.model->mesh.VertexCount());
		}


		// Render floor to shadow map
		glm::mat4 floorModel = floorTransform.GetMatrix();
		depthShader.SetUniform(depthShader.GetUniform("modelMatrix"), floorModel);
		graphics.BindResource(ResourceType::VERTEX_BUFFER, *floor.mesh.vao);  // Changed from vbo to vao
		graphics.DrawNonIndexed(floor.mesh.VertexCount());

		// =====================
		// PASS 2: Scene with shadows
		// =====================
		graphics.BindFrameBuffer(0);
		graphics.SetViewport(0, 0, window.GetWidth(), window.GetHeight());
		graphics.SetClearColor(0.0f, 0.0f, 0.0f);
		graphics.Clear(true, true, false);

		// Set shadow map on both materials
		cube.material.SetTexture(Material::Shadow, shadowMap.GetDepthTexture());
		floor.material.SetTexture(Material::Shadow, shadowMap.GetDepthTexture());

		// Render cubes
		cube.material.Apply();
		cube.material.SetUniform("viewMatrix", view);
		cube.material.SetUniform("projectionMatrix", projection);
		cube.material.SetUniform("lightSpaceMatrix", lightSpaceMatrix);
		cube.material.SetUniform("light.position", lightPosition);
		cube.material.SetUniform("light.color", lightColor);
		cube.material.SetUniform("light.intensity", lightIntensity);
		cube.material.SetUniform("viewPos", camera.position);

		graphics.BindResource(ResourceType::VERTEX_BUFFER, *cubeObject.model->mesh.vao);
		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = cubeTransforms[i].GetMatrix();
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

			cube.material.SetUniform("modelMatrix", model);
			cube.material.SetUniform("normalMatrix", normalMatrix);

			graphics.DrawNonIndexed(cubeObject.model->mesh.VertexCount());
		}

		// Render floor
		floor.material.Apply();
		floor.material.SetUniform("viewMatrix", view);
		floor.material.SetUniform("projectionMatrix", projection);
		floor.material.SetUniform("lightSpaceMatrix", lightSpaceMatrix);
		floor.material.SetUniform("light.position", lightPosition);
		floor.material.SetUniform("light.color", lightColor);
		floor.material.SetUniform("light.intensity", lightIntensity);
		floor.material.SetUniform("viewPos", camera.position);

		glm::mat3 floorNormalMatrix = glm::transpose(glm::inverse(glm::mat3(floorModel)));
		floor.material.SetUniform("modelMatrix", floorModel);
		floor.material.SetUniform("normalMatrix", floorNormalMatrix);

		graphics.BindResource(ResourceType::VERTEX_BUFFER, *floor.mesh.vao);
		graphics.DrawNonIndexed(floor.mesh.VertexCount());

		// =====================
		// Skybox
		// =====================
		graphics.SetDepthWrite(false);
		graphics.SetDepthFunc(DepthFunc::LessEqual);

		graphics.BindResource(ResourceType::SHADER_PROGRAM, skyboxShader);

		glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
		skyboxShader.SetUniform(skyboxShader.GetUniform("projectionMatrix"), projection);
		skyboxShader.SetUniform(skyboxShader.GetUniform("viewMatrix"), skyboxView);
		skyboxShader.SetUniform(skyboxShader.GetUniform("skybox"), 0);

		graphics.BindCubemap(cubemap, 0);
		graphics.BindResource(ResourceType::VERTEX_BUFFER, skyboxVAO);
		graphics.DrawNonIndexed(36);

		graphics.SetDepthWrite(true);
		graphics.SetDepthFunc(DepthFunc::Less);

		window.SwapBuffers();
	}

	return 0;
}
