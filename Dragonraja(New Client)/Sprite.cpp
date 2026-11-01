#include "Sprite.h"
#include"map.h"
DWORD dDxSize= 3840;
DWORD BitMaskR = 0xF800;
#include <cstdint>
#include <cstring>
#include<unordered_map>
WORD	AddTableR[128];
WORD	AddTableG[256];
WORD	AddTableB[128];

WORD	SubTableR[64];
//WORD	SubTableg[64];
WORD	SubTableG[128];
WORD	SubTableB[64];

PixelInfo _PixelInfo;
void PutCompressedImageFX(int x, int y, Spr* sp, DWORD alpha, DWORD op);
void SDL_CheckColorBitMask()
{

	int pitch = 1600;

		//window mode
		dDxSize = SCREEN_WIDTH * 2;
		dDxSize = SCREEN_WIDTH * 2;



	_PixelInfo.BitMaskR = 0xF800;

	if (_PixelInfo.BitMaskR == 0xF800)
	{
		// 요거이 RGB 5:6:5 포맷이니끼리 그렇게 알라우.
		//			 4444----3333----2222----1111----
		//           84218421842184218421842184218421
		//           00000000000000011111011111011110;
		_PixelInfo.ShiftRightR = 11;
		_PixelInfo.ShiftLeftR = 0;

		_PixelInfo.ShiftRightG = 5;
		_PixelInfo.ShiftLeftG = 5;

		_PixelInfo.ShiftRightB = 0;
		_PixelInfo.ShiftLeftB = 11;
	}
	else {
		if (_PixelInfo.BitMaskR == 0x7C00)
		{
			// 요고 RGB 5:5:5 포맷 아이가.
			//			   4444----3333----2222----1111----
			//             84218421842184218421842184218421
			//             00000000000000001111101111011110;
			_PixelInfo.ShiftRightR = 10;
			_PixelInfo.ShiftLeftR = 1;

			_PixelInfo.ShiftRightG = 5;
			_PixelInfo.ShiftLeftG = 6;

			_PixelInfo.ShiftRightB = 0;
			_PixelInfo.ShiftLeftB = 11;
		}
		else {
			if (_PixelInfo.BitMaskR == 0x001F)
			{
				// 아따 요것 BGR 5:6:5 포맷이것제.
				//			   4444----3333----2222----1111----
				//             84218421842184218421842184218421
				_PixelInfo.ShiftRightR = 0;
				_PixelInfo.ShiftLeftR = 11;

				_PixelInfo.ShiftRightG = 5;
				_PixelInfo.ShiftLeftG = 5;

				_PixelInfo.ShiftRightB = 11;
				_PixelInfo.ShiftLeftB = 0;
			}
		}
	}
}
void	InitSpriteTransTable()
{

	SDL_CheckColorBitMask();

	WORD	MaskG;
	if (_PixelInfo.BitMaskR == 0x7c00)
	{
		MaskG = 0x1f;
	}
	else
	{
		MaskG = 0x3f;
	}

	//Make ADD Table
	int i;
	for (i = 0; i < 128; i++) {
		if (i < 32)
		{
			AddTableR[i] = i << _PixelInfo.ShiftRightR;
			AddTableB[i] = i << _PixelInfo.ShiftRightB;
		}
		else
		{
			AddTableR[i] = 0x1f << _PixelInfo.ShiftRightR;
			AddTableB[i] = 0x1f << _PixelInfo.ShiftRightB;
		}
	}
	for (i = 0; i < 256; i++) {
		if (i <= MaskG)
		{
			AddTableG[i] = i << _PixelInfo.ShiftRightG;
		}
		else
		{
			AddTableG[i] = MaskG << _PixelInfo.ShiftRightG;
		}
	}

	//Make SUB Table
	for (i = 0; i < 64; i++) {
		if (i < 32)
		{
			SubTableR[i] = 0;
			SubTableB[i] = 0;
		}
		else
		{
			SubTableR[i] = (i - 32) << _PixelInfo.ShiftRightR;
			SubTableB[i] = (i - 32) << _PixelInfo.ShiftRightB;
		}
		//SubTableG[i] = i << _PixelInfo.ShiftRightG;
		//SubTableg[i] = 0;
	}
	for (i = 0; i < 128; i++) {
		if (i < 64)
		{
			SubTableG[i] = 0;
		}
		else
		{
			SubTableG[i] = (i - 64) << _PixelInfo.ShiftRightG;
		}
	}
}
#include <SDL2/SDL.h>
#include "SDLRenderer.h"
std::unordered_map<char*, SDL_Texture*> m_textures;		// sprite pixels -> its GPU texture
//void PlaceTile(int x, int y, void* src, void* dst)
//{
//	if (!src || !dst)
//		return;
//
//	const uint16_t* tile = static_cast<const uint16_t*>(src);
//	uint16_t* framebuffer = static_cast<uint16_t*>(dst);
//
//	// Destination start
//	framebuffer += y * dDxSize + (x * 2);
//
//	for (int row = 0; row < 32; row++)
//	{
//		uint16_t* out = framebuffer;
//
//		for (int col = 0; col < 32; col++)
//		{
//			uint16_t pixel = *tile++;
//
//			// Double each pixel horizontally
//			*out++ = pixel;
//			*out++ = pixel;
//		}
//
//		framebuffer += dDxSize;
//	}
//}
void PutTileCliping(int x, int y, LPVOID Stmp, LPVOID dest, int lx, int rx, int ty, int by);
void DrawMapTile(int x, int y, Spr& spr)
{
	if (!spr.img)
		return;

	// Raw 32x32 RGB565 tile
	if (spr.xl == -1)
	{
		// Fully visible tile
		if (x >= 0 &&
			y >= 0 &&
			x + TILE_SIZE <= SCREEN_WIDTH &&
			y + TILE_SIZE <= SCREEN_HEIGHT)
		{
			PutTile(
				x,
				y,
				spr.img,
				g_DestBackBuf
			);
		}
		else
		{
			// Partial tile at screen edge
			PutTileCliping(
				x,
				y,
				spr.img,
				g_DestBackBuf,
				0,
				TILE_SIZE,
				0,
				TILE_SIZE
			);
		}
	}
	else
	{
		// Compressed tile
		PutCompressedImage(
			x,
			y,
			&spr
		);
	}
}
void PutTileCliping(int x, int y, LPVOID Stmp, LPVOID dest, int lx, int rx, int ty, int by)
{
	uint16_t* src = reinterpret_cast<uint16_t*>(Stmp);
	uint16_t* dst = reinterpret_cast<uint16_t*>(dest);


	for (int row = ty; row < by; row++)
	{
		memcpy(
			dst + ((y + row - ty) * SCREEN_WIDTH + x),
			src + (row * TILE_SIZE + lx),
			(rx - lx) * sizeof(uint16_t)
		);
	}
}

