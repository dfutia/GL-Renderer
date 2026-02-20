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
#include "FrameBuffer.h"

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

	Model mannequinModel = LoadModel((GetMediaPath() / "Models/mannequin.fbx").string());

	// Basic vertex shader
	const std::string vertexShaderSource = GLSL
	(
		layout(location = 0) in vec3 aPos;
		layout(location = 1) in vec3 aNormal;
		layout(location = 2) in vec2 aTexCoord;

		uniform mat4 uMVP;

		out vec3 vNormal;
		out vec2 vTexCoord;

		void main()
		{
			gl_Position = uMVP * vec4(aPos, 1.0);
			vNormal = aNormal;
			vTexCoord = aTexCoord;
		}
	);

	// Basic fragment shader
	const std::string fragmentShaderSource = GLSL
	(
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
	);

	ShaderProgram shader(
		Shader(Shader::Vertex, vertexShaderSource),
		Shader(Shader::Fragment, fragmentShaderSource)
	);

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
	glm::vec3 cameraPosition = glm::vec3(0.0f, 100.0f, 300.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 100.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Model
	glm::vec3 modelPosition = glm::vec3(0.0f);
	glm::vec3 modelRotation = glm::vec3(0.0f);
	glm::vec3 modelScale = glm::vec3(1.0f);

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

		// =====================
		// PASS 1: Render scene to FBO
		// =====================
		//graphics.BindFrameBuffer(sceneFBO);
		graphics.BindFrameBuffer(0);
		graphics.SetViewport(0, 0, window.GetWidth(), window.GetHeight());
		graphics.SetClearColor(0.0f, 0.0f, 0.0f);
		graphics.Clear(true, true, false);
		graphics.SetDepthTest(true);

		// model
		glUseProgram(shader);
		shader.SetUniform(shader.GetUniform("uMVP"), mvp);

		graphics.BindResource(ResourceType::VERTEX_BUFFER, *mannequinModel.mesh.vao);
		graphics.DrawNonIndexed(mannequinModel.mesh.VertexCount());

		// skybox
		graphics.SetDepthWrite(false);
		graphics.SetDepthFunc(DepthFunc::LessEqual);

		glUseProgram(skyboxShader);
		graphics.BindResource(ResourceType::SHADER_PROGRAM, skyboxShader);

		glm::mat4 skyboxView = glm::mat4(glm::mat3(view));

		skyboxShader.SetUniform(skyboxShader.GetUniform("uProjection"), projection);
		skyboxShader.SetUniform(skyboxShader.GetUniform("uView"), skyboxView);
		skyboxShader.SetUniform(skyboxShader.GetUniform("uSkybox"), 0);

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
