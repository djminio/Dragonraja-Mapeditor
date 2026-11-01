#pragma once
#include <SDL2/SDL_timer.h>

namespace Frame
{
    

    extern Uint32 FrameStart;
    extern Uint32 FrameTime;
    extern int FrameCount;

    void StartFrame();
    void EndFrame();
}