#include "main.h"
#include <iostream>

void convert565to555RawTile(char* img)
{
	int i;
	WORD* p;

	if (BitMaskR != 0x7C00)		return;

	p = (WORD*)img;

	for (i = 0; i < 32 * 32; i++)
	{
		*p = ((*p) & 0xFFC0) >> 1 | ((*p) & 0x1F);
		p++;
	}
}
void	PutCmprsImgCliping(int x, int y, LPVOID Stmp, LPVOID dest, int lx, int rx, int ty, int by)
{
	auto* src = static_cast<uint8_t*>(Stmp);
	auto* dstBase = static_cast<uint8_t*>(dest);

	lx *= 2;
	rx *= 2;

	// Skip top clipped rows
	for (int row = 0; row < ty; row++)
	{
		while (true)
		{
			uint16_t skip = *reinterpret_cast<uint16_t*>(src);

			if (skip == 0xFFFF)
			{
				src += 2;
				break;
			}

			uint16_t bytes = *reinterpret_cast<uint16_t*>(src + 2);

			src += 4;
			src += bytes;
		}
	}

	for (int row = 0; row < by; row++)
	{
		uint8_t* dst = dstBase + (y + row) * dDxSize + x * 2;

		int xpos = 0;

		while (true)
		{
			uint16_t skip = *reinterpret_cast<uint16_t*>(src);

			if (skip == 0xFFFF)
			{
				src += 2;
				break;
			}

			uint16_t bytes = *reinterpret_cast<uint16_t*>(src + 2);

			src += 4;

			xpos += skip;
			dst += skip;

			int copyStart = xpos;
			int copyEnd = xpos + bytes;

			if (copyEnd > lx && copyStart < rx)
			{
				int leftClip = max(copyStart, lx);
				int rightClip = min(copyEnd, rx);

				int copyBytes = rightClip - leftClip;

				if (copyBytes > 0)
				{
					memcpy(
						dst + (leftClip - copyStart),
						src + (leftClip - copyStart),
						copyBytes);
				}
			}

			dst += bytes;
			xpos += bytes;
			src += bytes;
		}
	}
}
void PutCmprsImgNOTCliping(int x, int y, int yl, void* Stmp, void* dest)
{
	if (!Stmp || !dest || yl <= 0) {
		std::cerr << "Invalid input to PutCmprsImgNOTCliping\n";
		return;
	}

	BYTE* src = static_cast<BYTE*>(Stmp);
	BYTE* baseDest = static_cast<BYTE*>(dest) + y * dDxSize + (x * 2);

	for (int row = 0; row < yl; ++row)
	{
		BYTE* destPtr = baseDest;

		while (true)
		{
			if (*(WORD*)src == 0xFFFF) {
				src += 2;
				break;
			}

			// Skip pixels
			WORD skip = *(WORD*)src;
			src += 2;
			destPtr += skip;

			// Number of bytes to copy
			WORD count = *(WORD*)src;
			src += 2;

			if (count == 0) continue;

			// Copy pixels
			std::memcpy(destPtr, src, count);
			destPtr += count;
			src += count;
		}

		baseDest += dDxSize;
	}
}
void PutCompressedImage(int x, int y, Spr* sp)
{
	int		lx, rx, ty, by, IsC = 0;

	if (sp == NULL) return;
	if (sp->img == NULL) return;

	int xl = sp->xl;
	int yl = sp->yl;

	if (xl == 0) return;
	if (yl == 0) return;

	x -= sp->ox;
	y -= sp->oy;


	if (x >= SCREEN_WIDTH) return;
	else if (x + xl < 0) return;

	if (y >= SCREEN_HEIGHT) return;
	else if (y + yl < 0) return;

	rx = xl;
	lx = 0;
	ty = 0;
	by = yl;



	if (x + xl >= (SCREEN_WIDTH - 1))
	{
		rx -= x + xl - (SCREEN_HEIGHT - 1);
		IsC = 1;
	}
	if (x < 0)
	{
		lx = 0 - x;
		x = 0;
		rx -= lx;
		IsC = 1;
	}

	if (y + yl >= SCREEN_HEIGHT)
	{
		by -= y + yl - SCREEN_HEIGHT;
		IsC = 1;
	}
	if (y < 0)
	{
		ty = 0 - y;
		y = 0;
		by -= ty;
		IsC = 1;
	}


	if (sp->img == NULL) return;


	if (IsC)
	{
		PutCmprsImgCliping(x, y, sp->img, g_DestBackBuf, lx, rx + 1, ty, by);
	}
	else
	{
		PutCmprsImgNOTCliping(x, y, sp->yl, sp->img, g_DestBackBuf);
	}
}
int  convert565to555(Spr* sp)
{
	int FFFFCheak = 0;
	int ConvertCount;
	int i;
	WORD* p;
	int tt;


	if (BitMaskR != 0x7C00)
	{
		return 0;
	}

	if (sp->xl == 0) return -1;
	if (sp->yl == 0) return -2;
	if (sp->size == 0) return -3;

	p = (WORD*)sp->img;

	while (1)
	{
	AGAIN_:
		tt = 0;
		if (*p == 0xFFFF)
		{
			tt = *p;
			FFFFCheak++;
			p++;

			if (FFFFCheak == sp->yl) break;
		}

		if (tt == 0xffff) goto AGAIN_;

		p++;   
		ConvertCount = *p;   
		p++;    

		for (i = 0; i < ConvertCount; i += 2)
		{
			*p = ((*p) & 0xFFC0) >> 1 | ((*p) & 0x1F);
			p++;
		}

		if (*p == 0xFFFF)
		{
			FFFFCheak++;
			p++;

			if (FFFFCheak == sp->yl) break;
		}
	}

	return 0;
}
void PutTileNotCliping(int x, int y, LPVOID Stmp, LPVOID dest)
{
	uint8_t* src = static_cast<uint8_t*>(Stmp);
	uint8_t* dst = static_cast<uint8_t*>(dest);

	uint8_t* dtmp = dst + y * dDxSize + x * 2;

	for (int row = 0; row < 32; row++)
	{
		memcpy(dtmp, src, 64);   // copy one 32x1 RGB565 scanline

		src += 64;               // next source scanline (64 bytes)
		dtmp += dDxSize;         // next destination scanline (pitch in bytes)
	}

}
void PutTileNotClipingTest(
	int x,
	int y,
	 void* Stmp,
	void *dest,
	int dDxSizex)
{
	uint8_t* src = static_cast<uint8_t*>(Stmp);
	uint8_t* dst = static_cast<uint8_t*>(dest);

	uint8_t* dtmp = dst + y * dDxSizex + x * 2;

	for (int row = 0; row < 32; row++)
	{
		memcpy(dtmp, src, 64);   // copy one 32x1 RGB565 scanline

		src += 64;               // next source scanline (64 bytes)
		dtmp += dDxSizex;         // next destination scanline (pitch in bytes)
	}

}
#include <cstdint>

