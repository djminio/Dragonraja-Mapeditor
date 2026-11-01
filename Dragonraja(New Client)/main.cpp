// =====================================================================
// main.cpp - Dragon Raja Map Editor (x86 and x64)
//   SDL2 window + renderer, Dear ImGui with the Raja Management Tool theme, the editor (Editor.cpp).
//   Start it with a map to open it straight away:  "Dragonraja(Map Editor).exe" D:\MoonClient\map\renus_c.map
// =====================================================================
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <string>
#include "Editor.h"
#include "Theme.h"
#include "DragonIcon.h"


int main(int argc, char* argv[])
{
	SDL_SetMainReady();
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");		// tiles stay sharp when zoomed
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) { SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Map Editor", SDL_GetError(), nullptr); return 1; }
	SDL_Window* win = SDL_CreateWindow("Dragon Raja Map Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MAXIMIZED);
	if (!win) { SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Map Editor", SDL_GetError(), nullptr); return 1; }
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!ren) ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);		// no GPU (remote desktop)
	if (!ren) { SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Map Editor", SDL_GetError(), win); return 1; }

	// the red Dragon Raja dragon: window / taskbar icon and the empty-view picture
	if (SDL_Surface* icon = SDL_CreateRGBSurfaceWithFormatFrom((void*)DRAGON_ICON_RGBA, DRAGON_ICON_W, DRAGON_ICON_H, 32, DRAGON_ICON_W * 4, SDL_PIXELFORMAT_RGBA32))
	{
		SDL_SetWindowIcon(win, icon);
		if (SDL_Texture* t = SDL_CreateTextureFromSurface(ren, icon)) { SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND); Theme::Dragon = (ImTextureID)(intptr_t)t; }
		SDL_FreeSurface(icon);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	Theme::Apply();
	ImGui_ImplSDL2_InitForSDLRenderer(win, ren);
	ImGui_ImplSDLRenderer2_Init(ren);
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	Editor::Init(win, ren);
	if (argc > 1 && argv[1] && argv[1][0]) Editor::OpenMap(argv[1]);

	while (!Editor::WantsQuit())
	{
		SDL_Event e;
		const Uint32 flags = SDL_GetWindowFlags(win);
		if (flags & SDL_WINDOW_MINIMIZED) { if (SDL_WaitEventTimeout(&e, 200)) { ImGui_ImplSDL2_ProcessEvent(&e); Editor::HandleEvent(e); } continue; }
		if (!(flags & SDL_WINDOW_INPUT_FOCUS) && SDL_WaitEventTimeout(&e, 50)) { ImGui_ImplSDL2_ProcessEvent(&e); Editor::HandleEvent(e); }	// in the background: ~20 fps
		while (SDL_PollEvent(&e)) { ImGui_ImplSDL2_ProcessEvent(&e); Editor::HandleEvent(e); }		// in front: every frame (vsync), no waiting

		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		Editor::Frame();
		ImGui::Render();
		SDL_SetRenderDrawColor(ren, 12, 8, 6, 255);
		SDL_RenderClear(ren);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), ren);
		SDL_RenderPresent(ren);
	}

	Editor::Shutdown();
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	if (Theme::Dragon) SDL_DestroyTexture((SDL_Texture*)(intptr_t)Theme::Dragon);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
