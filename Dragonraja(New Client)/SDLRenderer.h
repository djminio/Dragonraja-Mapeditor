#pragma once

#include "SDL2/SDL.h"
#include <string>
struct RenderDrawInfo {

	SDL_Window* window;//the window to draw on
	SDL_Renderer* renderer;//the renderer to draw on
	SDL_Texture* texture;//the texture to draw on
	SDL_Surface* surface;//the surface to draw on
	int x;
	int y;
	int width;
	int height;
};
namespace RenderDraw
{
	
	bool Init(int width, int height, std::string title);
	void Render(RenderDrawInfo& info);
	void ClearScreen(SDL_Color color);
	void Shutdown(RenderDrawInfo& info);
	char* GetSurfacePointer();
	void SDL_Draw(RenderDrawInfo& info);
	char* GetPixels();
	void SetPixels(char* pixels);

}
extern RenderDrawInfo g_renderDrawInfo;