// Performs 16-bit RGB 555 alpha blending
// alpha range: 0 (completely background) to 32 (completely foreground)
inline uint16_t BlendPixel555(uint16_t src, uint16_t dst, uint32_t alpha)
{
	// Extract Red (bits 10-14), Green (bits 5-9), Blue (bits 0-4)
	uint32_t sR = (src >> 10) & 0x1F;
	uint32_t sG = (src >> 5) & 0x1F;
	uint32_t sB = src & 0x1F;

	uint32_t dR = (dst >> 10) & 0x1F;
	uint32_t dG = (dst >> 5) & 0x1F;
	uint32_t dB = dst & 0x1F;

	// Linear interpolation: dst + ((src - dst) * alpha) / 32
	uint32_t r = dR + (((sR - dR) * alpha) >> 5);
	uint32_t g = dG + (((sG - dG) * alpha) >> 5);
	uint32_t b = dB + (((sB - dB) * alpha) >> 5);

	return static_cast<uint16_t>((r << 10) | (g << 5) | b);
}
void PutCompressedImages(int x, int y, Spr* sp, LPVOID Stmp, LPVOID Dest, int dDxSizex)
{
	int		lx, rx, ty, by, IsC = 0;

	if (sp == NULL)
	{
		//printf("sp is null\n");
		return;
	}
	if (sp->img == NULL)
	{
		//printf("img is null\n");
		return;
	}

	int xl = sp->xl;
	int yl = sp->yl;

	if (xl == 0)
	{
		//printf("sp width is 0\n");
		return;
	}
	if (yl == 0)
	{
		//printf("sp height is 0\n");
		return;
	}

	x -= sp->ox;
	y -= sp->oy;


	if (x >= SCREEN_WIDTH)
	{
		//printf("x is greater than screen_width\n");
		return;
	}

	else if (x + xl < 0) return;

	if (y >= SCREEN_HEIGHT)
	{
		//printf("y is greater than screen_heighth\n");
		return;
	}
	else if (y + yl < 0) return;

	rx = xl;
	lx = 0;
	ty = 0;
	by = yl;



	if (x + xl >= (SCREEN_WIDTH - 1))
	{
		rx -= x + xl - (SCREEN_WIDTH - 1);
		IsC = 1;
	}
	if (x < 0)
	{
		lx = 0 - x;
		x = 0;
		rx -= lx;
		IsC = 1;
	}

	if (y + yl >= SCREEN_HEIGHT)
	{
		by -= y + yl - SCREEN_HEIGHT;
		IsC = 1;
	}
	if (y < 0)
	{
		ty = 0 - y;
		y = 0;
		by -= ty;
		IsC = 1;
	}


	if (sp->img == NULL) return;


	if (IsC)
	{

		PutCmprsImgCliping(x, y, sp->img, g_DestBackBuf, lx, rx + 1, ty, by);
	}
	else
	{
		PutTileNotClipingTest(x, y, Stmp, Dest, dDxSizex);

	}
}
void PutTiles(int x, int y, LPVOID Stmp, LPVOID dest, int dDxSizex) {
	int lx, rx, ty, by, IsC = 0;
	int xl = TILE_SIZE;
	int yl = TILE_SIZE;

	if (x >= SCREEN_WIDTH) return;
	else if (x + xl < 0) return;
	if (y >= SCREEN_HEIGHT) return;
	else if (y + yl < 0) return;

	rx = xl;
	lx = 0;
	ty = 0;
	by = yl;

	if (x + xl > SCREEN_WIDTH) {
		rx -= x + xl - SCREEN_WIDTH;
		IsC = 1;
	}
	if (x < 0) {
		lx = 0 - x;
		x = 0;
		IsC = 1;
	}
	if (y + yl > SCREEN_HEIGHT) {
		by -= y + yl - SCREEN_HEIGHT;
		IsC = 1;
	}
	if (y < 0) {
		ty = 0 - y;
		y = 0;
		IsC = 1;
	}


	if (IsC) {
		//PutTileClipingTest(x, y, Stmp, dest, lx, rx, ty, by, dDxSizex);
		PutTileCliping(x, y, Stmp, dest, lx, rx, ty, by);
	}
	else {
		PutTileNotClipingTest(x,y,dest, Stmp, dDxSizex);
		//PutTileNotCliping(x,y,dest, Stmp);
	}
}
void PutTile(int x, int y, LPVOID Stmp, LPVOID dest)
{
	int	lx, rx, ty, by, IsC = 0;
	int xl = TILE_SIZE;
	int yl = TILE_SIZE;

	if (x >= SCREEN_WIDTH)			return;
	else if (x + xl < 0)	return;
	if (y >= SCREEN_HEIGHT)			return;
	else if (y + yl < 0)	return;

	rx = xl;
	lx = 0;
	ty = 0;
	by = yl;

	if (x + xl > SCREEN_WIDTH)
	{
		rx -= x + xl - SCREEN_WIDTH;
		IsC = 1;
	}
	if (x < 0)
	{
		lx = 0 - x;
		x = 0;
		//		rx -= lx;
		IsC = 1;
	}
	if (y + yl > SCREEN_HEIGHT)
	{
		by -= y + yl - SCREEN_HEIGHT;
		IsC = 1;
	}
	if (y < 0)
	{
		ty = 0 - y;
		y = 0;
		//		by -= ty;
		IsC = 1;
	}

	if (IsC)		PutTileCliping(x, y, Stmp, dest, lx, rx, ty, by);
	else 		PutTileNotCliping(x, y, Stmp, dest);
}
void PutCmprsImgNOTClipingBlend555(int x, int y, int yl, void* Stmp, void* dest, uint32_t alpha)
{
	if (!Stmp || !dest || yl <= 0) return;

	// dDxSize represents surface stride/pitch in bytes


	uint8_t* pDstRow = static_cast<uint8_t*>(dest) + y * dDxSize + (x * 2);
	const uint8_t* pSrc = static_cast<const uint8_t*>(Stmp);

	for (int line = 0; line < yl; ++line)
	{
		uint16_t* pDstTile = reinterpret_cast<uint16_t*>(pDstRow);

		while (true)
		{
			uint16_t skipBytes = *reinterpret_cast<const uint16_t*>(pSrc);

			// 0xFFFF signals End of Line (EOL) in RLE compression
			if (skipBytes == 0xFFFF)
			{
				pSrc += 2;
				break;
			}

			pDstTile += (skipBytes / 2); // Skip empty space

			uint16_t dataBytes = *reinterpret_cast<const uint16_t*>(pSrc + 2);
			pSrc += 4; // Move past header (skip, size)

			int pixelCount = dataBytes / 2;
			const uint16_t* pSrcPixels = reinterpret_cast<const uint16_t*>(pSrc);

			for (int p = 0; p < pixelCount; ++p)
			{
				pDstTile[p] = BlendPixel555(pSrcPixels[p], pDstTile[p], alpha);
			}

			pDstTile += pixelCount;
			pSrc += dataBytes; // Advance source past pixels
		}

		pDstRow += dDxSize; // Move destination pointer to next screen line
	}
}
void PutCmprsImgClipingBlend555(int x, int y, void* Stmp, void* dest, int lx, int rx, int ty, int by, uint32_t alpha)
{
	if (!Stmp || !dest || by <= 0) return;

	//extern int wDxSize; // Surface pitch (width in bytes)


	uint8_t* pDstRow = static_cast<uint8_t*>(dest) + y * dDxSize + (x * 2);
	const uint8_t* pSrc = static_cast<const uint8_t*>(Stmp);

	// Convert pixel coordinates/bounds to byte offsets (2 bytes per pixel)
	int lxBytes = lx * 2;
	int rxBytes = rx * 2;

	// 1. Skip top non-visible clipped lines (ty)
	for (int line = 0; line < ty; ++line)
	{
		while (true)
		{
			uint16_t skip = *reinterpret_cast<const uint16_t*>(pSrc);
			if (skip == 0xFFFF) // End of line indicator
			{
				pSrc += 2;
				break;
			}
			uint16_t length = *reinterpret_cast<const uint16_t*>(pSrc + 2);
			pSrc += 4 + length;
		}
	}

	// 2. Render visible clipped lines (by)
	for (int line = 0; line < by; ++line)
	{
		uint8_t* pDst = pDstRow;
		int currentXBytes = 0; // Tracks horizontal byte offset in the current line

		while (true)
		{
			uint16_t skipBytes = *reinterpret_cast<const uint16_t*>(pSrc);
			if (skipBytes == 0xFFFF)
			{
				pSrc += 2;
				break; // End of line
			}

			pSrc += 2;
			uint16_t dataBytes = *reinterpret_cast<const uint16_t*>(pSrc);
			pSrc += 2;

			int runStartBytes = currentXBytes + skipBytes;
			int runEndBytes = runStartBytes + dataBytes;

			// Check if chunk is within horizontal clip region [lxBytes, rxBytes)
			if (runEndBytes > lxBytes && runStartBytes < rxBytes)
			{
				// Determine clip bounds for this specific pixel chunk
				int drawStartBytes = runStartBytes;
				int drawEndBytes = runEndBytes;

				if (drawStartBytes < lxBytes) drawStartBytes = lxBytes;
				if (drawEndBytes > rxBytes)   drawEndBytes = rxBytes;

				int bytesToDraw = drawEndBytes - drawStartBytes;
				if (bytesToDraw > 0)
				{
					int srcOffsetBytes = drawStartBytes - runStartBytes;
					int dstOffsetBytes = drawStartBytes - lxBytes;

					const uint16_t* pSrcPix = reinterpret_cast<const uint16_t*>(pSrc + srcOffsetBytes);
					uint16_t* pDstPix = reinterpret_cast<uint16_t*>(pDst + dstOffsetBytes);

					int pixelCount = bytesToDraw / 2;
					for (int p = 0; p < pixelCount; ++p)
					{
						pDstPix[p] = BlendPixel555(pSrcPix[p], pDstPix[p], alpha);
					}
				}
			}

			// Move source pointer past data block
			pSrc += dataBytes;
			currentXBytes = runEndBytes;
		}

		pDstRow += dDxSize; // Advance destination to next screen line
	}
}
#include <cstdint>
#include <algorithm>




