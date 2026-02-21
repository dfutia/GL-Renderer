#include <glad/glad.h>

#include "PCH.h"
#include "Window.h"

Window::Window() : window(nullptr), glContext(nullptr), initialized(false)
{
}

Window::Window(const Config& config) : config(config), window(nullptr), glContext(nullptr), initialized(false)
{
}

Window::~Window()
{
	Shutdown();
}

bool Window::Initialize()
{
	return Initialize(config);
}

bool Window::Initialize(const Config& config)
{
	this->config = config;

	if (!InitializeSDL())
	{
		return false;
	}

	if (!CreateWindow())
	{
		SDL_Quit();
		return false;
	}

	if (!CreateGLContext())
	{
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	if (!InitializeGLAD())
	{
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	initialized = true;
	return true;
}

void Window::Shutdown()
{
	if (glContext)
	{
		SDL_GL_DeleteContext(glContext);
		glContext = nullptr;
	}

	if (window)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	SDL_Quit();
	initialized = false;
}

void Window::SwapBuffers()
{
	if (window)
	{
		SDL_GL_SwapWindow(window);
	}
}

bool Window::InitializeSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0)
	{
		std::println("SDL could not initialize! SDL_Error: {}", SDL_GetError());
		return false;
	}

	// Set OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.glMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.glMinorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config.depthBits);

	return true;
}

bool Window::CreateWindow()
{
	window = SDL_CreateWindow(
		config.title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		config.width,
		config.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);

	if (window == nullptr)
	{
		std::println("Window could not be created! SDL_Error: {}", SDL_GetError());
		return false;
	}

	return true;
}

bool Window::CreateGLContext()
{
	glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr)
	{
		std::println("OpenGL context could not be created! SDL_Error: {}", SDL_GetError());
		return false;
	}

	// Set VSync
	SDL_GL_SetSwapInterval(config.vsync ? 1 : 0);

	return true;
}

bool Window::InitializeGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::println("Failed to initialize GLAD");
		return false;
	}

	return true;
}