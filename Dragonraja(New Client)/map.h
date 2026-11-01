#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include"Sprite.h"
#define TILE_MAX_   3600
#define HOUSETILE_MAX_	1200
#define TILE_SIZE 32
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1020
extern int TotalTileNo;
typedef struct	tagMAPFILE
{
	uint16_t		wType;
	uint16_t		wWidth, wHeight;
	char		cOdd, cEven;
} MAPFILE, * LPMAPFILE;

typedef struct	tagMAP
{
	int				x, y;
	uint16_t			wScreenWidth, wScreenHeight;
	MAPFILE			file;
	int				tox, toy;
	int				oX, oY;							//타일 기준 좌표와 함께 도트단위 표현을 위한 맵옵셋 좌표
	int             display_X, display_Y;
	Spr				sp;
} MAP, * LPMAP;

typedef struct	tagTILE
{
	uint32_t	back : 12;
	uint32_t	front : 12;
	uint32_t	empty__1 : 1;
	uint32_t	rooftype : 1;
	uint32_t	attr_dont : 1;
	uint32_t	attr_map : 1;
	uint32_t	attr_room : 2;
	uint32_t	empty__2 : 1;
	uint32_t	attr_skill : 1;

	uint32_t	roof : 12;
	uint32_t	show_roof : 1;
	uint32_t	frontoffset : 10;
	uint32_t	occupied : 1;
	uint32_t	canmatch : 1;
	uint32_t	empty__4 : 1;
	uint32_t	attr_light : 1;
	uint32_t	attr_inside : 1;
	uint32_t attr_river : 1;
	uint32_t	attr_magic : 1;
	uint32_t	attr_no_battle : 1;
	uint32_t	empty__6 : 1;
} TILE, * LPTILE;

extern MAP					g_Mapinfo;
extern bool ReadMapFile(const char* filename, LPMAPFILE map);
extern void LoadMap(LPMAP lpmap, std::string mapname);
extern std::vector<TILE> g_tileMap;
extern void MapDraw(int nType);
extern void LoadMapSource(std::string mapname);
extern void  MapDisplay(int nType);
extern uint32_t ReturnSizeOfSource2(char* source2name);
extern char* g_DestBackBuf, * g_OldDestBackBuf;
extern Spr Sou[3600];