// Additive Blending using Lookup Tables
inline uint16_t BlendAdd555_Table(uint16_t src, uint16_t dst, uint32_t alpha)
{
	uint32_t sR = (src >> 10) & 0x1F;
	uint32_t sG = (src >> 5) & 0x1F;
	uint32_t sB = src & 0x1F;

	uint32_t dR = (dst >> 10) & 0x1F;
	uint32_t dG = (dst >> 5) & 0x1F;
	uint32_t dB = dst & 0x1F;

	// Apply alpha scaling to source color components
	uint32_t scaledR = (sR * alpha) >> 4;
	uint32_t scaledG = (sG * alpha) >> 4;
	uint32_t scaledB = (sB * alpha) >> 4;

	uint32_t sumR = scaledR + dR;
	uint32_t sumG = scaledG + dG;
	uint32_t sumB = scaledB + dB;

	return AddTableB[sumB & 0xFF] | AddTableG[sumG & 0xFF] | AddTableR[sumR & 0xFF];
}

// Subtractive Blending using Lookup Tables
inline uint16_t BlendSub555_Table(uint16_t src, uint16_t dst, uint32_t alpha)
{
	int sR = (src >> 10) & 0x1F;
	int sG = (src >> 5) & 0x1F;
	int sB = src & 0x1F;

	int dR = (dst >> 10) & 0x1F;
	int dG = (dst >> 5) & 0x1F;
	int dB = dst & 0x1F;

	int scaledR = (sR * static_cast<int>(alpha)) >> 4;
	int scaledG = (sG * static_cast<int>(alpha)) >> 4;
	int scaledB = (sB * static_cast<int>(alpha)) >> 4;

	// Match original signed subtraction with table offset clamping
	int resB = dB - scaledB;
	int resG = dG - scaledG;
	int resR = dR - scaledR;

	return SubTableB[64 + (resB & 0xFF)] |
		SubTableG[128 + (resG & 0xFF)] |
		SubTableR[64 + (resR & 0xFF)];
}

// Pure Math Alternative (Use this if you do not want to use Lookup Tables)
inline uint16_t BlendAdd555_Direct(uint16_t src, uint16_t dst, uint32_t alpha)
{
	uint32_t sR = ((src >> 10) & 0x1F) * alpha >> 4;
	uint32_t sG = ((src >> 5) & 0x1F) * alpha >> 4;
	uint32_t sB = (src & 0x1F) * alpha >> 4;

	uint32_t r = std::min<uint32_t>(31, ((dst >> 10) & 0x1F) + sR);
	uint32_t g = std::min<uint32_t>(31, ((dst >> 5) & 0x1F) + sG);
	uint32_t b = std::min<uint32_t>(31, (dst & 0x1F) + sB);

	return static_cast<uint16_t>((r << 10) | (g << 5) | b);
}

