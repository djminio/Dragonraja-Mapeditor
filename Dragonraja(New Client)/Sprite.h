#pragma once
#include <cstdint>
#include <Windows.h>
typedef struct {
	short xl, yl;
	short ox, oy;
	uint32_t	size;
	char* img;
}Spr;

typedef struct tagSOU2
{
	short	tile_no;
	char	source_no;

} SOU2, * LPSOU2;
extern void PutTile(int x, int y, LPVOID Stmp, LPVOID dest);
extern int  convert565to555(Spr* sp);
extern void convert565to555RawTile(char* img);
extern void PutCompressedImage(int x, int y, Spr* sp);
extern void PlaceTile(int x, int y, void* src, void* dst);
extern void DrawMapTile(int x, int y, Spr& spr);
extern DWORD dDxSize;
void PutCompressedImages(int x, int y, Spr* sp, LPVOID Stmp, LPVOID Dest, int dDxSizex);
extern void PutTiles(int x, int y, LPVOID Stmp, LPVOID dest, int dDxSizex);
extern  void PutTileNotClipingTest(
	int x,
	int y,
	void* Stmp,
	void* dest,
	int dDxSizex);

struct PixelInfo
{
	DWORD	BitMaskR;
	DWORD	BitMaskG;
	DWORD	BitMaskB;

	DWORD	ShiftRightR;
	DWORD	ShiftRightG;
	DWORD	ShiftRightB;

	DWORD	ShiftLeftR;
	DWORD	ShiftLeftG;
	DWORD	ShiftLeftB;
};
extern void	InitSpriteTransTable();
extern PixelInfo _PixelInfo;// Remove the typedef struct { ... }Spr; and use a single struct definition for Spr


