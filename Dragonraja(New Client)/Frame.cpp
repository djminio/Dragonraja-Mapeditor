#include "Frame.h"


namespace Frame
{
    Uint32 FrameStart = 0;
    Uint32 FrameTime = 0;
    constexpr int FPS = 60;
    constexpr int FrameDelay = 1000 / FPS;
    int FrameCount = 0;

    void StartFrame()
    {
        FrameStart = SDL_GetTicks();
    }

    void EndFrame()
    {
        FrameTime = SDL_GetTicks() - FrameStart;

        if (FrameTime < FrameDelay)
        {
            SDL_Delay(FrameDelay - FrameTime);
        }

        ++FrameCount;
    }
}