inline uint16_t BlendSub555_Direct(uint16_t src, uint16_t dst, uint32_t alpha)
{
	int sR = ((src >> 10) & 0x1F) * static_cast<int>(alpha) >> 4;
	int sG = ((src >> 5) & 0x1F) * static_cast<int>(alpha) >> 4;
	int sB = (src & 0x1F) * static_cast<int>(alpha) >> 4;

	int r = std::max<int>(0, ((dst >> 10) & 0x1F) - sR);
	int g = std::max<int>(0, ((dst >> 5) & 0x1F) - sG);
	int b = std::max<int>(0, (dst & 0x1F) - sB);

	return static_cast<uint16_t>((r << 10) | (g << 5) | b);
}
void PutCmprsImgClipingAdd555(int x, int y, void* Stmp, void* dest, int lx, int rx, int ty, int by, uint32_t alpha)
{
	if (!Stmp || !dest || by <= 0) return;

	
	//extern int dDxSize; // Stride (bytes per line)

	uint8_t* pDstRow = static_cast<uint8_t*>(dest) + y * dDxSize + (x * 2);
	const uint8_t* pSrc = static_cast<const uint8_t*>(Stmp);

	int lxBytes = lx * 2;
	int rxBytes = rx * 2;

	// 1. Skip top non-visible clipped lines (ty)
	for (int line = 0; line < ty; ++line)
	{
		while (true)
		{
			uint16_t skip = *reinterpret_cast<const uint16_t*>(pSrc);
			if (skip == 0xFFFF)
			{
				pSrc += 2;
				break;
			}
			uint16_t length = *reinterpret_cast<const uint16_t*>(pSrc + 2);
			pSrc += 4 + length;
		}
	}

	// 2. Render visible clipped lines (by)
	for (int line = 0; line < by; ++line)
	{
		uint8_t* pDst = pDstRow;
		int currentXBytes = 0;

		while (true)
		{
			uint16_t skipBytes = *reinterpret_cast<const uint16_t*>(pSrc);
			if (skipBytes == 0xFFFF)
			{
				pSrc += 2;
				break;
			}

			pSrc += 2;
			uint16_t dataBytes = *reinterpret_cast<const uint16_t*>(pSrc);
			pSrc += 2;

			int runStartBytes = currentXBytes + skipBytes;
			int runEndBytes = runStartBytes + dataBytes;

			if (runEndBytes > lxBytes && runStartBytes < rxBytes)
			{
				int drawStartBytes = max(runStartBytes, lxBytes);
				int drawEndBytes = min(runEndBytes, rxBytes);

				int bytesToDraw = drawEndBytes - drawStartBytes;
				if (bytesToDraw > 0)
				{
					int srcOffsetBytes = drawStartBytes - runStartBytes;
					int dstOffsetBytes = drawStartBytes - lxBytes;

					const uint16_t* pSrcPix = reinterpret_cast<const uint16_t*>(pSrc + srcOffsetBytes);
					uint16_t* pDstPix = reinterpret_cast<uint16_t*>(pDst + dstOffsetBytes);

					int pixelCount = bytesToDraw / 2;
					for (int p = 0; p < pixelCount; ++p)
					{
						pDstPix[p] = BlendAdd555_Table(pSrcPix[p], pDstPix[p], alpha);
					}
				}
			}

			pSrc += dataBytes;
			currentXBytes = runEndBytes;
		}

		pDstRow += dDxSize;
	}
}
#include <cstdint>
#include <algorithm>
#include "GameProc.h"
#include "MapObject.h"


//extern int dDxSize; // Line stride in bytes

//void DisplaySpriteBottom(void)
//{
//	register int	i;
//
//	for (i = 0; i < g_OrderInfo.count; i++)
//	{
//		//LPCHARACTER		ch = (LPCHARACTER)g_OrderInfo.order[i].lpvData;
//		LPORDERTABLE o = &g_OrderInfo.order[i];
//		switch (o->wType)
//		{
//		case SPRITETYPE_EFFECT_BOTTOM:
//		{
//			o->show = 0;
//			switch (o->type)
//			{
//			case 1:
//			case 2:
//			case 3:
//			case 4:
//				PutCompressedImageFX(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData), o->alpha, o->type);
//				break;
//			case 5:
//				PutCompressedImage(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData));
//				break;
//			case 6:
//				//PutCompressedImageBlendRGB(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData), o->rgb);
//				break;
//
//			case 7:
//				//PutCompressedImageVoid(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData));
//				break;
//
//			}
//			break;
//		}
//		case SPRITETYPE_ALPHAOBJECT:
//		case SPRITETYPE_OBJECT:
//		{
//			//				LPMAPOBJECT mo;
//			if (((LPMAPOBJECT)o->lpvData)->attr == OBJECTATTR_BACK_ABOVE)
//			{
//				o->show = 0;
//				DisplayMapObject((LPMAPOBJECT)o->lpvData);
//			}
//			break;
//		}
//		case SPRITETYPE_ITEM_BOTTOM:
//		{
//			o->show = 0;
//
//			LPITEMGROUND  item = (LPITEMGROUND)o->lpvData;
//			Spr* sp = item->spr[item->curframe / 2];
//
//			//if (item->shadow)	PutCompressedImageShadow(o->x - Mapx, o->y - Mapy, sp);
//			PutCompressedImage(o->x - Mapx + item->offx, o->y - Mapy + item->offy, sp);
//			if (o->rgb && item->getable)	PutCompressedImageFX(o->x - Mapx + item->offx, o->y - Mapy + item->offy, sp, o->alpha, 2);
//			//MaskMapItem(o->x + item->offx, o->y + item->offy, o->y, sp);
//		}
//		break;
//		}
//		//			case  SPRITETYPE_DEST_POINT_BOTTOM :
//		//			{
//		//				o->show = 0;
//		//				PutCompressedImageFX( o->x-Mapx, o->y-Mapy, (Spr*)(o->lpvData), o->alpha, 1 );
//		//				break;
//		//			}
//
//	}
//}

// Subtractive blend helper matching assembly offset logic

