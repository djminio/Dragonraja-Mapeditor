#include "GameProc.h"
#include<SDL2/SDL.h>
#include "main.h"
#include<unordered_map>
#include "map.h"
#include "myTypes.h"
#include "MapObject.h"
int BoxAndBoxCrash_(int ax, int ay, int axl, int ayl, int bx, int by, int bxl, int byl)
{
    if (ax + axl >= bx && bx + bxl >= ax && ay + ayl >= by && by + byl >= ay)
        return 1;

    return 0;
}
int BoxAndBoxCrash(int ax, int ay, int axl, int ayl, int bx, int by, int bxl, int byl)
{
    if (ax + axl >= bx && bx + bxl >= ax && ay + ayl >= by && by + byl >= ay) return 1;
    return 0;
}
int BoxAndDotCrash(int dx, int dy, int dxl, int dyl, int x, int y)
{
    //	BoxOneColor( dx + IMapX, dy+ IMapY, dxl, dyl, 255);
    if (dx <= x && x <= dx + dxl) if (dy <= y && y <= dy + dyl) return 1;
    return 0;
}
//int SortWithObject(LPORDERTABLE oi, LPORDERTABLE oj)
//{
//    LPMAPOBJECT mo;
//
//    if (oi->wType != SPRITETYPE_OBJECT) return 0;
//    if (oi->wType != SPRITETYPE_ALPHAOBJECT) return 0;
//    mo = ((LPMAPOBJECT)(oi->lpvData));
//    if (mo->attr2 == 0) return 0;
//
//    //	if( oj->wType == SPRITETYPE_OBJECT ) 
//    //	{
//    //		mo = ((LPMAPOBJECT )(oj->lpvData));
//    //		if( mo->attr2 ) return 0; 
//    //	}
//
//    return CrashOiOj(oi, oj);
//}

int SortWithObjectObject(LPORDERTABLE oi, LPORDERTABLE oj)
{
    if (oi->wType != SPRITETYPE_OBJECT) return 0;
    if (oi->wType != SPRITETYPE_ALPHAOBJECT) return 0;
    if (oj->wType != SPRITETYPE_OBJECT) return 0;
    if (oj->wType != SPRITETYPE_ALPHAOBJECT) return 0;

    return 1;
}
int Determinant(int ax, int ay, int bx, int by, int X, int Y)
{
    int ret = (ax * (by - Y) - ay * (bx - X) + bx * Y - (by * X));
    return ret;
}
//BOOL IsScreenArea(LPCHARACTER ch)
//{
//    Spr* sp = ch->sp;
//    DWORD	mx = ch->position.x;
//    DWORD	my = ch->position.y;
//
//    if (ch->sprno == 50 || ch->sprno == 51 || ch->sprno == 52)	// LTS DRAGON BUG
//    {
//        ch->direction = DIRECTION_DOWN;
//        //sp = ch->sp = &CharSpr[ch->sprno].sp[ch->direction][ch->nCurrentFrame.GetDecrypted()];
//    }
//
//    if (sp == NULL) return 0;
//
//    if (mx < 0) return 0;
//    if (my < 0) return 0;
//    if (mx > g_Mapinfo.file.wWidth) return 0;
//    if (my > g_Mapinfo.file.wHeight) return 0;
//
//    if (g_Inside != g_tileMap[mx * g_Mapinfo.wScreenWidth + my].attr_inside) return 0;
//
//    int t0 = BoxAndBoxCrash(Mapx, Mapy, GAME_SCREEN_XSIZE, GAME_SCREEN_YSIZE, ch->x - sp->ox, ch->y - sp->oy - ch->height, sp->xl, sp->yl);
//    int t1 = BoxAndBoxCrash(Mapx, Mapy, GAME_SCREEN_XSIZE, GAME_SCREEN_YSIZE, ch->x - sp->ox, ch->y - sp->oy, sp->xl, sp->yl);
//
//    if (t0 || t1) return 1;
//
//    return 0;
//}

//BOOL IsScreenAreaMapObject(int x, int y, SIZE size)
//{
//    int xl = size.cx;
//    int yl = size.cy;
//
//    if (BoxAndBoxCrash(Mapx, Mapy, GAME_SCREEN_XSIZE, GAME_SCREEN_YSIZE, x, y, xl, yl)) return 1;
//
//    return 0;
//}
//
//BOOL IsScreenAreaCSP(int x, int y, Spr* sp)
//{
//    int xl = sp->xl;
//    int yl = sp->yl;
//    if (BoxAndBoxCrash(Mapx, Mapy, GAME_SCREEN_XSIZE, GAME_SCREEN_YSIZE, x - sp->ox, y - sp->oy, xl, yl)) return 1;
//    return 0;
//}

void keyboardProc()
{
    SDL_Event ev;

    while (SDL_PollEvent(&ev))
    {
        if (ev.type == SDL_QUIT)
        {
            isQuit = true;
        }
    }

    SDL_PumpEvents();

    const Uint8* keyboard = SDL_GetKeyboardState(nullptr);

    const int CAMERA_SPEED = 2;

    if (keyboard[SDL_SCANCODE_W])
    {
        g_Mapinfo.y -= CAMERA_SPEED;
    }

    if (keyboard[SDL_SCANCODE_S])
    {
        g_Mapinfo.y += CAMERA_SPEED;
    }

    if (keyboard[SDL_SCANCODE_A])
    {
        g_Mapinfo.x -= CAMERA_SPEED;
    }

    if (keyboard[SDL_SCANCODE_D])
    {
        g_Mapinfo.x += CAMERA_SPEED;
    }

    if (keyboard[SDL_SCANCODE_SPACE])
    {
        // Center camera here
    }

    if (keyboard[SDL_SCANCODE_ESCAPE])
    {
        isQuit = true;
    }


    // Clamp camera AFTER movement
    if (g_Mapinfo.x < 0)
        g_Mapinfo.x = 0;

    if (g_Mapinfo.y < 0)
        g_Mapinfo.y = 0;


    int maxX = (g_Mapinfo.file.wWidth * 32) - SCREEN_WIDTH;
    int maxY = (g_Mapinfo.file.wHeight * 32) - SCREEN_HEIGHT;


    // Handle maps smaller than the screen
    if (maxX < 0)
        maxX = 0;

    if (maxY < 0)
        maxY = 0;


    if (g_Mapinfo.x > maxX)
        g_Mapinfo.x = maxX;

    if (g_Mapinfo.y > maxY)
        g_Mapinfo.y = maxY;
}
void MouseProc()
{

}