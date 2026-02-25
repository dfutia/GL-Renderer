#ifndef DELTA_TIME_H
#define DELTA_TIME_H

#include <SDL.h>

class DeltaTime
{
public:
    DeltaTime()
    {
        lastTime = SDL_GetPerformanceCounter();
        frequency = SDL_GetPerformanceFrequency();
    }

    float Update()
    {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        delta = (currentTime - lastTime) / static_cast<float>(frequency);
        lastTime = currentTime;
        return delta;
    }

    float Get() const { return delta; }
    float GetFPS() const { return 1.0f / delta; }
    float GetMS() const { return delta * 1000.0f; }

private:
    Uint64 lastTime = 0;
    Uint64 frequency = 0;
    float delta = 0.0f;
};

#endif