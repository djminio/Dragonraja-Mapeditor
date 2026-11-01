#include "SDLRenderer.h"
#include "main.h"
#include "map.h"
RenderDrawInfo g_renderDrawInfo;
uint16_t* framebuffer;
char* m_pixels;



bool RenderDraw::Init(int width, int height, std::string title)
{

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL Init Error: %s\n", SDL_GetError());
		return false;
	}


	g_renderDrawInfo.window =
		SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width,
			height,
			SDL_WINDOW_SHOWN
		);


	if (!g_renderDrawInfo.window)
	{
		printf("Window Error: %s\n", SDL_GetError());
		return false;
	}



	g_renderDrawInfo.renderer =
		SDL_CreateRenderer(
			g_renderDrawInfo.window,
			-1,
			SDL_RENDERER_ACCELERATED |
			SDL_RENDERER_PRESENTVSYNC
		);


	if (!g_renderDrawInfo.renderer)
	{
		printf("Renderer Error: %s\n", SDL_GetError());
		return false;
	}



	// Allows old game coordinates to stay 1280x720
	SDL_RenderSetLogicalSize(
		g_renderDrawInfo.renderer,
		SCREEN_WIDTH,
		SCREEN_HEIGHT
	);



	g_renderDrawInfo.texture =
		SDL_CreateTexture(
			g_renderDrawInfo.renderer,
			SDL_PIXELFORMAT_RGB565,
			SDL_TEXTUREACCESS_STREAMING,
			SCREEN_WIDTH,
			SCREEN_HEIGHT
		);


	if (!g_renderDrawInfo.texture)
	{
		printf("Texture Error: %s\n", SDL_GetError());
		return false;
	}



	int texW, texH;

	SDL_QueryTexture(
		g_renderDrawInfo.texture,
		nullptr,
		nullptr,
		&texW,
		&texH
	);


	printf("Texture: %d x %d\n", texW, texH);
	printf("Screen : %d x %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);


	return true;
}

char* RenderDraw::GetPixels()
{
	return m_pixels;
}

void RenderDraw::SetPixels(char* pixels)
{
	m_pixels = pixels;
}
static SDL_Rect g_renderRect = { 0, 0, 0, 0 };
void RenderDraw::Render(RenderDrawInfo& info)
{

	if (!info.renderer || !info.texture)
		return;


	/*
		g_DestBackBuf is your old DirectDraw surface

		uint16_t*
		SCREEN_WIDTH * SCREEN_HEIGHT
	*/


	//SDL_UpdateTexture(
	//	info.texture,
	//	nullptr,
	//	g_DestBackBuf,
	//	SCREEN_WIDTH * sizeof(uint16_t)
	//);



	//SDL_RenderClear(info.renderer);



	//SDL_RenderCopy(
	//	info.renderer,
	//	info.texture,
	//	nullptr,
	//	nullptr
	//);



	SDL_RenderPresent(info.renderer);

}
void RenderDraw::ClearScreen(SDL_Color color)
{
	/*char* pixels = GetPixels();

	if (pixels)
	{
		uint16_t rgb565 =
			((color.r >> 3) << 11) |
			((color.g >> 2) << 5) |
			(color.b >> 3);

		uint16_t* buffer = (uint16_t*)pixels;

		for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
		{
			buffer[i] = rgb565;
		}
	}*/

	SDL_SetRenderDrawColor(
		g_renderDrawInfo.renderer,
    color.r,
		color.g,
		color.b,
		color.a
);

SDL_RenderClear(g_renderDrawInfo.renderer);
}

void RenderDraw::Shutdown(RenderDrawInfo& info)
{
		if (info.texture)
		{
			SDL_DestroyTexture(info.texture);
			info.texture = nullptr;
		}

		if (info.renderer)
		{
			SDL_DestroyRenderer(info.renderer);
			info.renderer = nullptr;
		}

		if (info.window)
		{
			SDL_DestroyWindow(info.window);
			info.window = nullptr;
		}

		if (m_pixels)
		{
			free(m_pixels);
			m_pixels = nullptr;
		}
	}

char* RenderDraw::GetSurfacePointer()
{
	char* pixels = GetPixels();
	if (pixels != NULL) return pixels;

	int w = SCREEN_WIDTH;
	int h = SCREEN_HEIGHT;

	pixels = (char*)malloc(w * h * 2); // ✅ FIXED

	if (pixels == NULL)
	{
		printf("SDL_GetSurfacePointer: malloc failed\n");
		return NULL;
	}

	memset(pixels, 0, w * h * 2); // ✅ FIXED
	SetPixels(pixels);
	return pixels;
}

void RenderDraw::SDL_Draw(RenderDrawInfo& info)
{
	char* pixels = GetPixels();
	if (!info.renderer || !info.texture || !pixels) return;
	// 1. Upload game pixels to the texture
	SDL_UpdateTexture(info.texture, NULL, pixels, SCREEN_WIDTH * 2);

	// 2. Copy the texture to the centered rectangle
	SDL_RenderCopy(info.renderer, info.texture, NULL, &g_renderRect);
}
