#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>

class Window
{
public:
	struct Config
	{
		std::string title = "My Application";
		int width = 1280;
		int height = 720;
		int glMajorVersion = 4;
		int glMinorVersion = 3;
		int depthBits = 24;
		int msaaSamples = 8; // 0 = disabled, 2 = 2x MSAA, 4 = 4x MSAA, 8 = 8x MSAA
		bool vsync = true;
	};

	Window();
	Window(const Config& config);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	bool Initialize();
	bool Initialize(const Config& config);
	void Shutdown();

	void SwapBuffers();

	bool IsInitialized() const { return initialized; }
	SDL_Window* GetSDLWindow() const { return window; }
	SDL_GLContext GetGLContext() const { return glContext; }

	int GetWidth() const { return config.width; }
	int GetHeight() const { return config.height; }
	float GetAspectRatio() const { return static_cast<float>(config.width) / static_cast<float>(config.height); }

	void OnResize(int width, int height)
	{
		config.width = width;
		config.height = height;
	}
private:
	Config config;
	SDL_Window* window;
	SDL_GLContext glContext;
	bool initialized;

	bool InitializeSDL();
	bool CreateWindow();
	bool CreateGLContext();
	bool InitializeGLAD();
};

#endif 