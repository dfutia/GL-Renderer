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
	cube.material = Material::CreatePhongMaterial();

	Object cubeObject;
	cubeObject.model = &cube;
	cubeObject.transform.scale = glm::vec3(10.0f);
	cubeObject.model->material.SetTexture(Material::Diffuse, containerTexture);

	// Floor
	Model floor;
	floor.mesh = cubeMesh;  // reuse the same mesh
	floor.material = Material::CreatePhongMaterial();
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

	FrameBuffer sceneFBO(window.GetWidth(), window.GetHeight(), 32, 24);

	ShaderProgram postProcessShader(
		Shader(Shader::Vertex, ReadTextFile(GetMediaPath() / "Shaders/postprocess.vert")),
		Shader(Shader::Fragment, ReadTextFile(GetMediaPath() / "Shaders/postprocess.frag"))
	);

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
		// PASS 1: Render scene to FBO
		// =====================
		//graphics.BindFrameBuffer(sceneFBO);
		graphics.BindFrameBuffer(0);
		graphics.SetViewport(0, 0, window.GetWidth(), window.GetHeight());
		graphics.SetClearColor(0.0f, 0.0f, 0.0f);
		graphics.Clear(true, true, false);
		graphics.SetDepthTest(true);

		cubeObject.model->material.Apply();
		cubeObject.model->material.SetUniform("viewMatrix", view);
		cubeObject.model->material.SetUniform("projectionMatrix", projection);
		cubeObject.model->material.SetUniform("hasDiffuseTexture", 0);
		cubeObject.model->material.SetUniform("light.position", lightPosition);
		cubeObject.model->material.SetUniform("light.color", lightColor);
		cubeObject.model->material.SetUniform("light.intensity", lightIntensity);
		cubeObject.model->material.SetUniform("viewPos", camera.position);
		cubeObject.model->material.SetUniform("hasDiffuseTexture", 1);  // enable texture

		graphics.BindResource(ResourceType::VERTEX_BUFFER, *cubeObject.model->mesh.vao);

		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = cubeTransforms[i].GetMatrix();
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

			cubeObject.model->material.SetUniform("modelMatrix", model);
			cubeObject.model->material.SetUniform("normalMatrix", normalMatrix);

			graphics.DrawNonIndexed(cubeObject.model->mesh.VertexCount());
		}

		floor.material.Apply();
		floor.material.SetUniform("viewMatrix", view);
		floor.material.SetUniform("projectionMatrix", projection);
		floor.material.SetUniform("hasDiffuseTexture", 1);  // enable texture
		floor.material.SetUniform("light.position", lightPosition);
		floor.material.SetUniform("light.color", lightColor);
		floor.material.SetUniform("light.intensity", lightIntensity);
		floor.material.SetUniform("viewPos", camera.position);

		glm::mat4 floorModel = floorTransform.GetMatrix();
		glm::mat3 floorNormalMatrix = glm::transpose(glm::inverse(glm::mat3(floorModel)));

		floor.material.SetUniform("modelMatrix", floorModel);
		floor.material.SetUniform("normalMatrix", floorNormalMatrix);

		graphics.BindResource(ResourceType::VERTEX_BUFFER, *floor.mesh.vao);
		graphics.DrawNonIndexed(floor.mesh.VertexCount());

		// model
		//mannequinModel.material.Apply();

		//mannequinModel.material.SetUniform("modelMatrix", model);
		//mannequinModel.material.SetUniform("viewMatrix", view);
		//mannequinModel.material.SetUniform("projectionMatrix", projection);

		//glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
		//mannequinModel.material.SetUniform("normalMatrix", normalMatrix);

		//mannequinModel.material.SetUniform("light.position", lightPosition);
		//mannequinModel.material.SetUniform("light.color", lightColor);
		//mannequinModel.material.SetUniform("light.intensity", lightIntensity);

		//mannequinModel.material.SetUniform("viewPos", camera.position);

		//graphics.BindResource(ResourceType::VERTEX_BUFFER, *mannequinModel.mesh.vao);
		//graphics.DrawNonIndexed(mannequinModel.mesh.VertexCount());

		// skybox
		graphics.SetDepthWrite(false);
		graphics.SetDepthFunc(DepthFunc::LessEqual);

		glUseProgram(skyboxShader);
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

		// =====================
		// PASS 2: Post-process fullscreen quad
		// =====================
		//graphics.BindFrameBuffer(0);
		//graphics.Clear(true, false, false);
		//graphics.SetDepthTest(false);

		//glUseProgram(postProcessShader);

		//// Bind the scene color texture to slot 0
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, (unsigned int)sceneFBO.GetTexture());
		//postProcessShader.SetUniform(postProcessShader.GetUniform("screenTexture"), 0);

		//graphics.BindResource(ResourceType::VERTEX_BUFFER, quadVAO);
		//graphics.DrawNonIndexed(6);

		//graphics.SetDepthTest(true);

		window.SwapBuffers();
	}

	return 0;
}