void PutCmprsImgClipingSub555(int x, int y, void* Stmp, void* dest, int lx, int rx, int ty, int by, uint32_t alpha)
{
	if (!Stmp || !dest || by <= 0) return;

	uint8_t* pDstRow = static_cast<uint8_t*>(dest) + y * dDxSize + (x * 2);
	const uint8_t* pSrc = static_cast<const uint8_t*>(Stmp);

	int lxBytes = lx * 2;
	int rxBytes = rx * 2;

	// 1. Skip top non-visible clipped lines (ty)
	for (int line = 0; line < ty; ++line)
	{
		while (true)
		{
			uint16_t skip = *reinterpret_cast<const uint16_t*>(pSrc);
			if (skip == 0xFFFF)
			{
				pSrc += 2;
				break;
			}
			uint16_t length = *reinterpret_cast<const uint16_t*>(pSrc + 2);
			pSrc += 4 + length;
		}
	}

	// 2. Render visible clipped lines (by)
	for (int line = 0; line < by; ++line)
	{
		uint8_t* pDst = pDstRow;
		int currentXBytes = 0;

		while (true)
		{
			uint16_t skipBytes = *reinterpret_cast<const uint16_t*>(pSrc);
			if (skipBytes == 0xFFFF)
			{
				pSrc += 2;
				break;
			}

			pSrc += 2;
			uint16_t dataBytes = *reinterpret_cast<const uint16_t*>(pSrc);
			pSrc += 2;

			int runStartBytes = currentXBytes + skipBytes;
			int runEndBytes = runStartBytes + dataBytes;

			if (runEndBytes > lxBytes && runStartBytes < rxBytes)
			{
				int drawStartBytes = max(runStartBytes, lxBytes);
				int drawEndBytes = min(runEndBytes, rxBytes);

				int bytesToDraw = drawEndBytes - drawStartBytes;
				if (bytesToDraw > 0)
				{
					int srcOffsetBytes = drawStartBytes - runStartBytes;
					int dstOffsetBytes = drawStartBytes - lxBytes;

					const uint16_t* pSrcPix = reinterpret_cast<const uint16_t*>(pSrc + srcOffsetBytes);
					uint16_t* pDstPix = reinterpret_cast<uint16_t*>(pDst + dstOffsetBytes);

					int pixelCount = bytesToDraw / 2;
					for (int p = 0; p < pixelCount; ++p)
					{
						pDstPix[p] = BlendSub555_Table(pSrcPix[p], pDstPix[p], alpha);
					}
				}
			}

			pSrc += dataBytes;
			currentXBytes = runEndBytes;
		}

		pDstRow += dDxSize;
	}
}
#include <cstdint>
#include <algorithm>
#include <SDL2/SDL_render.h>

//extern int wDxSize; // Surface pitch (width in bytes)

// 50% Alpha Blending helper (555 format)
// Uses the binary mask 0x7BDE (0111 1011 1101 1110b) from original inline assembly
// to clear the LSB of each color channel before adding to prevent channel overflow.
inline uint16_t BlendHalf555(uint16_t src, uint16_t dst)
{
	return static_cast<uint16_t>(((src & 0x7BDE) + (dst & 0x7BDE)) >> 1);
}

