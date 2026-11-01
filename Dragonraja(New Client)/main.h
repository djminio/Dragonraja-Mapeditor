#pragma once
#include"stdafx.h"
#include<string>
#define SDL_MAIN_HANDLED
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GAME_TITLE "Dragonraja x64"
#define GAME_VERSION "1.0.0"
#define GAME_SCREEN_XSIZE	1600
#define GAME_SCREEN_YSIZE	900// 35
extern std::string path;
extern std::string basePath;
extern bool isQuit;
extern void DisplaySpriteBottom();
extern std::string Mapname;
extern DWORD				g_Inside;
// Before using 'basepath', declare and initialize it appropriately.
// Example fix (assuming basepath should be a std::string containing the base directory):
