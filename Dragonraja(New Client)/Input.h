#pragma once

#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_events.h>

namespace Input
{
    // -----------------------------
    // KEY MAPPING (SDL backend)
    // -----------------------------
    enum class Key 
    {
        S = SDL_SCANCODE_S,
        W = SDL_SCANCODE_W,
        A = SDL_SCANCODE_A,
        D = SDL_SCANCODE_D,

        Space = SDL_SCANCODE_SPACE,
        Escape = SDL_SCANCODE_ESCAPE,
        Enter = SDL_SCANCODE_RETURN,

        Left = SDL_SCANCODE_LEFT,
        Right = SDL_SCANCODE_RIGHT,
        Up = SDL_SCANCODE_UP,
        Down = SDL_SCANCODE_DOWN
    };


    // -----------------------------
    // INPUT API
    // -----------------------------
    int GetMouseX();
    int GetMouseY();

    void SetMousePosition(int x, int y);

    void Clear();

    bool IsDown(Key key);
    bool IsPressed(Key key);
    bool IsReleased(Key key);

    void Insert(Key key);
    void Remove(Key key);
}