void PutCmprsImgClipingHBlend555(int x, int y, void* Stmp, void* dest, int lx, int rx, int ty, int by)
{
	if (!Stmp || !dest || by <= 0) return;

	uint8_t* pDstRow = static_cast<uint8_t*>(dest) + y * dDxSize + (x * 2);
	const uint8_t* pSrc = static_cast<const uint8_t*>(Stmp);

	// Convert pixel coordinate bounds to byte offsets (2 bytes per pixel)
	int lxBytes = lx * 2;
	int rxBytes = rx * 2;

	// 1. Skip top non-visible clipped lines (ty)
	for (int line = 0; line < ty; ++line)
	{
		while (true)
		{
			uint16_t skip = *reinterpret_cast<const uint16_t*>(pSrc);
			if (skip == 0xFFFF) // End-of-line marker
			{
				pSrc += 2;
				break;
			}
			uint16_t length = *reinterpret_cast<const uint16_t*>(pSrc + 2);
			pSrc += 4 + length;
		}
	}

	// 2. Render visible clipped lines (by)
	for (int line = 0; line < by; ++line)
	{
		uint8_t* pDst = pDstRow;
		int currentXBytes = 0;

		while (true)
		{
			uint16_t skipBytes = *reinterpret_cast<const uint16_t*>(pSrc);
			if (skipBytes == 0xFFFF)
			{
				pSrc += 2;
				break; // Line finished
			}

			pSrc += 2;
			uint16_t dataBytes = *reinterpret_cast<const uint16_t*>(pSrc);
			pSrc += 2;

			int runStartBytes = currentXBytes + skipBytes;
			int runEndBytes = runStartBytes + dataBytes;

			// Check if chunk falls within horizontal clipping bounds [lxBytes, rxBytes)
			if (runEndBytes > lxBytes && runStartBytes < rxBytes)
			{
				int drawStartBytes = max(runStartBytes, lxBytes);
				int drawEndBytes = min(runEndBytes, rxBytes);

				int bytesToDraw = drawEndBytes - drawStartBytes;
				if (bytesToDraw > 0)
				{
					int srcOffsetBytes = drawStartBytes - runStartBytes;
					int dstOffsetBytes = drawStartBytes - lxBytes;

					const uint16_t* pSrcPix = reinterpret_cast<const uint16_t*>(pSrc + srcOffsetBytes);
					uint16_t* pDstPix = reinterpret_cast<uint16_t*>(pDst + dstOffsetBytes);

					int pixelCount = bytesToDraw / 2;
					for (int p = 0; p < pixelCount; ++p)
					{
						pDstPix[p] = BlendHalf555(pSrcPix[p], pDstPix[p]);
					}
				}
			}

			pSrc += dataBytes;
			currentXBytes = runEndBytes;
		}

		pDstRow += dDxSize; // Advance destination pointer to next line
	}
}
SDL_Texture* GetSpriteTexture(Spr* sp)
{
	if (!sp)
		return nullptr;

	if (!sp->img)
		return nullptr;

	if (sp->xl <= 0 || sp->yl <= 0)
		return nullptr;


	// -------------------------------------------------
	// Check cache
	// -------------------------------------------------

	auto it = m_textures.find(sp->img);

	if (it != m_textures.end())
	{
		return it->second;
	}


	// -------------------------------------------------
	// Get SDL renderer
	// -------------------------------------------------

	SDL_Renderer* renderer =
		g_renderDrawInfo.renderer;

	if (!renderer)
		return nullptr;


	// -------------------------------------------------
	// Create hardware texture
	// -------------------------------------------------

	SDL_Texture* texture =
		SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STATIC,
			sp->xl,
			sp->yl
		);

	if (!texture)
	{
		std::cerr
			<< "SDL_CreateTexture failed: "
			<< SDL_GetError()
			<< std::endl;

		return nullptr;
	}


	// -------------------------------------------------
	// Enable alpha blending
	// -------------------------------------------------

	SDL_SetTextureBlendMode(
		texture,
		SDL_BLENDMODE_BLEND
	);


	// -------------------------------------------------
	// Temporary CPU buffer
	//
	// This is only used once when the compressed
	// sprite is converted to an SDL texture.
	// -------------------------------------------------

	const size_t pixelCount =
		static_cast<size_t>(sp->xl) *
		static_cast<size_t>(sp->yl);

	std::vector<uint32_t> pixels(
		pixelCount,
		0
	);


	// -------------------------------------------------
	// RLE source
	// -------------------------------------------------

	const uint8_t* pSrc =
		reinterpret_cast<const uint8_t*>(
			sp->img
			);


	// -------------------------------------------------
	// Decompress every scanline
	// -------------------------------------------------

	for (int y = 0; y < sp->yl; ++y)
	{
		uint32_t* pDst =
			pixels.data() +
			static_cast<size_t>(y) *
			static_cast<size_t>(sp->xl);


		while (true)
		{
			// -----------------------------------------
			// Read skip amount
			// -----------------------------------------

			uint16_t skipBytes =
				*reinterpret_cast<
				const uint16_t*
				>(pSrc);


			// -----------------------------------------
			// End Of Line
			// -----------------------------------------

			if (skipBytes == 0xFFFF)
			{
				pSrc += 2;
				break;
			}


			// -----------------------------------------
			// Skip transparent pixels
			// -----------------------------------------

			int skipPixels =
				skipBytes / 2;

			pDst += skipPixels;


			// -----------------------------------------
			// Read data size
			// -----------------------------------------

			uint16_t dataBytes =
				*reinterpret_cast<
				const uint16_t*
				>(pSrc + 2);


			// Header:
			//
			// WORD skipBytes
			// WORD dataBytes
			//

			pSrc += 4;


			// -----------------------------------------
			// Number of RGB555 pixels
			// -----------------------------------------

			int pixelCount =
				dataBytes / 2;


			const uint16_t* pSrcPixels =
				reinterpret_cast<
				const uint16_t*
				>(pSrc);


			// -----------------------------------------
			// Convert RGB555 -> ARGB8888
			// -----------------------------------------

			for (int p = 0;
				p < pixelCount;
				++p)
			{
				// Safety
				if (pDst >=
					pixels.data() +
					static_cast<size_t>(y + 1) *
					static_cast<size_t>(sp->xl))
				{
					break;
				}


				uint16_t pixel =
					pSrcPixels[p];


				// RGB555
				uint8_t r =
					static_cast<uint8_t>(
						((pixel >> 10) & 0x1F) *
						255 /
						31
						);

				uint8_t g =
					static_cast<uint8_t>(
						((pixel >> 5) & 0x1F) *
						255 /
						31
						);

				uint8_t b =
					static_cast<uint8_t>(
						(pixel & 0x1F) *
						255 /
						31
						);


				// Fully opaque pixel
				*pDst =
					(0xFFu << 24) |
					(static_cast<uint32_t>(r) << 16) |
					(static_cast<uint32_t>(g) << 8) |
					static_cast<uint32_t>(b);


				++pDst;
			}


			// -----------------------------------------
			// Move to next compressed block
			// -----------------------------------------

			pSrc += dataBytes;
		}
	}


	// -------------------------------------------------
	// Upload decompressed pixels to GPU
	// -------------------------------------------------

	if (SDL_UpdateTexture(
		texture,
		nullptr,
		pixels.data(),
		sp->xl *
		static_cast<int>(
			sizeof(uint32_t)
			)
	) != 0)
	{
		std::cerr
			<< "SDL_UpdateTexture failed: "
			<< SDL_GetError()
			<< std::endl;

		SDL_DestroyTexture(texture);

		return nullptr;
	}


	// -------------------------------------------------
	// Store in cache
	// -------------------------------------------------

	m_textures.emplace(
		sp->img,
		texture
	);


	return texture;
}


