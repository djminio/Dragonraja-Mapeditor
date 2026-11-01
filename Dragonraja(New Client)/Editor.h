#pragma once
// =====================================================================
// Editor.h - the Dragon Raja map editor (Raja Management Tool look)
// =====================================================================
#include <SDL2/SDL.h>
#include <string>

namespace Editor
{
	bool Init(SDL_Window* window, SDL_Renderer* renderer);
	void Shutdown();
	void HandleEvent(const SDL_Event& e);		// file drops, window close
	void Frame();								// the whole UI, between ImGui::NewFrame and ImGui::Render
	bool WantsQuit();							// asked to quit and nothing unsaved (or it was saved / thrown away)
	bool OpenMap(const std::string& path);
}