void PutCompressedImageFX(
	int x,
	int y,
	Spr* sp,
	DWORD alpha,
	DWORD op)
{
	if (!sp)
		return;

	if (!sp->img)
		return;

	if (sp->xl <= 0 ||
		sp->yl <= 0)
		return;


	SDL_Renderer* renderer =
		g_renderDrawInfo.renderer;

	if (!renderer)
		return;


	// -------------------------------------------------
	// Get cached GPU texture
	// -------------------------------------------------

	SDL_Texture* texture =
		GetSpriteTexture(sp);

	if (!texture)
		return;


	int xl = sp->xl;
	int yl = sp->yl;


	// -------------------------------------------------
	// Apply sprite origin
	// -------------------------------------------------

	x -= sp->ox;
	y -= sp->oy;


	// -------------------------------------------------
	// Completely outside screen
	// -------------------------------------------------

	if (x >= SCREEN_WIDTH)
		return;

	if (x + xl <= 0)
		return;

	if (y >= SCREEN_HEIGHT)
		return;

	if (y + yl <= 0)
		return;


	// -------------------------------------------------
	// Source rectangle
	// -------------------------------------------------

	SDL_Rect src;

	src.x = 0;
	src.y = 0;
	src.w = xl;
	src.h = yl;


	// -------------------------------------------------
	// Destination rectangle
	// -------------------------------------------------

	SDL_Rect dst;

	dst.x = x;
	dst.y = y;
	dst.w = xl;
	dst.h = yl;


	// -------------------------------------------------
	// Clip LEFT
	// -------------------------------------------------

	if (dst.x < 0)
	{
		int clipped =
			-dst.x;

		src.x += clipped;
		src.w -= clipped;

		dst.x = 0;
		dst.w -= clipped;
	}


	// -------------------------------------------------
	// Clip TOP
	// -------------------------------------------------

	if (dst.y < 0)
	{
		int clipped =
			-dst.y;

		src.y += clipped;
		src.h -= clipped;

		dst.y = 0;
		dst.h -= clipped;
	}


	// -------------------------------------------------
	// Clip RIGHT
	// -------------------------------------------------

	if (dst.x + dst.w >
		SCREEN_WIDTH)
	{
		int clipped =
			(dst.x + dst.w) -
			SCREEN_WIDTH;

		src.w -= clipped;
		dst.w -= clipped;
	}


	// -------------------------------------------------
	// Clip BOTTOM
	// -------------------------------------------------

	if (dst.y + dst.h >
		SCREEN_HEIGHT)
	{
		int clipped =
			(dst.y + dst.h) -
			SCREEN_HEIGHT;

		src.h -= clipped;
		dst.h -= clipped;
	}


	// -------------------------------------------------
	// Make sure rectangle is valid
	// -------------------------------------------------

	if (src.w <= 0 ||
		src.h <= 0 ||
		dst.w <= 0 ||
		dst.h <= 0)
	{
		return;
	}


	// -------------------------------------------------
	// Alpha
	// -------------------------------------------------

	Uint8 textureAlpha =
		static_cast<Uint8>(
			std::min<DWORD>(
				alpha,
				255
			)
			);


	// -------------------------------------------------
	// Rendering operation
	// -------------------------------------------------

	switch (op)
	{
	case 1:
		// ---------------------------------------------
		// Normal alpha
		// ---------------------------------------------

		SDL_SetTextureBlendMode(
			texture,
			SDL_BLENDMODE_BLEND
		);

		SDL_SetTextureAlphaMod(
			texture,
			textureAlpha
		);

		SDL_RenderCopy(
			renderer,
			texture,
			&src,
			&dst
		);

		break;


	case 2:
		// ---------------------------------------------
		// Additive
		// ---------------------------------------------

		SDL_SetTextureBlendMode(
			texture,
			SDL_BLENDMODE_ADD
		);

		SDL_SetTextureAlphaMod(
			texture,
			textureAlpha
		);

		SDL_RenderCopy(
			renderer,
			texture,
			&src,
			&dst
		);

		break;


	case 3:
		// ---------------------------------------------
		// Subtractive
		//
		// SDL2 doesn't have a standard subtract mode.
		// Leave this for a custom blend mode/shader.
		// ---------------------------------------------

		break;


	case 4:
		// ---------------------------------------------
		// H Alpha
		//
		// Depends on the original HBlend555
		// implementation.
		// ---------------------------------------------

		break;


	case 5:
		// Add but don't draw same colours
		break;


	case 6:
		// Alpha but don't draw same colours
		break;


	default:
		break;
	}


	// Reset alpha so the texture doesn't accidentally
	// retain the previous sprite's alpha.
	SDL_SetTextureAlphaMod(
		texture,
		255
	);
}
//void PutCompressedImageFX(int x, int y, Spr* sp, DWORD alpha, DWORD op)
//{
//	int		lx, rx, ty, by, IsC = 0;
//
//	if (sp == NULL) return;
//	if (sp->img == NULL) return;
//
//
//	int xl = sp->xl;
//	int yl = sp->yl;
//
//	if (xl == 0) return;
//	if (yl == 0) return;
//
//	x -= sp->ox;
//	y -= sp->oy;
//
//
//	if (x >= SCREEN_WIDTH) return;
//	else if (x + xl < 0) return;
//
//	if (y >= SCREEN_HEIGHT) return;
//	else if (y + yl < 0) return;
//
//	rx = xl;
//	lx = 0;
//	ty = 0;
//	by = yl;
//
//
//
//
//	if (x + xl >= SCREEN_WIDTH)
//	{
//		rx -= x + xl - SCREEN_WIDTH;
//		IsC = 1;
//	}
//	if (x < 0)
//	{
//		lx = 0 - x;
//		x = 0;
//		rx -= lx;
//		IsC = 1;
//	}
//
//	if (y + yl >= SCREEN_HEIGHT)
//	{
//		by -= y + yl - SCREEN_HEIGHT;
//		IsC = 1;
//	}
//	if (y < 0)
//	{
//		ty = 0 - y;
//		y = 0;
//		by -= ty;
//		IsC = 1;
//	}
//
//
//	//-----------------------
//	if (BitMaskR == 0x7C00)
//	{
//		if (IsC) {
//			switch (op) {
//			case 1://alpha
//				PutCmprsImgClipingBlend555(x, y, sp->img, g_DestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 2://add
//				PutCmprsImgClipingAdd555(x, y, sp->img, g_DestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 3://sub
//				PutCmprsImgClipingSub555(x, y, sp->img, g_DestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 4://H alpha
//				PutCmprsImgClipingHBlend555(x, y, sp->img, g_DestBackBuf, lx, rx, ty, by);
//				break;
//			case 5:// Add not put the same colors between dest and source
//				//PutCmprsImgClipingAddDifer555(x, y, sp->img, g_DestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 6://alpha not put the same colors between dest and source
//				//PutCmprsImgClipingBlendDifer555(x, y, sp->img, g_DestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			}
//		}
//		else
//			switch (op) {
//			case 1://alpha
//				PutCmprsImgNOTClipingBlend555(x, y, yl, sp->img, g_DestBackBuf, alpha);
//				break;
//			case 2://add
//				//PutCmprsImgNOTClipingAdd555(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 3://sub
//				//PutCmprsImgNOTClipingSub555(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 4://H alpha
//				//PutCmprsImgNOTClipingHBlend555(x, y, yl, sp->img, g_pDestBackBuf);
//				break;
//			case 5://add not put the same colors between dest and source
//				//PutCmprsImgNOTClipingAddDifer555(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 6://alpha not put the same colors between dest and source
//				//PutCmprsImgNOTClipingBlendDifer555(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			}
//	}
//	else
//	{
//		if (IsC) {
//			switch (op) {
//			case 1://alpha
//				//PutCmprsImgClipingBlend565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 2://add
//				if (sp->yl == 464)
//				{
//					//_asm nop;
//
//				}
//
//				//PutCmprsImgClipingAdd565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				//PutCmprsImgClipingOneColor(x, y, sp->img, g_DestBackBuf, lx, rx, ty, by, alpha);
//				//PutCmprsImgClipingBlend565
//				//PutCmprsImgClipingSub565
//
//				//PutCmprsImgClipingHBlend565(x, y,  sp->img, g_DestBackBuf, lx, rx, ty, by);
//				break;
//			case 3://sub
//				//PutCmprsImgClipingSub565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 4://H alpha
//				//PutCmprsImgClipingHBlend565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by);
//				break;
//			case 5://add not put the same colors between dest and source
//				//PutCmprsImgClipingAddDifer565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 6://alpha not put the same colors between dest and source
//				//PutCmprsImgClipingBlendDifer565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			}
//		}
//		else
//			switch (op) {
//			case 1://alpha
//				//PutCmprsImgNOTClipingBlend565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 2://add
//				//PutCmprsImgNOTClipingAdd565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 3://sub
//				//PutCmprsImgNOTClipingSub565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 4://H alpha
//				//PutCmprsImgNOTClipingHBlend565(x, y, yl, sp->img, g_pDestBackBuf);
//				break;
//			case 5://add  not put the same colors between dest and source
//				//PutCmprsImgNOTClipingAddDifer565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 6://alpha not put the same colors between dest and source
//				//PutCmprsImgNOTClipingBlendDifer565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			}
//	}

