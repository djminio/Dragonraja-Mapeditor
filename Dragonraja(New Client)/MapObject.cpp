//#include "stdafx.h"
//#include "MapObject.h"
//#include "Main.h"
//#include "Sprite.h"
//
//#include"GameProc.h"
//#include "Hong_Light.h"
//
//int TestDeter;
//MAPOBJECTIMAGE      MoImage[MAX_MAPOBJECTIMAGE * 10];
//WORD				TotalMapObjectID;
//WORD				TotalMapObject = 0;
//MAPOBJECT			Mo[MAX_MAPOBJECT_];
//MAPOBJECTIMAGE		TempMoImage;
//int g_nLButtonState, g_nRButtonState;
//int g_nLDButtonState, g_nRDButtonState;
//int NewOrderC = 0;
//ORDERTABLE neworder[2000] = { 0, };
//int TempMoImageID[2000];
//ORDERINFO		g_OrderInfo;
//int				g_OldOrderInfoCount;
//
//int					WaveTileStart = 0;
////LPCHARACTER         Hero = NULL;			// 주인공의 Point..
//void changemapobjectname(const char* src, char* dest);
//void DisplayMapObject(LPMAPOBJECT mo);
//int IDCheckedMapObject;
//int g_BoadAndSignNumber; // CSD-030324 // thai2 YGI
//int tool_AnimationEditCurFrame;
//int tool_AnimationEditTotalSpriteNo;
//int tool_ViewAnimationFrame;
//bool tool_DrawMap, tool_DrawObject, tool_DrawFrontMap, tool_ViewMessage, tool_ViewChat, tool_ViewNumber, tool_ViewMenu;
//int tool_DrawRoofMap;
//int tool_MultiChar;
//char				str_Data[80];
//void PutCompressedImageFX(int x, int y, Spr* sp, DWORD alpha, DWORD op);
//DIRECTBOARDDATA		directboardData;
//SCharacter SCharacterData = {
//								7, 3333, 10000,
//								{0},
//								{0},
//								{0},
//								{10,},
//								{1,},
//								20000000,
//
//								0,70,30,100,100,50,100,
//								0,0,
//								0,0,0,0,0L,
//								0,
//};
//
//int IsDead(LPCHARACTER ch)
//{
//	if (ch == NULL) return 1;
//	if (ch == Hero)
//	{
//		return (SCharacterData.nCharacterHP <= 0);
//	}
//	else
//	{
//		return (ch->hp <= 0);
//	}
//
//	return 0;
//}
//bool CheckContinueLine(char check)
//{
//	if (check == 0 || check == ' ' || check == ';' || check == '#' || check == '\n') return true;
//	return false;
//}
//#include <stdio.h>
//#include "GameProc.h"
//#include "map.h"
//#include "myTypes.h"
//bool	GetBoardData(int board_Number, char* str_Data)
//{	//< CSD-030324
//	char* token;
//
//	char		str_boardData[100];	//ÇÑ±Û 50ÀÚ/¿µ¹® 100ÀÚ¸¦ ¹þ¾î³ªÁö ¸øÇÔ
//	BOARDDATA	boardData;
//	FILE* fp;
//	char		DataPath[MAX_PATH];
//
//	sprintf(DataPath, "/map/%s.bsd", Mapname.c_str());
//
//	fp = fopen(DataPath, "rt");			//read only+text file
//	if (fp == NULL)	return false;
//
//	while (!feof(fp))
//	{
//		fgets(str_boardData, 99, fp);
//		if (CheckContinueLine(str_boardData[0])) continue;
//
//		token = strtok(str_boardData, "\t\n");
//		if (!token) continue;
//
//		boardData.boardNumber = atoi(token);
//		token = strtok(NULL, "\t\n");
//		if (token)  strcpy(boardData.str_boardData, token);
//
//		if (boardData.boardNumber == board_Number)
//		{
//			strcpy(str_Data, boardData.str_boardData);
//			fclose(fp);
//			return true;
//		}
//		//return	boardData.str_boardData;
//	}
//	fclose(fp);
//	return	false;
//}	//> CSD-030324
//bool	GetDirectBoardData(int board_Number, DIRECTBOARDDATA* directboardData)
//{	//< CSD-030324
//	char* token;
//
//	char		str_boardData[160];	//ÇÑ±Û 50ÀÚ/¿µ¹® 100ÀÚ¸¦ ¹þ¾î³ªÁö ¸øÇÔ
//	//DIRECTBOARDDATA		directboardData;
//	FILE* fp;
//	char		DataPath[MAX_PATH];
//
//	sprintf(DataPath, "/map/%s.dsd", Mapname);
//
//	fp = fopen(DataPath, "rt");			//read only+text file
//	if (fp == NULL)	return false;
//
//	while (!feof(fp))
//	{
//		fgets(str_boardData, 159, fp);
//
//		if (CheckContinueLine(str_boardData[0])) continue;
//		token = strtok(str_boardData, "\t\n");
//		if (!token) continue;
//
//		directboardData->boardNumber = atoi(token);
//
//		token = strtok(NULL, "\t\n");
//		strcpy(directboardData->str_Title, token);
//		if (!strncmp(directboardData->str_Title, ".", 1))	memset(directboardData->str_Title, 0, sizeof(char[30]));
//
//		token = strtok(NULL, "\t\n");
//		strcpy(directboardData->str_East, token);
//		if (!strncmp(directboardData->str_East, ".", 1))		memset(directboardData->str_East, 0, sizeof(char[30]));
//
//		token = strtok(NULL, "\t\n");
//		strcpy(directboardData->str_South, token);
//		if (!strncmp(directboardData->str_South, ".", 1))	memset(directboardData->str_South, 0, sizeof(char[30]));
//
//		token = strtok(NULL, "\t\n");
//		strcpy(directboardData->str_West, token);
//		if (!strncmp(directboardData->str_West, ".", 1))		memset(directboardData->str_West, 0, sizeof(char[30]));
//
//		token = strtok(NULL, "\t\n");
//		strcpy(directboardData->str_North, token);
//		if (!strncmp(directboardData->str_North, ".", 1))	memset(directboardData->str_North, 0, sizeof(char[30]));
//
//		if (directboardData->boardNumber == board_Number)
//		{
//			fclose(fp);
//			return true;
//		}
//	}
//
//	fclose(fp);
//	return	false;
//}	//> CSD
//bool	ReturnCheckObject(LPMAPOBJECT		mo)
//{
//	static int			buttonOnCount;
//	static int			selectObject = 0;				//¸¶¿ì½º¿¡ ÀÇÇØ Ã¼Å©µÈ ¿ÀºêÁ§Æ® ¹øÈ£
//
//	static int			selectObject_X;					//¼±ÅÃµÈ ¿ÀºêÁ§Æ®ÀÇ XÃà
//	static int			selectObject_Y;					//¼±ÅÃµÈ ¿ÀºêÁ§Æ®ÀÇ YÃà
//
//	static int			tempObject = 0;				//ÀÌÀü¿¡ ¼±ÅÃµÈ ¿ÀºêÁ§Æ® ±â¾ï
//	static int			objectType = 0;				//Ã¼Å©µÈ ¿ÀºêÁ§Æ®ÀÇ Á¾·ù(5:°£ÆÇ/6:ÀÌÁ¤Ç¥)
//	static int			displayBoard = 0;			//ÇöÀç º¸¿©Áö°í ÀÖ´Â °£ÆÇ¹øÈ£
//	static int			displayDirect = 0;			//ÇöÀç º¸¿©Áö°í ÀÖ´Â ÀÌÁ¤Ç¥¹øÈ£	
//
//	char				str_Data[80];
//
//	//DIRECTBOARDDATA		directboardData;
//	int					onCount = 15;				//1ÃÊ
//
//	//if (g_MouseInMenu)  return 0;
//
//	//WORD  objectoritem;				//object Type(0~65535)
//
//	//////////////////////////////// 0420 lkh Ãß°¡ (¸Þ´º°¡ ¿­·Á ¸¶¿ì½º°¡ ¸Þ´ºÀ§¿¡ ÀÖ´Â °æ¿ì´Â Ã¼Å©ÇÏÁö ¾ÊÀ½) 
//	Spr* sp = &MoImage[mo->id - 1].spr[0];
//	if (BoxAndDotCrash((int)(mo->x + mo->offx - sp->ox), (int)(mo->y + mo->offy - sp->oy), sp->xl, sp->yl, Mox, Moy))
//	{
//		selectObject = mo->dum;
//		selectObject_X = mo->x;
//		selectObject_Y = mo->y;
//		objectType = mo->objectoritem;
//
//		if (!IsDead(Hero))
//			if (g_nLButtonState == STATE_BUTTON_PRESSED)
//			{
//				switch (objectType)
//				{
//				case ITEMTYPE_SEAL_NOR:
//					break;
//				case ITEMTYPE_SEAL_SPC:
//					break;
//				case ITEMTYPE_HP_UP:
//					break;
//				case ITEMTYPE_MP_UP:
//					break;
//					//case ITEMTYPE_NATION_STONE:	if (MapInfo[MapNumber].nation == Hero->name_status.nation)		// 1004 YGI---
//					//{
//					//	//MP3(SN_OPEN_NATION_BBS, 1);				// 1027 YGI
//					//	//CallSmallMenu(MN_NATION_BBS);
//					//}
//					//						  break;
//				case ITEMTYPE_GUILD_STONE:
//					break;
//				case ITEMTYPE_SENARIO_STONE:	break;//CallSmallMenu(MN_SCENARIO); break;
//				case ITEMTYPE_SCALE:	break;//CallSmallMenu(MN_SCALE); break;
//
//				default:
//					if ((mo->id >= 336 && mo->id <= 337) || (mo->id >= 2482 && mo->id <= 2485))
//					{
//						//CallSmallMenu(MN_HUNTER_MAIN);
//					}
//					break;
//				}
//			}
//
//
//		if ((mo->objectoritem == ITEMTYPE_SIGNBOARD || mo->objectoritem == ITEMTYPE_DIRECTBOARD) /*&& *//*!g_MouseInMenu*/)
//		{
//			buttonOnCount++;
//			if (buttonOnCount >= onCount + 10)	buttonOnCount = onCount + 10;
//			if (buttonOnCount == onCount || (buttonOnCount >= onCount && selectObject != tempObject))
//			{
//				if (objectType == ITEMTYPE_SIGNBOARD)
//				{
//					g_BoadAndSignNumber = mo->dum; // CSD-030324 // thai2 YGI
//
//					if (GetBoardData(mo->dum, str_Data) == true)
//					{
//						displayBoard = mo->dum;
//						//PutBoardMenu(mo->dum, str_Data);
//						tempObject = selectObject;
//						//SMenu[MN_DISTANCE_TABLE].bActive = false;
//						displayDirect = 0;
//					}
//					return true;
//				}
//				else if (objectType == ITEMTYPE_DIRECTBOARD)
//				{
//					g_BoadAndSignNumber = mo->dum + 10000; // CSD-030324 // thai2 YGI
//
//					if (GetDirectBoardData(mo->dum, &directboardData) == true)
//					{
//						displayDirect = mo->dum;
//						//PutDirectBoardMenu(&directboardData);
//						tempObject = selectObject;
//						//SMenu[MN_STORE_SIGN].bActive = false;
//						displayBoard = 0;
//					}
//					return true;
//				}
//
//			}
//		}
//		else
//		{
//			if ((selectObject == mo->dum && objectType == mo->objectoritem && (selectObject_X == mo->x && selectObject_Y == mo->y)) || (selectObject == 0))
//			{
//				buttonOnCount--;
//				if (buttonOnCount < 0)		buttonOnCount = 0;
//				selectObject = 0;
//				selectObject_X = 0;
//				selectObject_Y = 0;
//				tempObject = 0;
//				objectType = 0;
//
//				if (buttonOnCount == onCount)
//				{
//					if (displayBoard != 0)
//					{
//						//SMenu[MN_STORE_SIGN].bActive = false;
//						displayBoard = 0;
//					}
//					if (displayDirect != 0)
//					{
//						//SMenu[MN_DISTANCE_TABLE].bActive = false;
//						displayDirect = 0;
//					}
//				}
//			}
//		}
//	}
//}
//void CheckMapObject(LPMAPOBJECT mo)
//{
//	if (--mo->delay < 0) { mo->delay = 3; }
//	else return;
//
//	int totalframe = MoImage[mo->id - 1].nFrameCount;
//
//	switch (mo->animationType)
//	{
//	case OBJECTANI_NO:	if (mo->curframe < 0 || mo->curframe >= totalframe)
//	{
//		mo->curframe = 0;
//	}
//					 break;
//
//	case OBJECTANI_RETURNLOOP:	if (mo->anidir)
//	{
//		mo->curframe++;
//		if (mo->curframe >= totalframe) { mo->anidir = !mo->anidir; mo->curframe = totalframe - 1; }
//	}
//							 else
//	{
//		mo->curframe--;
//		if (mo->curframe < 0) { mo->anidir = !mo->anidir; mo->curframe = 0; }
//	}
//							 break;
//
//	case OBJECTANI_LOOP:	mo->curframe++;
//		if (mo->curframe >= totalframe) { mo->curframe = 0; }
//		break;
//
//	case OBJECTANI_TOGGLE_ANI:	mo->curframe = 0;
//		break;
//
//	case OBJECTANI_1_RETURNLOOP:	if (mo->anidir)
//	{
//		mo->curframe++;
//		if (mo->curframe >= totalframe) { mo->anidir = !mo->anidir; mo->curframe = totalframe - 1; }
//	}
//							   else
//	{
//		mo->curframe--;
//		if (mo->curframe < 1) { mo->anidir = !mo->anidir; mo->curframe = 1; }
//	}
//							   break;
//	case OBJECTANI_1_LOOP:	mo->curframe++;
//		if (mo->curframe >= totalframe) { mo->curframe = 1; }
//		break;
//	}
//}
//
//
//int CrashOiOj(LPORDERTABLE oi, LPORDERTABLE oj)
//{
//	//	return 1;
//	int ax, ay, axl, ayl, bx, by, bxl, byl;
//	LPCHARACTER		ch;
//	LPITEMGROUND    i;
//	LPMAPOBJECT		mo;
//	Spr* sp;
//
//
//	switch (oi->wType)
//	{
//	case SPRITETYPE_ALPHAOBJECT:
//	case SPRITETYPE_OBJECT:
//	{
//		switch (oj->wType)
//		{
//		case SPRITETYPE_CHARACTER:
//		case SPRITETYPE_NPC:
//		case SPRITETYPE_MONSTER:
//			mo = (LPMAPOBJECT)oi->lpvData;
//			sp = &MoImage[mo->id - 1].spr[0];
//			ax = mo->x - sp->ox + mo->offx; ay = mo->y - sp->oy + mo->offy;
//			axl = sp->xl; ayl = sp->yl;
//
//			ch = (LPCHARACTER)oj->lpvData;
//			sp = ch->sp;
//			bx = oj->x - sp->ox, by = oj->y - sp->oy;
//			bxl = sp->xl;   byl = sp->yl;
//
//			return BoxAndBoxCrash_(ax, 0, axl, 2, bx, 0, bxl, 3);
//			break;
//		case SPRITETYPE_ITEM:		i = (LPITEMGROUND)oj->lpvData;
//			//											if( ( i->ddx[i->curframe/2] != 0 ) && ( i->ddy[i->curframe/2] != 0 )  ) return 0;
//			sp = i->spr[i->curframe / 2];
//			bx = oj->x - sp->ox, by = oj->y - sp->oy;
//			bxl = sp->xl;  byl = sp->yl;
//
//			mo = (LPMAPOBJECT)oi->lpvData;
//			sp = &MoImage[mo->id - 1].spr[0];
//			ax = mo->x - sp->ox + mo->offx; ay = mo->y - sp->oy + mo->offy;
//			axl = sp->xl; ayl = sp->yl;
//
//			break;
//		case SPRITETYPE_TILE:		bx = oj->x, by = oj->y;
//			bxl = TILE_SIZE;  byl = TILE_SIZE;
//			mo = (LPMAPOBJECT)oi->lpvData;
//			sp = &MoImage[mo->id - 1].spr[0];
//			ax = mo->x - sp->ox; ay = mo->y - sp->oy;
//			axl = sp->xl; ayl = sp->yl;
//			break;
//
//		case SPRITETYPE_ALPHAOBJECT:
//		case SPRITETYPE_OBJECT: 	mo = (LPMAPOBJECT)oi->lpvData;
//			sp = &MoImage[mo->id - 1].spr[0];
//			ax = mo->x - sp->ox + mo->offx; ay = mo->y - sp->oy + mo->offy;
//			axl = sp->xl; ayl = sp->yl;
//
//			mo = (LPMAPOBJECT)oj->lpvData;
//			sp = &MoImage[mo->id - 1].spr[0];
//			bx = mo->x - sp->ox + mo->offx; by = mo->y - sp->oy + mo->offy;
//			bxl = sp->xl; byl = sp->yl;
//
//			return BoxAndBoxCrash_(ax, 0, axl, 2, bx, 0, bxl, 3);
//
//
//			break;
//
//
//
//		default: return 0;
//		}
//
//		//			int t = 
//
//		return BoxAndBoxCrash_(ax, ay, axl, ayl, bx, by, bxl, byl);
//	}
//	break;
//
//	case SPRITETYPE_ITEM:
//	{
//		switch (oj->wType)
//		{
//		case SPRITETYPE_CHARACTER:
//		case SPRITETYPE_NPC:
//		case SPRITETYPE_MONSTER:
//			i = (LPITEMGROUND)oi->lpvData;
//			sp = i->spr[i->curframe / 2];
//			ax = oi->x - sp->ox, ay = oi->y - sp->oy;
//			axl = sp->xl;  ayl = sp->yl;
//
//			ch = (LPCHARACTER)oj->lpvData;
//			sp = ch->sp;
//			bx = oj->x - sp->ox, by = oj->y - sp->oy;
//			bxl = sp->xl;   byl = sp->yl;
//			break;
//		case SPRITETYPE_ALPHAOBJECT:
//		case SPRITETYPE_OBJECT:	mo = (LPMAPOBJECT)oi->lpvData;
//			if (mo->attr2) return 0;
//			sp = &MoImage[mo->id - 1].spr[0];
//			bx = mo->x - sp->ox + mo->offx; by = mo->y - sp->oy + mo->offy;
//			bxl = sp->xl; byl = sp->yl;
//
//			i = (LPITEMGROUND)oi->lpvData;
//			sp = i->spr[i->curframe / 2];
//			ax = oi->x - sp->ox, ay = oi->y - sp->oy;
//			axl = sp->xl;  ayl = sp->yl;
//			break;
//		case SPRITETYPE_TILE:		bx = oj->x, by = oj->y;
//			bxl = TILE_SIZE;  byl = TILE_SIZE;
//			i = (LPITEMGROUND)oi->lpvData;
//			sp = i->spr[i->curframe / 2];
//			ax = oi->x - sp->ox, ay = oi->y - sp->oy;
//			axl = sp->xl;  ayl = sp->yl;
//			break;
//		default: return 0;
//		}
//		return BoxAndBoxCrash_(ax, ay, axl, ayl, bx, by, bxl, byl);
//	}
//	break;
//	default: return 0;
//	}
//
//	return 0;
//}
//
//int SortWithItem(LPORDERTABLE oi, LPORDERTABLE oj)
//{
//	if (oi->wType != SPRITETYPE_ITEM) return 0;
//
//	LPITEMGROUND i = (LPITEMGROUND)oi->lpvData;
//
//	if (i->type != ITEMTYPE_DOOR) return 0;
//
//	return CrashOiOj(oi, oj);
//}
//
//void CalcOrder(void)
//{
//	// alistar: reset this ..
//	g_OldOrderInfoCount = g_OrderInfo.count;
//	g_OrderInfo.count = 0;
//
//	register int	i, j;
//	LPORDERTABLE	pi;
//
//	LPCHARACTER		ch;
//	//	LPTILE			lpTile;
//
//	Spr* s;
//	LPORDERTABLE	o, oj, lpmin = nullptr;
//	LPMAPOBJECT		mo;
//	ORDERTABLE min;
//
//	//	MapObjectTouchOrder();
//
//	WaveTileStart++;
//	WaveTileStart %= 39;
//
//	ch = Hero;
//	int cc = 0;
//	while (ch != NULL)
//	{
//		cc++;
//		if (IsScreenArea(ch))
//		{
//			if (g_OrderInfo.count >= MAX_ORDERTABLE) break;
//
//			o = &g_OrderInfo.order[g_OrderInfo.count];
//
//			if (ch->height > 100) o->wType = SPRITETYPE_ON_THE_CEILING_CHAR;
//			else o->wType = ch->type;
//			o->x = ch->x;
//			o->y = ch->y;
//			o->height = ch->height;
//			o->offset = ch->y;
//			o->lpvData = ch;
//			o->show = 1;
//			o->sort = 1;
//			g_OrderInfo.count++;
//
//
//		}
//		else
//		{
//			if (ch != Hero)
//			{
//				// À½... ÁÖÀÎ°øÀÇ ½Ã¾ß+SIGHT_PLUS¿¡¼­ ¹þ¾î³ª¸é ¾ø¾Ø´Ù. 
////				if( (ch->position.x >Hero->position.x) * ( ch->x - Hero->x )  +  (ch->y-Hero->y) * (ch->y-Hero->y) > Hero->sight + 90000 )
////				if( abs( ch->position.x - Hero->position.x ) > 20 && abs( ch->position.y - Hero->position.y ) > 20 )
////				{
////					tempch = ch->lpNext;
////					DeleteCharacter( &g_CharacterList, ch );
////					ch = tempch;
////					continue;
////				}		
//			}
//		}
//		ch = ch->lpNext;
//	}
//
//
//
//
//	// object				
//	//lpObjectHeader = g_ObjectListHeader.lpHead;
//	IDCheckedMapObject = -1;
//	g_BoadAndSignNumber = 0; // CSD-030324 // thai2 YGI
//
//	if (tool_DrawObject == 0)
//		for (i = 0; i < TotalMapObject; i++)
//		{
//			mo = &Mo[i];
//			if (IsScreenAreaCSP(mo->x + mo->offx, mo->y + mo->offy, &MoImage[mo->id - 1].spr[0]))
//			{
//				bool	check = ReturnCheckObject(mo);
//
//				//////////////////////////// 0812 lkh Ãß°¡ ///////////////////////////////
//				//	Çö»ó±Ý °Ô½ÃÆÇÀ» ´õºíÅ¬¸¯(¿ÞÂÊ¹öÆ°)ÇÏ´Â °æ¿ì °Ô½ÃÆÇ ¸Þ´º ¶á´Ù.
//	//			Spr *sp = &MoImage[ mo->id-1].spr[0];
//	///			if( mo->id >= 2482 && mo->id <=2485 )
//	///				if( BoxAndDotCrash( (int)(mo->x+mo->offx-sp->ox), (int)(mo->y+mo->offy-sp->oy), sp->xl, sp->yl, Mox, Moy) && g_MouseInMenu == false )
//	//					if( g_nLDButtonState )
//	//						CallSmallMenu(MN_HUNTER_MAIN);
//				//////////////////////////////////////////////////////////////////////////
//
//				CheckMapObject(mo);
//				{
//					if (g_OrderInfo.count >= MAX_ORDERTABLE) break;
//
//					o = &g_OrderInfo.order[g_OrderInfo.count];
//
//					if (check)	o->wType = SPRITETYPE_ALPHAOBJECT;
//					else		o->wType = SPRITETYPE_OBJECT;		//ALPHAOBJECT
//					o->x = mo->x;
//					o->y = mo->y;
//					o->offset = mo->y;
//					o->rgb = i; // rgb·Î ¾²´Â°ÅÀÌ ¾Æ´Ï¶ó ¹è¿­¹øÈ£¸¦ ¾´´Ù. 
//
//					o->height = 0;
//					o->lpvData = (LPVOID)(mo);
//					o->show = 1;
//					o->sort = 1;
//					if (mo->light)
//						OutputLight(mo->x + mo->offx + mo->lightx, mo->y + mo->offy + mo->lighty, mo->light, 0);
//					g_OrderInfo.count++;
//				}
//#if 0
//#ifdef _DEBUG // finito 060507							
//				if (GetSysInfo(SI_GAME_MAKE_MODE))
//				{
//					// MapObject¸¦ CheckÇÒ¶§.1. ÂïÀº ÁÂÇ¥¿Í Ãæµ¹µÇ´Â MapObject¸¦ Ã£´Â´Ù. 
//					if (BoxAndDotCrash(mo->x - 10, mo->y - 10, 20, 20, Mox, Moy))
//					{
//						IDCheckedMapObject = i;
//					}
//				}
//#endif
//#endif
//			}
//		}
//
//#if 0
//	// MapObject¸¦ CheckÇÒ¶§.2. ¸¶¿ì½º¿Í Ãæµ¹µÇ´Â MapObject¸¦ Ã£´Â´Ù. 
//	if (GetSysInfo(SI_GAME_MAKE_MODE))
//		if (IDCheckedMapObject == -1)
//			for (i = 0; i < TotalMapObject; i++)
//			{
//				mo = &Mo[i];
//				if (IsScreenAreaCSP(mo->x + mo->offx, mo->y + mo->offy, &MoImage[mo->id - 1].spr[0]))
//				{
//					{
//						Spr* sp = &MoImage[mo->id - 1].spr[mo->curframe];
//						if (BoxAndDotCrash(mo->x + mo->offx - sp->ox, mo->y + mo->offy - sp->oy, sp->xl, sp->yl, Mox, Moy))
//						{
//							IDCheckedMapObject = i;
//						}
//					}
//				}
//			}
//#endif
//
//	int minid = 0;
//
//	NewOrderC = 0;
//	for (i = 0; i < g_OrderInfo.count; i++)
//	{
//		for (j = 0; j < g_OrderInfo.count; j++)
//		{
//			oj = &g_OrderInfo.order[j];
//			if (oj->sort == 0) continue;
//
//			switch (oj->wType)
//			{
//			case SPRITETYPE_DETECT_TILE:
//			case SPRITETYPE_SKILLCURSOR:
//				continue;
//			}
//			memcpy(&min, oj, sizeof(ORDERTABLE));
//			lpmin = oj;
//			minid = j;
//			break;
//		}
//
//		for (j = 0; j < g_OrderInfo.count; j++)
//		{
//			if (minid == j) continue;
//
//			oj = &g_OrderInfo.order[j];
//			if (oj->sort == 0) continue;
//
//			switch (oj->wType)
//			{
//			case SPRITETYPE_DETECT_TILE:
//			case SPRITETYPE_SKILLCURSOR:
//				continue;
//			}
//
//			if (SortWithItem(lpmin, oj))
//				//if( CrashlpminOj( lpmin, oj ) )
//			{
//				LPITEMGROUND ti = (LPITEMGROUND)(lpmin->lpvData);
//				int c = ti->curframe >> 1;
//				int kkx = lpmin->x, kky = lpmin->y;
//				if (Determinant(ti->dsx[c], ti->dsy[c], ti->ddx[c], ti->ddy[c], oj->x, oj->offset) < 0)
//				{
//					//memcpy( &min, oj, sizeof( ORDERTABLE ) );
//					lpmin = oj;//min;
//					minid = j;
//				}
//			}
//			else if (SortWithItem(oj, lpmin))
//			{
//				LPITEMGROUND tj = (LPITEMGROUND)(oj->lpvData);
//				int c = tj->curframe >> 1;
//				int kkx = oj->x, kky = oj->y;
//				if (Determinant(tj->dsx[c], tj->dsy[c], tj->ddx[c], tj->ddy[c], lpmin->x, lpmin->offset) > 0)
//				{
//					//memcpy( &min, oj, sizeof( ORDERTABLE ) );
//					lpmin = oj;//min;
//					minid = j;
//				}
//			}
//			else if (SortWithObjectObject(oj, lpmin))
//			{
//				LPMAPOBJECT mo1 = (LPMAPOBJECT)(oj->lpvData);
//				LPMAPOBJECT mo2 = (LPMAPOBJECT)(lpmin->lpvData);
//
//				int mo1minoff = min(mo1->y + mo1->dy0[mo1->curframe], mo1->y + mo1->dy1[mo1->curframe]);
//				int mo2minoff = min(mo2->y + mo2->dy0[mo2->curframe], mo2->y + mo2->dy1[mo2->curframe]);
//
//				if (mo1minoff < mo2minoff)
//				{
//					lpmin = oj;//min;
//					minid = j;
//				}
//			}
//			else if (SortWithObject(oj, lpmin))
//			{
//				LPMAPOBJECT mo = (LPMAPOBJECT)(oj->lpvData);
//				int kkx = mo->x, kky = mo->y;
//				int c = mo->curframe;
//				if (Determinant(kkx + mo->dx0[c], kky + mo->dy0[c], kkx + mo->dx1[c], kky + mo->dy1[c], lpmin->x, lpmin->offset) > 0)
//				{
//					//memcpy( &min, oj, sizeof( ORDERTABLE ) );
//					lpmin = oj;//min;
//					minid = j;
//				}
//			}
//
//			else if (SortWithObject(lpmin, oj))
//			{
//				mo = (LPMAPOBJECT)(lpmin->lpvData);
//				int kkx = lpmin->x, kky = lpmin->y;
//				int c = mo->curframe;
//				TestDeter = Determinant(kkx + mo->dx0[c], kky + mo->dy0[c], kkx + mo->dx1[c], kky + mo->dy1[c], oj->x, oj->offset);
//				if (TestDeter < 0)
//				{
//					//memcpy( &min, oj, sizeof( ORDERTABLE ) );
//					lpmin = oj;//min;
//					minid = j;
//				}
//			}
//			else if (lpmin->offset > oj->offset)
//			{
//				//memcpy( &min, oj, sizeof( ORDERTABLE ) );
//				lpmin = oj;//min;
//				minid = j;
//			}
//			else if (lpmin->offset == oj->offset)
//			{
//				if (oj->wType == SPRITETYPE_TILE)
//				{
//					switch (lpmin->wType)
//					{
//						//						case	SPRITETYPE_CHARACTER:
//						//						{		
//						//							memcpy( &temp,	oj, sizeof( ORDERTABLE ) );
//						//							memcpy( oj,		lpmin, sizeof( ORDERTABLE ) );
//						//							memcpy( lpmin,	 &temp, sizeof( ORDERTABLE ) );
//						//							break;
//						//						}		
//					case SPRITETYPE_ALPHAOBJECT:
//					case	SPRITETYPE_OBJECT:
//					{
//						LPMAPOBJECT	mo = (LPMAPOBJECT)lpmin->lpvData;
//
//						if (mo->attr == OBJECTATTR_BACK_ABOVE || mo->attr == OBJECTATTR_FRONT_ABOVE || mo->attr == OBJECTATTR_FRONT_BELOW)
//						{
//							//memcpy( &min, oj, sizeof( ORDERTABLE ) );
//							lpmin = oj;//min;
//							minid = j;
//						}
//						break;
//					}
//					}
//				}
//			}
//		}
//
//		g_OrderInfo.order[minid].sort = 0;
//		memcpy(&neworder[NewOrderC], &g_OrderInfo.order[minid], sizeof(ORDERTABLE));
//		NewOrderC++;
//	}
//
//	for (i = 0; i < g_OrderInfo.count; i++)
//	{
//		pi = &g_OrderInfo.order[i];
//
//		if (pi->sort)
//		{
//			pi->sort = 0;
//
//			memcpy(&neworder[NewOrderC], pi, sizeof(ORDERTABLE));
//			NewOrderC++;
//		}
//	}
//
//	LPITEMGROUND item;
//	Spr* sp;
//	bool bClickOn = false;
//
//#if 0
//	MouseCheckCharacterName = NULL;
//	g_GameInfo.nSelectedSpriteType = SPRITETYPE_NONE;
//	g_GameInfo.lpcharacter = NULL;
//#endif
//
//	for (i = 0; i < g_OrderInfo.count; i++)
//	{
//		pi = &g_OrderInfo.order[i];
//		pi->rank = i;
//
//		switch (pi->wType)
//		{
//#if 0
//		case SPRITETYPE_ITEM:
//			item = (LPITEMGROUND)pi->lpvData;
//			sp = item->spr[item->curframe / 2];
//			if (item->attr[IATTR_ATTR] & IA2_BOX || item->attr[IATTR_ATTR] & IA2_COLOSSUS_STONE)
//			{
//				if (BoxAndDotCrash(item->x + item->offx - sp->ox, item->y + item->offy - sp->oy, sp->xl, sp->yl, Mox, Moy))
//				{
//					g_GameInfo.lpvSelectedSprite = pi->lpvData;
//					g_GameInfo.nSelectedSpriteType = pi->wType;
//					g_GameInfo.lpcharacter = (LPCHARACTER)pi->lpvData;
//					return;
//				}
//			}
//			break;
//
//		case SPRITETYPE_ON_THE_CEILING_CHAR:
//		case SPRITETYPE_MONSTER:
//		case SPRITETYPE_NPC:
//		case SPRITETYPE_CHARACTER:
//
//			ch = (LPCHARACTER)pi->lpvData;
//			int dir = ch->direction;
//			int sprc = ch->AnimationTable[ch->nCurrentAction].cFrame[ch->nCurrentFrame.GetDecrypted()];
//			int xl, yl;
//			int ox, oy;
//			//int height	= ch->AnimationTable[ ch->nCurrentAction].height[  ch->nCurrentFrame.GetDecrypted() ];
//			s = &(CharSpr[ch->sprno].sp[dir][sprc]);
//
//			ox = s->ox;
//			oy = s->oy;
//			xl = s->xl; if (xl < 15) { ox <<= 2;  xl <<= 2; }
//			yl = s->yl;	if (yl < 15) { oy <<= 2;  xl <<= 2; }
//
//
//			if (ch->just_ani == false)
//				if (BoxAndDotCrash(pi->x - ox, pi->y - oy - ch->height, xl, yl, Mox, Moy))
//				{
//					if (ch->viewtype == VIEWTYPE_OBSERVE_)
//					{	//< CSD-030509
//						return;
//					}	//> CSD-030509
//					//	³»°¡ À¯·ÉÀÌ°Å³ª À¯·ÉÀ» º¼ ÀÖÀ» ¶§¸¸ »ó´ëÀ¯·ÉÀ» ¼±ÅÃÇÒ¼ö ÀÖ´Ù. 
//					if (ch->viewtype == VIEWTYPE_GHOST_)
//					{
//						// YGI ACER
//						if (IsHidenGhost(ch)) break;//020510 lsw
//						if (!CanViewGhost(Hero) && Hero->viewtype != VIEWTYPE_GHOST_) break;
//						g_GameInfo.lpvSelectedSprite = pi->lpvData;
//						g_GameInfo.nSelectedSpriteType = pi->wType;
//						g_GameInfo.lpcharacter = (LPCHARACTER)pi->lpvData;
//						MouseCheckCharacterName = (LPCHARACTER)pi->lpvData;		//1027 YGI
//						return;
//					}
//
//					MouseCheckCharacterName = (LPCHARACTER)pi->lpvData;
//
//					//	Á×¾î ÀÖÀ»°æ¿ì..............
//					if (IsDead(ch))
//					{
//						if (SkillOn == -1) break;
//					}
//
//
//					//	ÀÏ¹Ý PCÀÏ°Ï¿ì¿¡´Â 
//					if (ch->id < 10000)			// 0906 YGI	fightmap_2
//					{
//						if (g_FightMapStart) /* ÀüÅõ ¸Ê¿¡¼­ ¼­·Î ´Ù¸¥ ÆÀÀÏ°æ¿ì ÄÁÆ®·Ñ Å° ¾øÀÌµµ ¼±ÅÃÀÌ °¡´ÉÇÏ°Ô...*/
//						{
//							if (Hero->fight_map_team_num && ch->fight_map_team_num && Hero->fight_map_team_num != ch->fight_map_team_num) bClickOn = true;
//						}
//
//#ifdef _NATION_APPLY_
//						else if (ch->name_status.nation != Hero->name_status.nation && ch->lv >= 1)	// 011022 LTS
//						{	//< CSD-040414
//							if (!CheckNotClickNation())
//							{
//								bClickOn = true;	// Å¬¸¯ ºÒ°¡´ÉÀÌ ¾Æ´Ï¸é // 010724 YGI
//
//								for (int i = 0; i < 6; ++i)
//								{
//									if (SCharacterData.party[i].ch == ch)
//									{
//										bClickOn = false;
//										break;
//									}
//								}
//							}
//						}	//> CSD-040414
//#endif
//						else if (IsNK(ch) >= 5 && ch != Hero)				bClickOn = true;
//						else if (Hero->fight_flag && Hero->fight_id == ch->id) bClickOn = true; // °áÅõ½Ã.. 
//
//						if (g_bWarBegin)	// ÀüÀïÁßÀÌ¸é	// 011022 LTS
//						{
//							if (isOurSquad(Hero, ch)) bClickOn = false; //±¹°¡ÀüÀÌ°í ±âÅ¸µîµîÀÌ¸é
//							else bClickOn = true;
//						}
//						if (Hero->DragonLordWarTeam > 0 && ch->DragonLordWarTeam > 0)		// LTS DRAGONLORD
//						{
//							if (Hero->DragonLordWarTeam != ch->DragonLordWarTeam) bClickOn = true;		// ´Ù¸¥ÆíÀÌ¸é 
//							else bClickOn = false;
//						}
//						if ((MapNumber == 39) && (g_EventLocalWarBegin == 3)) bClickOn = true;	// LTS BUG
//
//						if (!bClickOn)
//							if (CheckPC == false && SkillOn == -1 && g_mgrBattle.IsSelect(ch) && !bClickOn)
//							{
//								break;
//							}
//					}
//					else
//					{	//< CSD-031013
//						if (CheckPC == false && (ch->HostName[0]))
//						{
//							if (GetBattlePeace() != PK_MODE)
//							{
//								break;
//							}
//						}
//					}	//> CSD-031013
//
//					g_GameInfo.lpvSelectedSprite = pi->lpvData;
//					g_GameInfo.nSelectedSpriteType = pi->wType;
//					g_GameInfo.lpcharacter = (LPCHARACTER)pi->lpvData;
//					return;
//				}
//			break;
//#endif
//		}
//	}
//}
//
//void DisplaySprite_Item(int& x, int& y, LPCHARACTER ch, LPORDERTABLE o)//021108 lsw
//{
//	o->show = 0;
//	LPITEMGROUND  item = (LPITEMGROUND)o->lpvData;
//
//	int skip = 0;
//	const int iIX = item->x;
//	const int iIY = item->y;
//	const int iIOX = item->offx;
//	const int iIOY = item->offy;
//
//	switch (item->Anitype)
//	{
//	case ITEMANIMATION_FIRE:
//	{
//		PutCompressedImage(iIX - Mapx + iIOX, iIY - Mapy + iIOY, item->spr[0]);
//	}break;
//	case ITEMANIMATION_ACTIVE: // 020701 YGI
//	{
//		switch (item->attr[IATTR_SPRITE_TYPE] & 0x0003)
//		{
//		case OBJECTSPRITE_ALPHA:
//		{
//			PutCompressedImage(iIX - Mapx, iIY - Mapy, item->spr[0]);
//		}
//		case OBJECTSPRITE_TRANS:
//		{
//			PutCompressedImageFX(iIX - Mapx, iIY - Mapy, item->spr[item->curframe / 2], item->attr[IATTR_SPRITE_TYPE] >> 3, 2);
//			//OutputLight(iIX, iIY, 16);		// ±¤¿ø
//		}break;
//		}
//		skip = 1;
//	}break;
//	}
//	if (skip) { return; }
//	if (item->curframe < 0)
//	{
//		item->curframe = 0;		// 020701 YGI
//	}
//	///////////////////////////////////////
//
//	Spr* sp = item->spr[item->curframe / 2];
//	//if( item->shadow)	::PutCompressedImageShadow(  o->x - Mapx, o->y-Mapy, sp );	
//
//
//	if (item->attr[IATTR_ATTR] & IA2_FIRE) // ºÒÀ» ÇÇ¿ü¾î..
//	{
//		::PutCompressedImageFX(iIX - Mapx + iIOX, iIY - Mapy + iIOY - 16, sp, 10 + (rand() % 10), 2);
//	}
//	else if (item->attr[IATTR_ATTR] & IA2_HIDE) // ¾Èº¸ÀÌ´Â ¼Ó¼ºÀÌ¸é...
//	{
//		if (Hero->canviewitem)
//		{
//			::PutCompressedImageFX(iIX - Mapx + iIOX, iIY - Mapy + iIOY, sp, rand() % 15, 2);
//			//::PutCompressedImageVoid(iIX - Mapx + iIOX + rand() % 4 - 2, iIY - Mapy + iIOY + rand() % 4 - 2, sp);
//		}
//		else
//		{
//			// ¾ÊÂï¾î.. ¿Ö.. ¾Èº¸ÀÌ´Ï±î...
//		}
//	}
//
//	else if (item->attr[IATTR_ATTR] & IA2_DOOR)
//	{
//		::PutCompressedImage(iIX - Mapx + iIOX, iIY - Mapy + iIOY, sp);
//	}
//}
//void MaskMapEffect(int x, int y, int height, Spr* sp)
//{
//	int i, j;
//
//	int sx = x - sp->ox;
//	int sy = y - sp->oy - height;
//	int ex = sx + sp->xl + TILE_SIZE - 1;
//	int ey = sy + sp->yl + TILE_SIZE - 1;
//
//	//	if( !TileMap[ x/TILE_SIZE][ y/TILE_SIZE].front )
//	//	{
//	//		return;
//	//	}
//
//	//	int offset = TileMap[ x/TILE_SIZE][ y/TILE_SIZE].frontoffset;
//
//	//	ex += (( ex - sx )/2);
//
//	sx /= TILE_SIZE;
//	sy /= TILE_SIZE;
//	ex /= TILE_SIZE;
//	ey /= TILE_SIZE;
//
//	if (sx < 0) sx = 0;
//	if (sy < 0) sy = 0;
//	if (ex >= g_Map.file.wWidth) ex = g_Map.file.wWidth;
//	if (ey >= g_Map.file.wHeight) ey = g_Map.file.wHeight;
//
//	//	my += Mapy;
//
//	for (i = sx; i < ex; i++)
//		for (j = sy; j < ey; j++)
//		{
//			int front = TileMap[i][j].front;
//			if (front)
//				if ((int)TileMap[i][j].frontoffset * TILE_SIZE >= y)
//				{
//					if (Sou[front].xl == -1)		PutTile(i * TILE_SIZE - Mapx, j * TILE_SIZE - Mapy, Sou[front].img, g_pDestBackBuf);
//					else							PutCompressedImage(i * TILE_SIZE - Mapx, j * TILE_SIZE - Mapy, &Sou[front]);
//				}
//		}
//}
//void DisplaySprite_DestPoint(int& x, int& y, LPCHARACTER ch, LPORDERTABLE o)//021108 lsw
//{
//	o->show = 0;
//	if (o->rgb)
//	{
//		::PutCompressedImage(o->x - Mapx, o->y - Mapy, (Spr*)(o->lpvData));
//	}
//	else
//	{
//		::PutCompressedImageFX(o->x - Mapx, o->y - Mapy, (Spr*)(o->lpvData), o->alpha, 1);
//	}
//}
//void DisplaySprite_Tile(int& x, int& y, LPCHARACTER ch, LPORDERTABLE o)//021108 lsw
//{
//#ifdef _DEBUG	
//	if (tool_DrawMap) { return; }
//#endif
//	o->show = 0;
//	const int mx = o->x - Mapx;
//	const int my = o->y - Mapy;
//	const int front = ((LPTILE)o->lpvData)->front;
//
//	if (Sou[front].xl == -1)
//	{
//		//		if( mx >= 0 && mx < SCREEN_WIDTH && my >= 0 && my < SCREEN_HEIGHT )
//		::PutTile(mx, my, Sou[front].img, g_pDestBackBuf);
//	}
//	else
//	{
//		::PutCompressedImage(mx, my, &Sou[front]);
//	}
//}
//void PutCmprsImgClipingBlendRGB555(int x, int y, LPVOID Stmp, LPVOID dest, int lx, int rx, int ty, int by, DWORD RGB)
//{
//	int		l;
//	LPVOID	Dtmp = (LPVOID)((char*)dest + y * 1440 + (x * 2));
//	_asm {
//		mov		ecx, rx;
//		shl		ecx, 1;
//		mov		rx, ecx;
//
//		mov		ecx, lx;
//		shl		ecx, 1;
//		mov		lx, ecx;
//	}
//
//	_asm {
//		push	es;
//
//		push	ds;
//		pop		es;
//
//		xor eax, eax;
//
//		mov		esi, Stmp;
//
//		mov		ecx, ty;
//		or ecx, ecx;
//		jz		exit_1;
//	loop_1:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_xxxx;
//		mov		ax, [esi + 2]
//			add		esi, 4;
//		add		esi, eax;
//		jmp		loop_1;
//	exit_xxxx:
//		add		esi, 2;
//		loop	loop_1;
//	exit_1:
//	}
//	_asm {
//		mov		ebx, by;		//
//		or ebx, ebx;
//		jz		exit_for1;
//	loop_for1:
//		xor edx, edx;
//		mov		edi, Dtmp;
//		test	lx, 0xffff;			//if(lx)
//		jz		exit_if1;
//	loop_while1:
//		cmp		word ptr[esi], 0xffff;	//while(*stmp != 0xffff)
//		jz		exit_while1;			//
//		add		dx, [esi];
//		cmp		edx, lx;		//if(j>lx)
//		jle		jmp_if1;			//
//		sub		edx, lx;		//j-=lx
//		add		edi, edx;			//dtmp+=j
//		jmp		jmp_0416;//****************************************************
//
//	jmp_if1:///////////////////////////////////////
//		mov		ax, [esi + 2];
//		add		esi, 4;
//		add		edx, eax;
//		add		esi, eax;	//
//		cmp		edx, lx;		//
//		jle		loop_while1;
//		sub		edx, lx;
//		sub		esi, edx;
//		mov		eax, edx;
//		xor edx, edx;
//		jmp		jmp_0429;		//4.29
//	exit_while1:	//end while(*stmp != 0xffff)*/
//	exit_if1:	//end if(lx)
//
//	loop_while2:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_while2;
//
//		movzx	eax, word ptr[esi];
//		add		edi, eax;	//dtmp+=*stmp
//		add		edx, eax;	//j+=*stmp
//	jmp_0416:
//		mov		ax, [esi + 2];
//		add		esi, 4;
//	jmp_0429:
//		mov		ecx, eax;		//l=k=*stmp
//		mov		l, eax;
//
//		add		eax, edx;
//		cmp		eax, rx;
//		jle		exit_if0;
//		cmp		edx, rx;
//		jg		exit_if0;
//		mov		ecx, rx;
//		sub		ecx, edx;
//	exit_if0:
//		cmp		edx, rx;
//		jg		else_if;
//
//		add		edx, l;			//j+=l;
//		sub		l, ecx;			//l-k;
//		or ecx, ecx;
//		jz		jmp_kkk;
//
//		shr		ecx, 1;		//move DATA
//		push	edx;
//	loop_alpha:
//
//		///////////////////
//		movzx	eax, word ptr[esi];
//		ror		eax, 10;
//		shl		ax, 11;
//		rol		eax, 5;
//		shl		ax, 6;
//		rol		eax, 5;
//
//		and eax, RGB;
//
//		and eax, 0x03e0fc1f;
//		ror		eax, 5;
//		shr		ax, 6;
//		ror		eax, 5;
//		shr		ax, 11;
//		rol		eax, 10;
//
//		mov[edi], ax;
//
//		//////////////////
//
//
//		mov[edi], bx;
//
//		add		edi, 2;
//		add		esi, 2;
//		loop	loop_alpha;
//		pop		edx;
//	jmp_kkk:
//		add		esi, l;
//		jmp		loop_while2;
//
//	else_if:
//		add		esi, l;
//	ext_loop:
//		cmp		word ptr[esi], 0xffff;
//		jz		ext;
//		movzx	eax, [esi + 2];
//		add		esi, 4;
//		add		esi, eax;
//		jmp		ext_loop;
//	ext:
//
//	exit_while2:
//		mov		eax, dDxSize;
//		add		esi, 2;
//		add		dword ptr Dtmp, eax;
//		dec		dword ptr by;	//for(i=0;i<by;i++)
//		jnz		loop_for1;	//
//	exit_for1:
//
//		pop		es;
//	};
//}
//void PutCmprsImgNOTClipingBlendRGB555(int x, int y, int yl, LPVOID Stmp, LPVOID dest, DWORD RGB)
//{
//	DWORD	j;
//	LPVOID	Dtmp = (LPVOID)((char*)dest + y * 1440 + (x * 2));
//
//	j = yl;
//
//	_asm {
//		push	es;
//
//		push	ds;
//		pop		es;
//
//		mov		esi, Stmp;
//
//		//		mov		ebx,RGB;
//
//		inc		j;
//
//		xor eax, eax;
//		xor ecx, ecx;
//	loop_1:
//		dec		j
//			jz		exit_1;  //if(SY==0)
//		mov		edi, Dtmp;
//
//	loop_2:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_2;
//
//		xor eax, eax;
//		mov		ax, [esi];
//		add		edi, eax;	//0 skip
//
//		mov		cx, [esi + 2];	//load data num
//		add		esi, 4;
//
//		shr		ecx, 1;
//
//	loop_alpha:
//		xor eax, eax;
//		mov		ax, [esi];
//		ror		eax, 10;
//		shl		ax, 11;
//		rol		eax, 5;
//		shl		ax, 6;
//		rol		eax, 5;
//
//		and eax, RGB;
//
//		and eax, 0x03e0fc1f;
//		ror		eax, 5;
//		shr		ax, 6;
//		ror		eax, 5;
//		shr		ax, 11;
//		rol		eax, 10;
//
//		mov[edi], ax;
//
//		add		edi, 2;
//		add		esi, 2;
//		loop	loop_alpha;
//
//		jmp		loop_2;
//	exit_2:
//		add		esi, 2;
//
//		mov		eax, Dtmp;
//		add		eax, dDxSize;
//		mov		Dtmp, eax;
//		jmp		loop_1;
//	exit_1:
//		pop		es;
//	};
//}
//void PutCmprsImgClipingBlendRGB565(int x, int y, LPVOID Stmp, LPVOID dest, int lx, int rx, int ty, int by, DWORD RGB)
//{
//	int		l;
//	LPVOID	Dtmp = (LPVOID)((char*)dest + y * 1440 + (x * 2));
//	_asm {
//		mov		ecx, rx;
//		shl		ecx, 1;
//		mov		rx, ecx;
//
//		mov		ecx, lx;
//		shl		ecx, 1;
//		mov		lx, ecx;
//	}
//
//	_asm {
//		push	es;
//
//		push	ds;
//		pop		es;
//
//		xor eax, eax;
//
//		mov		esi, Stmp;
//
//		mov		ecx, ty;
//		or ecx, ecx;
//		jz		exit_1;
//	loop_1:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_xxxx;
//		mov		ax, [esi + 2]
//			add		esi, 4;
//		add		esi, eax;
//		jmp		loop_1;
//	exit_xxxx:
//		add		esi, 2;
//		loop	loop_1;
//	exit_1:
//	}
//	_asm {
//		mov		ebx, by;		//
//		or ebx, ebx;
//		jz		exit_for1;
//	loop_for1:
//		xor edx, edx;
//		mov		edi, Dtmp;
//		test	lx, 0xffff;			//if(lx)
//		jz		exit_if1;
//	loop_while1:
//		cmp		word ptr[esi], 0xffff;	//while(*stmp != 0xffff)
//		jz		exit_while1;			//
//		add		dx, [esi];
//		cmp		edx, lx;		//if(j>lx)
//		jle		jmp_if1;			//
//		sub		edx, lx;		//j-=lx
//		add		edi, edx;			//dtmp+=j
//		jmp		jmp_0416;//****************************************************
//
//	jmp_if1:///////////////////////////////////////
//		mov		ax, [esi + 2];
//		add		esi, 4;
//		add		edx, eax;
//		add		esi, eax;	//
//		cmp		edx, lx;		//
//		jle		loop_while1;
//		sub		edx, lx;
//		sub		esi, edx;
//		mov		eax, edx;
//		xor edx, edx;
//		jmp		jmp_0429;		//4.29
//	exit_while1:	//end while(*stmp != 0xffff)*/
//	exit_if1:	//end if(lx)
//
//	loop_while2:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_while2;
//
//		mov  	 ax, word ptr[esi];
//		add		edi, eax;	//dtmp+=*stmp
//		add		edx, eax;	//j+=*stmp
//	jmp_0416:
//		mov		ax, [esi + 2];
//		add		esi, 4;
//	jmp_0429:
//		mov		ecx, eax;		//l=k=*stmp
//		mov		l, eax;
//
//		add		eax, edx;
//		cmp		eax, rx;
//		jle		exit_if0;
//		cmp		edx, rx;
//		jg		exit_if0;
//		mov		ecx, rx;
//		sub		ecx, edx;
//	exit_if0:
//		cmp		edx, rx;
//		jg		else_if;
//
//		add		edx, l;			//j+=l;
//		sub		l, ecx;			//l-k;
//		or ecx, ecx;
//		jz		jmp_kkk;
//
//		shr		ecx, 1;		//move DATA
//		push	edx;
//	loop_alpha:
//
//		///////////////////
//		movzx	eax, word ptr[edi];
//		ror		eax, 11;
//		shl		ax, 10;
//		rol		eax, 6;
//		shl		ax, 5;
//		rol		eax, 5;
//
//		and eax, RGB;
//
//		and eax, 0x03e0fc1f;
//		ror		eax, 5;
//		shr		ax, 5;
//		ror		eax, 6;
//		shr		ax, 10;
//		rol		eax, 11;
//
//		mov[edi], ax;
//
//		//////////////////
//
//
//
//
//		add		edi, 2;
//		add		esi, 2;
//		loop	loop_alpha;
//		pop		edx;
//	jmp_kkk:
//		add		esi, l;
//		jmp		loop_while2;
//
//	else_if:
//		add		esi, l;
//	ext_loop:
//		cmp		word ptr[esi], 0xffff;
//		jz		ext;
//		movzx	eax, [esi + 2];
//		add		esi, 4;
//		add		esi, eax;
//		jmp		ext_loop;
//	ext:
//
//	exit_while2:
//		mov		eax, dDxSize;
//		add		esi, 2;
//		add		dword ptr Dtmp, eax;
//		dec		dword ptr by;	//for(i=0;i<by;i++)
//		jnz		loop_for1;	//
//	exit_for1:
//
//		pop		es;
//	};
//}
//void PutCmprsImgNOTClipingBlendRGB565(int x, int y, int yl, LPVOID Stmp, LPVOID dest, DWORD RGB)
//{
//	DWORD	j;
//	LPVOID	Dtmp = (LPVOID)((char*)dest + y * 1440 + (x * 2));
//
//	j = yl;
//
//	_asm {
//		push	es;
//
//		push	ds;
//		pop		es;
//
//		mov		esi, Stmp;
//
//		//		mov		ebx,RGB;
//
//		inc		j;
//
//		xor eax, eax;
//		xor ecx, ecx;
//	loop_1:
//		dec		j
//			jz		exit_1;  //if(SY==0)
//		mov		edi, Dtmp;
//
//	loop_2:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_2;
//
//		xor eax, eax;
//		mov		ax, [esi];
//		add		edi, eax;	//0 skip
//
//		mov		cx, [esi + 2];	//load data num
//		add		esi, 4;
//
//		shr		ecx, 1;
//
//	loop_alpha:
//		xor eax, eax;
//		mov		ax, [esi];
//		ror		eax, 11;
//		shl		ax, 10;
//		rol		eax, 6;
//		shl		ax, 5;
//		rol		eax, 5;
//
//		and eax, RGB;
//
//		and eax, 0x03e0fc1f;
//		ror		eax, 5;
//		shr		ax, 5;
//		ror		eax, 6;
//		shr		ax, 10;
//		rol		eax, 11;
//
//		mov[edi], ax;
//
//		add		edi, 2;
//		add		esi, 2;
//		loop	loop_alpha;
//
//		jmp		loop_2;
//	exit_2:
//		add		esi, 2;
//
//		mov		eax, Dtmp;
//		add		eax, dDxSize;
//		mov		Dtmp, eax;
//		jmp		loop_1;
//	exit_1:
//		pop		es;
//	};
//}
//void PutCompressedImageBlendRGB(int x, int y, Spr* sp, DWORD RGB)
//{
//	int	lx, rx, ty, by, IsC = 0;
//
//	if (sp == NULL) return;
//	if (sp->img == NULL) return;
//
//
//	int xl = sp->xl;
//	int yl = sp->yl;
//
//
//	x -= sp->ox;
//	y -= sp->oy;
//
//	if (x >= SCREEN_WIDTH)			return;
//	else if (x + xl < 0)	return;
//	if (y >= SCREEN_HEIGHT)			return;
//	else if (y + yl < 0)	return;
//
//	rx = xl;
//	lx = 0;
//	ty = 0;
//	by = yl;
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
//	if (_PixelInfo.BitMaskR == 0x7C00)
//	{
//		if (IsC)	PutCmprsImgClipingBlendRGB555(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, RGB);
//		else 	PutCmprsImgNOTClipingBlendRGB555(x, y, yl, sp->img, g_pDestBackBuf, RGB);
//	}
//	else
//	{
//		if (IsC)	PutCmprsImgClipingBlendRGB565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, RGB);
//		else 	PutCmprsImgNOTClipingBlendRGB565(x, y, yl, sp->img, g_pDestBackBuf, RGB);
//	}
//
//}
//void	PutCmprsImgVoidCliping(int x, int y, LPVOID Stmp, LPVOID dest, int lx, int rx, int ty, int by)
//{
//	int		l;
//	LPVOID Dtmp = (LPVOID)((char*)dest + y * 1440 + (x * 2));
//
//	_asm {
//		mov		ecx, rx;
//		shl		ecx, 1;
//		mov		rx, ecx;
//
//		mov		ecx, lx;
//		shl		ecx, 1;
//		mov		lx, ecx;
//	}
//
//	_asm {
//		push	es;
//
//		push	ds;
//		pop		es;
//
//		xor eax, eax;
//
//		mov		esi, Stmp;
//
//		mov		ecx, ty;
//		or ecx, ecx;
//		jz		exit_1;
//	loop_1:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_xxxx;
//		mov		ax, [esi + 2]
//			add		esi, 4;
//		add		esi, eax;
//		jmp		loop_1;
//	exit_xxxx:
//		add		esi, 2;
//		loop	loop_1;
//	exit_1:
//	}
//	_asm {
//		mov		ebx, by;		//
//		or ebx, ebx;
//		jz		exit_for1;
//	loop_for1:
//		xor edx, edx;
//		mov		edi, Dtmp;
//		test	lx, 0xffff;			//if(lx)
//		jz		exit_if1;
//	loop_while1:
//		cmp		word ptr[esi], 0xffff;	//while(*stmp != 0xffff)
//		jz		exit_while1;			//
//		add		dx, [esi];
//		cmp		edx, lx;		//if(j>lx)
//		jle		jmp_if1;			//
//		sub		edx, lx;		//j-=lx
//		add		edi, edx;			//dtmp+=j
//		jmp		jmp_0416;//****************************************************
//
//	jmp_if1:///////////////////////////////////////
//		mov		ax, [esi + 2];
//		add		esi, 4;
//		add		edx, eax;
//		add		esi, eax;	//
//		cmp		edx, lx;		//
//		jle		loop_while1;
//		sub		edx, lx;
//		sub		esi, edx;
//		mov		eax, edx;
//		xor edx, edx;
//		jmp		jmp_0429;		//4.29
//	exit_while1:	//end while(*stmp != 0xffff)*/
//	exit_if1:	//end if(lx)
//
//	loop_while2:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_while2;
//
//		movzx	eax, word ptr[esi];
//		add		edi, eax;	//dtmp+=*stmp
//		add		dx, ax;		//j+=*stmp
//	jmp_0416:
//		mov		ax, [esi + 2];
//		add		esi, 4;
//	jmp_0429:
//		mov		ecx, eax;		//l=k=*stmp
//		mov		l, eax;
//
//		add		eax, edx;
//		cmp		eax, rx;
//		jle		exit_if0;
//		cmp		edx, rx;
//		jg		exit_if0;
//		mov		ecx, rx;
//		sub		ecx, edx;
//	exit_if0:
//		cmp		edx, rx;
//		jg		else_if;
//
//		add		edx, l;			//j+=l;
//		sub		l, ecx;			//l-k;
//
//		//			shr		ecx,2;		//move DATA
//		//			jnc		Next2_1;	//
//		//			movsw;				//
//		//		Next2_1:				//
//		///			rep		movsd;		//
//		//
//
//
//		shr		ecx, 2;		//move DATA
//		jnc		Next2_1;		//
//		mov		ax, word ptr[edi]
//			mov		word ptr[edi - 2], ax
//			add		esi, 2
//			add		edi, 2
//
//			Next2_1:	//
//
//		cmp		cx, 0
//			jz		NextVoid_2;
//
//	NextVoid:
//
//		mov		eax, [edi];
//		mov[edi - 2], eax
//			add		esi, 4
//			add		edi, 4
//
//			loop	NextVoid
//
//			NextVoid_2 :
//
//		add		esi, l;
//		jmp		loop_while2;
//
//	else_if:
//		add		esi, l;
//	ext_loop:
//		cmp		word ptr[esi], 0xffff;
//		jz		ext;
//		mov		ax, [esi + 2];
//		add		esi, 4;
//		add		esi, eax;
//		jmp		ext_loop;
//	ext:
//
//	exit_while2:
//		mov		eax, dDxSize;
//		add		esi, 2;
//		add		dword ptr Dtmp, eax;
//		dec		ebx;	//for(i=0;i<by;i++)
//		jnz		loop_for1;	//
//	exit_for1:
//
//		pop		es;
//	};
//}
//void	PutCmprsImgVoidNOTCliping(int x, int y, int yl, LPVOID Stmp, LPVOID dest)
//{
//	DWORD	j;
//	LPVOID	Dtmp = (LPVOID)((char*)dest + y * 1440 + (x * 2));
//	j = yl;
//
//	_asm {
//		push	es;
//
//		push	ds;
//		pop		es;
//
//		mov		edx, Dtmp;
//		mov		esi, Stmp;
//
//		mov		ebx, j;
//
//		xor eax, eax;
//		xor ecx, ecx;
//	loop_1:
//		or bx, bx;		// if(SY==0)
//		jz		exit_1;
//		mov		edi, edx;
//
//	loop_2:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_2;
//
//		xor eax, eax
//			mov		ax, [esi];
//		add		edi, eax;	//0 skip
//
//		mov		cx, [esi + 2];	//load data num
//		add		esi, 4;
//
//		shr		ecx, 2;		//move DATA
//		jnc		Next2;		//
//		mov		ax, word ptr[edi]
//			mov		word ptr[edi - 2], ax
//			add		esi, 2
//			add		edi, 2
//
//			Next2:	//
//
//		cmp		cx, 0
//			jz		loop_2;
//
//	NextVoid:
//
//		mov		eax, [edi];
//		mov[edi - 2], eax
//			add		esi, 4
//			add		edi, 4
//
//			loop	NextVoid
//
//			//		rep		movsd;		//
//
//			jmp		loop_2;
//	exit_2:
//		add		esi, 2;
//
//		add		edx, dDxSize;
//		dec		bx;
//		jmp		loop_1;
//	exit_1:
//		pop		es;
//	};
//}
//void PutCompressedImageVoid(int x, int y, Spr* sp)
//{
//	//#ifdef _SDL
//	//	SDL_Lock();
//	//#endif
//	int	lx, rx, ty, by, IsC = 0;
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
//	if (x >= (SCREEN_WIDTH - 2))	return;
//	else if (x + xl < 0)			return;
//
//	if (y >= SCREEN_HEIGHT)		return;
//	else if (y + yl < 0)			return;
//
//	rx = xl;
//	lx = 0;
//	ty = 0;
//	by = yl;
//
//	if (x + xl >= (SCREEN_WIDTH - 2))
//	{
//		rx -= x + xl - (SCREEN_WIDTH - 2);
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
//	if (IsC)
//		PutCmprsImgVoidCliping(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by);
//	else
//		PutCmprsImgVoidNOTCliping(x, y, sp->yl, sp->img, g_pDestBackBuf);
//	//#ifdef _SDL
//	//	SDL_UnLock();
//	//#endif
//}
//
//
//void DisplaySprite_Effect(int& x, int& y, LPCHARACTER ch, LPORDERTABLE o)//021108 lsw
//{
//	switch (o->type)
//	{
//	case 1:
//	case 2:
//	case 3:
//	case 4:
//		::PutCompressedImageFX(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData), o->alpha, o->type);
//		break;
//	case 5:
//		::PutCompressedImage(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData));
//		break;
//	case 6:
//		PutCompressedImageBlendRGB(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData), o->rgb);
//		break;
//	case 7:
//		PutCompressedImageVoid(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData));
//		break;
//	case 8:			//0502 lkh Ãß°¡
//		PutCompressedImageVoid(o->x + (rand() % 5 - 2) - Mapx, o->y - o->height + (rand() % 5 - 2) - Mapy, (Spr*)(o->lpvData));
//		::PutCompressedImageFX(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData), o->alpha, 2);
//		break;
//	}
//	::MaskMapEffect(o->x, o->y, o->height, (Spr*)(o->lpvData));
//}
//
//void Box_(int sx, int sy, int xl, int yl, int color)
//{
//	//Line(sx, sy, sx, sy + yl - 1, color);
//	//Line(sx, sy, sx + xl - 1, sy, color);
//	//Line(sx + xl - 1, sy + yl - 1, sx, sy + yl - 1, color);
//	//Line(sx + xl - 1, sy + yl - 1, sx + xl - 1, sy, color);
//}
//void DisplaySpriteTrans(void)
//{
//	register int	i;
//	LPORDERTABLE o;
//
//
//	for (i = 0; i < g_OrderInfo.count; i++)
//	{
//		if (0)
//
//			switch (g_OrderInfo.order[i].wType)
//			{
//			case	SPRITETYPE_CHARACTER:
//			case	SPRITETYPE_MONSTER:
//			case	SPRITETYPE_NPC:
//			{
//				LPCHARACTER		ch = (LPCHARACTER)g_OrderInfo.order[i].lpvData;
//				Box_(ch->x - ch->sp->ox, ch->y - ch->sp->oy, ch->sp->xl, ch->sp->yl, RGB16(0, 255, 0));
//				break;
//			}
//			//				case SPRITETYPE_ALPHAOBJECT :
//			//				case SPRITETYPE_OBJECT :
//			//				{	
//			//					DisplayMapObject(( LPMAPOBJECT )o->lpvData );
//			//				}	
//			case	SPRITETYPE_TILE:
//			{
//				Box_(o->x, o->y, TILE_SIZE, TILE_SIZE, RGB16(255, 255, 0));
//				break;
//			}
//			case  SPRITETYPE_DEST_POINT_BOTTOM:
//			{
//				Spr* sp = (Spr*)(o->lpvData);
//				Box_(o->x - sp->ox, o->y - sp->oy, sp->xl, sp->yl, RGB16(255, 255, 255));
//				break;
//			}
//			case SPRITETYPE_ITEM:
//			case SPRITETYPE_ITEM_BOTTOM:
//			{
//				LPITEMGROUND  item = (LPITEMGROUND)o->lpvData;
//				Spr* sp = item->spr[item->curframe / 2];
//				Box_(o->x - sp->ox + item->offx, o->y - sp->oy + item->offy, sp->xl, sp->yl, RGB16(255, 255, 255));
//
//				Box_(o->x - sp->ox + item->offx, o->y - sp->oy + item->offy, sp->xl, sp->yl, RGB16(255, 255, 255));
//				break;
//			}
//			case SPRITETYPE_EFFECT_CEILING:
//			case SPRITETYPE_EFFECT_NORMAL:
//			case SPRITETYPE_EFFECT_BOTTOM:
//			{
//				Spr* sp = (Spr*)(o->lpvData);
//				Box_(o->x - sp->ox, o->y - sp->oy, sp->xl, sp->yl, RGB16(0, 0, 255));
//				break;
//			}
//			}
//		//		Hcolor(255, 0, 0);
//			//	Hprint2(o->x - Mapx, o->y - Mapy, g_DestBackBuf, "%d", o->rank);
//				//Hcolor(255, 255, 255);
//				//Hprint2(o->x - Mapx, o->y - Mapy, g_DestBackBuf, "%d", o->rank);
//	}
//
//	switch (g_OrderInfo.order[i].wType)
//	{
//	case	SPRITETYPE_CHARACTER:
//	case	SPRITETYPE_MONSTER:
//	case	SPRITETYPE_NPC:
//	{
//		LPCHARACTER		ch = (LPCHARACTER)g_OrderInfo.order[i].lpvData;
//		switch (ch->sprno)
//		{
//		case 0:
//		case 1: 	break;
//		}
//		break;
//	}
//	}
//}
//void PutCmprsImgNOTClipingShadow555(int x, int y, int yl, LPVOID Stmp, LPVOID dest)
//{
//	DWORD  sk, j;
//
//	x += (yl / 2);
//	y += (yl / 2);
//	LPVOID	Dtmp = (LPVOID)((char*)dest + y * dDxSize + (x * 2));
//	j = yl;
//	sk = dDxSize - 2;
//	_asm {
//		push	es;
//
//		push	ds;
//		pop		es;
//
//		mov		edx, Dtmp;
//		mov		esi, Stmp;
//
//		mov		ebx, j;
//
//		xor eax, eax;
//		xor ecx, ecx;
//
//		or ebx, ebx;	// if(SY==0)
//		jz		exit_1;
//	loop_1:
//		mov		edi, edx;
//
//	loop_2:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_2;
//
//		mov		ax, [esi];
//		add		edi, eax;	//0 skip
//
//		mov		cx, [esi + 2];	//load data num
//		add		esi, 4;
//		shr		ecx, 1;
//
//	loop_TR:
//		mov		ebx, [edi];
//		and ebx, 111101111011110b;
//		shr		ebx, 1;
//		mov[edi], bx;
//		add		esi, 2;
//		add		edi, 2;
//		loop	loop_TR;
//
//		jmp		loop_2;
//	exit_2:
//		add		esi, 2;
//
//		add		edx, sk;
//		dec		j;
//		jz		exit_1;
//
//		//---------------------
//		mov		edi, edx;
//
//	loop_22:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_22;
//
//		mov		ax, [esi];
//		add		edi, eax;	//0 skip
//
//		mov		cx, [esi + 2];	//load data num
//		add		esi, 4;
//		add     esi, ecx
//			jmp		loop_22;
//
//	exit_22:
//		add		esi, 2;
//
//		dec		j;
//		jnz		loop_1;
//		//---------------------
//	exit_1:
//		pop		es;
//	};
//}
//void PutCmprsImgNOTClipingShadow565(int x, int y, int yl, LPVOID Stmp, LPVOID dest)
//{
//	DWORD  sk, j;
//
//	x += (yl / 2);
//	y += (yl / 2);
//	LPVOID	Dtmp = (LPVOID)((char*)dest + y * dDxSize + (x * 2));
//	j = yl;
//	sk = dDxSize - 2;
//	_asm {
//		push	es;
//
//		push	ds;
//		pop		es;
//
//		mov		edx, Dtmp;
//		mov		esi, Stmp;
//
//		mov		ebx, j;
//
//		xor eax, eax;
//		xor ecx, ecx;
//
//		or ebx, ebx;	// if(SY==0)
//		jz		exit_1;
//	loop_1:
//		mov		edi, edx;
//
//	loop_2:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_2;
//
//		mov		ax, [esi];
//		add		edi, eax;	//0 skip
//
//		mov		cx, [esi + 2];	//load data num
//		add		esi, 4;
//		shr		ecx, 1;
//
//	loop_TR:
//		mov		ebx, [edi];
//		and ebx, 1111011111011110b;
//		shr		ebx, 1;
//		mov[edi], bx;
//		add		esi, 2;
//		add		edi, 2;
//		loop	loop_TR;
//
//		jmp		loop_2;
//	exit_2:
//		add		esi, 2;
//
//		add		edx, sk;
//		dec		j;
//		jz		exit_1;
//
//		//---------------------
//		mov		edi, edx;
//
//	loop_22:
//		cmp		word ptr[esi], 0xffff;
//		jz		exit_22;
//
//		mov		ax, [esi];
//		add		edi, eax;	//0 skip
//
//		mov		cx, [esi + 2];	//load data num
//		add		esi, 4;
//		add     esi, ecx
//			jmp		loop_22;
//
//	exit_22:
//		add		esi, 2;
//
//		dec		j;
//		jnz		loop_1;
//		//---------------------
//	exit_1:
//		pop		es;
//	};
//}
//void PutCompressedImageShadow(int x, int y, Spr* sp)
//{
//#ifdef _SDL
//	SDL_Lock();
//#endif
//	int	lx, rx, ty, by, IsC = 0;
//	int xl = sp->xl;
//	int yl = sp->yl;
//
//	if (sp == NULL) return;
//	if (sp->img == NULL) return;
//
//	x -= sp->ox;
//	y -= sp->oy;
//
//	if (x >= SCREEN_WIDTH)			return;
//	else if (x + xl < 0)	return;
//	if (y >= GAME_SCREEN_YSIZE)			return;
//	else if (y + yl < 0)	return;
//
//	rx = xl;
//	lx = 0;
//	ty = 0;
//	by = yl;
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
//	if (y + yl >= GAME_SCREEN_YSIZE)
//	{
//		by -= y + yl - GAME_SCREEN_YSIZE;
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
//	if (_PixelInfo.BitMaskR == 0x7C00)
//	{
//		if (IsC) {
//		}
//		else
//		{
//			PutCmprsImgNOTClipingShadow555(x, y, yl, sp->img, g_pDestBackBuf);
//		}
//	}
//	else
//	{
//		if (IsC) {
//		}
//		else
//		{
//			PutCmprsImgNOTClipingShadow565(x, y, yl, sp->img, g_pDestBackBuf);
//		}
//	}
//#ifdef _SDL
//	SDL_UnLock();
//#endif
//}
//void MaskMapItem(int x, int y, int offy, Spr* sp)
//{
//	int i, j;
//
//	int sx = x - sp->ox;
//	int sy = y - sp->oy;
//	int ex = sx + sp->xl + TILE_SIZE - 1;
//	int ey = sy + sp->yl + TILE_SIZE - 1;
//
//	//ex += (( ex - sx )/2); // ±×¸²ÀÚ¸¦ À§ÇÑ X±æÀÌ È®Àå.
//
//	sx /= TILE_SIZE;
//	sy /= TILE_SIZE;
//	ex /= TILE_SIZE;
//	ey /= TILE_SIZE;
//
//	if (sx < 0) sx = 0;
//	if (sy < 0) sy = 0;
//	if (ex >= g_Map.file.wWidth) ex = g_Map.file.wWidth;
//	if (ey >= g_Map.file.wHeight) ey = g_Map.file.wHeight;
//
//
//	for (i = sx; i < ex; i++)
//		for (j = sy; j < ey; j++)
//		{
//			int front = TileMap[i][j].front;
//			if (front)
//				if ((int)TileMap[i][j].frontoffset * TILE_SIZE >= offy)
//				{
//					if (Sou[front].xl == -1)		PutTile(i * TILE_SIZE - Mapx, j * TILE_SIZE - Mapy, Sou[front].img, g_pDestBackBuf);
//					else							PutCompressedImage(i * TILE_SIZE - Mapx, j * TILE_SIZE - Mapy, &Sou[front]);
//				}
//		}
//}
//void DisplaySpriteBottom(void)
//{
//	register int	i;
//
//	for (i = 0; i < g_OrderInfo.count; i++)
//	{
//		LPCHARACTER		ch = (LPCHARACTER)g_OrderInfo.order[i].lpvData;
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
//				PutCompressedImageBlendRGB(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData), o->rgb);
//				break;
//
//			case 7:
//				PutCompressedImageVoid(o->x - Mapx, o->y - o->height - Mapy, (Spr*)(o->lpvData));
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
//			if (item->shadow)	PutCompressedImageShadow(o->x - Mapx, o->y - Mapy, sp);
//			PutCompressedImage(o->x - Mapx + item->offx, o->y - Mapy + item->offy, sp);
//			if (o->rgb && item->getable)	PutCompressedImageFX(o->x - Mapx + item->offx, o->y - Mapy + item->offy, sp, o->alpha, 2);
//			MaskMapItem(o->x + item->offx, o->y + item->offy, o->y, sp);
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
//
//void DisplaySpriteUnderTile(void)
//{
//	int i;
//
//	for (i = 0; i < g_OrderInfo.count; i++)
//	{
//		LPORDERTABLE o = &g_OrderInfo.order[i];
//
//		if (o->show == 0) continue;
//
//		switch (o->wType)
//		{
//		case SPRITETYPE_ALPHAOBJECT:
//		case SPRITETYPE_OBJECT:
//		{
//			//				LPMAPOBJECT mo;
//			if (((LPMAPOBJECT)o->lpvData)->attr == OBJECTATTR_BACK_BELOW)
//			{
//				o->show = 0;
//				DisplayMapObject((LPMAPOBJECT)o->lpvData);
//			}
//			break;
//		}
//		}
//	}
//}
//void DisplaySprite(void)
//{
//	int x = 0, y = 0;
//	int count = 0;
//	for (register int i = 0; i < NewOrderC; i++)
//	{
//		LPCHARACTER	ch = (LPCHARACTER)neworder[i].lpvData;
//		LPORDERTABLE o = &neworder[i];
//
//		if (o->show == 0) continue;
//		switch (o->wType)
//		{
//		case SPRITETYPE_CHARACTER:
//		case SPRITETYPE_MONSTER:
//		case SPRITETYPE_NPC:
//		{
//			//::DisplaySprite_Character(x, y, ch, o);//021108 lsw		
//		}break;
//		case SPRITETYPE_ALPHAOBJECT:
//		case SPRITETYPE_OBJECT:
//		{
//			if (((LPMAPOBJECT)o->lpvData)->attr != OBJECTATTR_BACK_ABOVE)
//				if (((LPMAPOBJECT)o->lpvData)->attr != OBJECTATTR_BACK_BELOW)
//				{
//					::DisplayMapObject((LPMAPOBJECT)o->lpvData);
//				}
//			o->show = 0;
//		}break;
//		case SPRITETYPE_EFFECT_NORMAL:
//		{
//			::DisplaySprite_Effect(x, y, ch, o);
//		}break;
//		case SPRITETYPE_ITEM:
//		{//021108 lsw
//			::DisplaySprite_Item(x, y, ch, o);
//		}break;
//		case	SPRITETYPE_TILE:
//		{
//			DisplaySprite_Tile(x, y, ch, o);
//		}break;
//		case  SPRITETYPE_DEST_POINT:
//		{
//			DisplaySprite_DestPoint(x, y, ch, o);
//		}break;
//		case SPRITETYPE_SKILLGAUGE:
//		{
//			o->show = 0;
//			//LPSKILLGAUGE n = (LPSKILLGAUGE)o->lpvData;
//			//::DrawSkillGauge(n);
//		}break;
//		//////////////////////////// 0126 lkh ////////////////////////////
//		case SPRITETYPE_CLOCKNIDDLE:
//		{
//			o->show = 0;
//		}break;
//		default:
//		{
//			continue;
//		}break;
//		}
//	}
//}
////void DisplaySprite()
////{
////	int x = 0, y = 0;
////	int count = 0;
////	int test[5];
////	for (register int i = 0; i < NewOrderC; i++)
////	{
////		LPCHARACTER	ch = (LPCHARACTER)neworder[i].lpvData;
////		LPORDERTABLE o = &neworder[i];
////		
////		if (o->show == 0) continue;
////		switch (o->wType)
////		{
////		case SPRITETYPE_ALPHAOBJECT:
////		case SPRITETYPE_OBJECT:
////		{
////			if (((LPMAPOBJECT)o->lpvData)->attr != OBJECTATTR_BACK_ABOVE)
////				if (((LPMAPOBJECT)o->lpvData)->attr != OBJECTATTR_BACK_BELOW)
////				{
////					DisplayMapObject((LPMAPOBJECT)o->lpvData);
////				}
////			o->show = 0;
////		}break;
////
////		//////////////////////////// 0126 lkh ////////////////////////////
////		case SPRITETYPE_CLOCKNIDDLE:
////		{
////			o->show = 0;
////		}break;
////		default:
////		{
////			continue;
////		}break;
////		}
////	}
////}
//
//void PutCompressedImageFX(int x, int y, Spr* sp, DWORD alpha, DWORD op)
//{
//	//#ifdef _SDL
//	//	SDL_Lock();
//	//#endif
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
//	if (_PixelInfo.BitMaskR == 0x7C00)
//	{
//		if (IsC) {
//			switch (op) {
//			case 1://alpha
//				PutCmprsImgClipingBlend555(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 2://add
//				PutCmprsImgClipingAdd555(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 3://sub
//				PutCmprsImgClipingSub555(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 4://H alpha
//				PutCmprsImgClipingHBlend555(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by);
//				break;
//			case 5:// Add not put the same colors between dest and source
//				PutCmprsImgClipingAddDifer555(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 6://alpha not put the same colors between dest and source
//				PutCmprsImgClipingBlendDifer555(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			}
//		}
//		else
//			switch (op) {
//			case 1://alpha
//				PutCmprsImgNOTClipingBlend555(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 2://add
//				PutCmprsImgNOTClipingAdd555(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 3://sub
//				PutCmprsImgNOTClipingSub555(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 4://H alpha
//				PutCmprsImgNOTClipingHBlend555(x, y, yl, sp->img, g_pDestBackBuf);
//				break;
//			case 5://add not put the same colors between dest and source
//				PutCmprsImgNOTClipingAddDifer555(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 6://alpha not put the same colors between dest and source
//				PutCmprsImgNOTClipingBlendDifer555(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			}
//	}
//	else
//	{
//		if (IsC) {
//			switch (op) {
//			case 1://alpha
//				PutCmprsImgClipingBlend565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 2://add
//				if (sp->yl == 464)
//				{
//					_asm nop;
//
//				}
//
//				PutCmprsImgClipingAdd565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				//PutCmprsImgClipingOneColor(x, y, sp->img, g_DestBackBuf, lx, rx, ty, by, alpha);
//				//PutCmprsImgClipingBlend565
//				//PutCmprsImgClipingSub565
//
//				//PutCmprsImgClipingHBlend565(x, y,  sp->img, g_DestBackBuf, lx, rx, ty, by);
//				break;
//			case 3://sub
//				PutCmprsImgClipingSub565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 4://H alpha
//				PutCmprsImgClipingHBlend565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by);
//				break;
//			case 5://add not put the same colors between dest and source
//				PutCmprsImgClipingAddDifer565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			case 6://alpha not put the same colors between dest and source
//				PutCmprsImgClipingBlendDifer565(x, y, sp->img, g_pDestBackBuf, lx, rx, ty, by, alpha);
//				break;
//			}
//		}
//		else
//			switch (op) {
//			case 1://alpha
//				PutCmprsImgNOTClipingBlend565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 2://add
//				PutCmprsImgNOTClipingAdd565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 3://sub
//				PutCmprsImgNOTClipingSub565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 4://H alpha
//				PutCmprsImgNOTClipingHBlend565(x, y, yl, sp->img, g_pDestBackBuf);
//				break;
//			case 5://add  not put the same colors between dest and source
//				PutCmprsImgNOTClipingAddDifer565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			case 6://alpha not put the same colors between dest and source
//				PutCmprsImgNOTClipingBlendDifer565(x, y, yl, sp->img, g_pDestBackBuf, alpha);
//				break;
//			}
//	}
//	//#ifdef _SDL
//	//	SDL_UnLock();
//	//#endif
//}
//void InsertMapObject(int moid, int x, int y)
//{
//	int i;
//
//	if (TotalMapObject >= MAX_MAPOBJECT_)
//	{
//		//JustMsg("À½,, ¹ú¼­ MapObject°¹¼ö°¡ 5000³Ñ¾ú¾î¿ä.. ´õÀÌ»óÀº ¹«¸®¿¡¿ä..");
//		return;
//	}
//
//	for (i = 0; i < TotalMapObject; i++)
//	{
//		if (Mo[i].id == moid) break;
//	}
//
//	if (i == TotalMapObject)
//	{
//		memset(&Mo[TotalMapObject], 0, sizeof(MAPOBJECT));
//	}
//	else
//	{
//		memcpy(&Mo[TotalMapObject], &Mo[i], sizeof(MAPOBJECT));
//	}
//
//	Mo[TotalMapObject].id = moid;
//	Mo[TotalMapObject].x = x;
//	Mo[TotalMapObject].y = y;
//
//	Mo[TotalMapObject].animationType = OBJECTANI_NO;
//
//	LPMAPOBJECT mo = &Mo[TotalMapObject];
//
//
//	int frame = LoadMapObjectCSP(mo->id, &MoImage[mo->id - 1]);
//	if (frame) MoImage[mo->id - 1].nFrameCount = frame;
//
//	Mo[TotalMapObject].soundno = 0;
//	Mo[TotalMapObject].soundframe = 0;
//	Mo[TotalMapObject].sounddelay = 0;
//	TotalMapObject++;
//}
//extern int mapOffsetX;
//extern int mapOffsetY;
//
//void DisplayMapObject(LPMAPOBJECT mo)
//{
//
//	Spr* sp = &MoImage[mo->id - 1].spr[mo->curframe];
//	Spr* fsp = &MoImage[mo->id - 1].spr[0];
//
//	int mapx = g_Map.x * TILE_SIZE - g_Map.oX;
//	int mapy = g_Map.y * TILE_SIZE - g_Map.oY;
//
//	int mx = mo->x + mo->offx - mapx;
//	int my = mo->y + mo->offy - mapy;
//	int fspflag = 0;
//
//	/*if (MapNumber == 2)
//	{
//		if (4040 <= mo->id && mo->id <= 4043) { PutCompressedImageWaveTile(mx, my, sp); goto END_; }
//		if (4045 == mo->id) { PutCompressedImageWaveTile(mx, my, sp); goto END_; }
//	}*/
//
//
//	// 020305 YGI
//	// µðÆúÆ® È­¸éÀ» ÂïÀ» ÇÊ¿ä°¡ ¾ø´Ù.
//	//if (!GetSysInfo(SI_NOT_CONNECT_MODE) && mo->objectoritem == ITEMTYPE_COLOSSUS_STONE)	goto END_;
//
//	switch (mo->animationType)
//	{
//	case OBJECTANI_NO:
//	case OBJECTANI_RETURNLOOP:
//	case OBJECTANI_LOOP:
//	case OBJECTANI_TOGGLE_ANI:	switch (mo->attr1 & 0x0003)
//	{
//	case OBJECTSPRITE_NORMAL:
//
//
//
//		PutCompressedImage(mx, my, sp);
//		break;
//	case OBJECTSPRITE_TRANS:
//		PutCompressedImageFX(mx, my, sp, mo->attr1 >> 3, 1);
//		break;
//	case OBJECTSPRITE_ALPHA:
//		if (mo->light && DayLightControl >= 31)
//		{
//		}
//		else
//		{
//
//			PutCompressedImageFX(mx, my, sp, mo->attr1 >> 3, 2);
//			break;
//		}
//	}
//							 break;
//
//	case OBJECTANI_1_RETURNLOOP:
//	case OBJECTANI_1_LOOP:
//		PutCompressedImage(mx, my, fsp);
//		fspflag = 1;
//
//		// Light¸¦ °¡Áö°í ÀÖ´Ù¸é..
//
//		if (mo->light && DayLightControl >= 31)
//		{
//		}
//		else
//		{
//			switch (mo->attr1 & 0x0003)
//			{
//			case OBJECTSPRITE_NORMAL:
//				PutCompressedImage(mx, my, sp);
//				break;
//			case OBJECTSPRITE_TRANS:
//				PutCompressedImageFX(mx, my, sp, mo->attr1 >> 3, 1);
//				break;
//			case OBJECTSPRITE_ALPHA:
//				PutCompressedImageFX(mx, my, sp, mo->attr1 >> 3, 2);
//				break;
//			}
//		}
//		break;
//	}
//
//#ifdef _DEBUG											
//	//if (mo->attr2 && GetSysInfo(SI_GAME_MAKE_MODE))
//	//{
//	//	//		Box_( mx - fsp->ox, my - fsp->oy, fsp->xl, fsp->yl );
//	//	Line(mo->dx0[0] + mo->x, mo->dy0[0] + mo->y, mo->dx1[1] + mo->x, mo->dy1[1] + mo->y, RGB16(0x00, 0xff, 0x00));
//	//}
//#endif													
//
//
//END_:
//
//	int i, j;
//	int tsx, tsy, tex, tey;
//
//	int sx = mx - sp->ox + Mapx;
//	int sy = my - sp->oy + Mapy;
//	int ex = sx + sp->xl + TILE_SIZE - 1;
//	int ey = sy + sp->yl + TILE_SIZE - 1;
//
//	if (fspflag)
//	{
//		tsx = mx - fsp->ox + Mapx;
//		tsy = my - fsp->oy + Mapy;
//		tex = sx + fsp->xl + TILE_SIZE - 1;
//		tey = sy + fsp->yl + TILE_SIZE - 1;
//
//		if (tsx < sx) sx = tsx;	if (tsy < sy) sy = tsy;
//		if (tex > ex) ex = tex;	if (tey > ey) ey = tey;
//	}
//	ex += ((ex - sx) / 2);
//
//	sx /= TILE_SIZE;
//	sy /= TILE_SIZE;
//	ex /= TILE_SIZE;
//	ey /= TILE_SIZE;
//
//	my += Mapy;
//
//	if (sx < 0) sx = 0;
//	if (sy < 0) sy = 0;
//	if (ex >= g_Map.file.wWidth) ex = g_Map.file.wWidth;
//	if (ey >= g_Map.file.wHeight) ey = g_Map.file.wHeight;
//
//
//	for (i = sx; i < ex; i++)
//		for (j = sy; j < ey; j++)
//		{
//			int front = TileMap[i][j].front;
//			if (front)
//				if (TileMap[i][j].frontoffset * TILE_SIZE >= (DWORD)mo->y)
//				{
//					if (Sou[front].xl == -1)
//						PutTile(i * TILE_SIZE - Mapx, j * TILE_SIZE - Mapy, Sou[front].img, GetSurfacePointer(g_surface));
//					else
//						PutCompressedImage(i * TILE_SIZE - Mapx, j * TILE_SIZE - Mapy, &Sou[front]);
//
//				}
//
//		}
//
//
//}
//int SaveMapObjectCSP(int id, LPMAPOBJECTIMAGE mi) {
//	FILE* fp;
//	char filename[FILENAME_MAX];
//	int lev = id / 1000;
//	if (lev == 1) lev = 0;
//
//	for (int i = 0; i < ITEM_FRAME_MAX_; i++) {
//		if (i == 0)
//			sprintf(filename, "D:/MoonClient/object/%d/%04d.csp", lev, id);
//		else
//			sprintf(filename, "D:/MoonClient/object/%d/%04d%02d.csp", lev, id, i);
//
//		fp = fopen(filename, "wb");
//		if (!fp) {
//			std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
//			return 0;
//		}
//
//		fwrite(&mi->spr[i].xl, sizeof(short), 1, fp);
//		fwrite(&mi->spr[i].yl, sizeof(short), 1, fp);
//		fwrite(&mi->spr[i].ox, sizeof(short), 1, fp);
//		fwrite(&mi->spr[i].oy, sizeof(short), 1, fp);
//		fwrite(&mi->spr[i].size, sizeof(unsigned int), 1, fp);
//		fwrite(mi->spr[i].img, mi->spr[i].size, 1, fp);
//
//		fclose(fp);
//	}
//
//	return 1; // Success
//}
//extern std::string path;
//int LoadMapObjectCSP(int id, LPMAPOBJECTIMAGE mi)
//{
//	FILE* fp;
//	char filename[FILENAME_MAX];
//	char tempmapobject[FILENAME_MAX];
//	short ox, oy;
//	int i;
//
//	if (id == 1447) {
//		volatile int breakpoint = 1; // Debugging breakpoint
//	}
//
//	if (mi->spr[0].img != NULL) return 0;
//	//its corrupeted barantan  i need to replace it choose renes
// //   changemapobjectname(Mapname.c_str(), tempmapobject);
//
//	int lev = id / 1000;
//	if (lev == 1) lev = 0;
//
//	//for (int folderNum = 1; folderNum <= 7; ++folderNum)
//	{
//		for (i = 0; i < ITEM_FRAME_MAX_; i++)
//		{
//			if (i == 0)
//				sprintf(filename, "%s/object/%d/%04d.csp", path.c_str(), lev, id);
//			else
//				sprintf(filename, "%s/object/%d/%04d%02d.csp", path.c_str(), lev, id, i);
//
//			/*if (i == 0)
//			{
//				sprintf(filename, "D:/MoonClient/Object/%d/%04d.csp", folderNum, id);
//			}
//			else
//			{
//				sprintf(filename, "D:/MoonClient/Object/%d/%04d%02d.csp", folderNum, id, i);
//			}*/
//
//			fp = fopen(filename, "rb");
//			if (fp == NULL)
//				return i; //early return so that we don't have blinking objects due to missing frames
//
//			fread(&mi->spr[i].xl, sizeof(short), 1, fp);
//			fread(&mi->spr[i].yl, sizeof(short), 1, fp);
//			fread(&mi->spr[i].ox, sizeof(short), 1, fp);
//			fread(&mi->spr[i].oy, sizeof(short), 1, fp);
//			fread(&mi->spr[i].size, sizeof(unsigned int), 1, fp);
//
//			// clear previous mem
//			delete[] mi->spr[i].img;
//
//			mi->spr[i].img = new char[mi->spr[i].size];
//			fread(mi->spr[i].img, mi->spr[i].size, 1, fp);
//			convert565to555(&mi->spr[i]);
//			//SDL_Texture* texture = GetOrCreateRoofTexture(i, Sou[i].img);
//
//			//if (texture) {
//			   // MapobjectCache[i] = texture; // Store in cache
//
//			//}
//			//
//			fclose(fp);
//		}
//	}
//
//	return i;
//
//
//	// Load additional data from .org file if needed
//	sprintf(filename, "%s/Object/%04d.org", path.c_str(), id);
//	fp = fopen(filename, "rb");
//	if (fp)
//	{
//		fseek(fp, 4, SEEK_SET);
//		fread(&ox, 2, 1, fp);
//		fread(&oy, 2, 1, fp);
//
//		mi->spr[0].ox = ox;
//		mi->spr[0].oy = oy;
//		fclose(fp);
//	}
//
//}
////extern void changemapobjectname(char* oldname, char* newname);
//int SaveTOI2(char* filename) {
//	FILE* fp = nullptr;
//	char szFilePath[FILENAME_MAX];
//	char temptoiname[FILENAME_MAX];
//
//	CharUpper(filename);
//	changemapobjectname(filename, temptoiname);
//
//	sprintf(szFilePath, "%s/Object/%s/%s.toi2", path.c_str(), temptoiname, filename);
//
//	fp = fopen(szFilePath, "wb");
//	if (!fp) {
//		return 0; // Failed to open file
//	}
//
//	// Write 0xFFFF to indicate new format
//	WORD header = 0xFFFF;
//	fwrite(&header, sizeof(WORD), 1, fp);
//
//	// Write total map object count
//	WORD totalObject = TotalMapObject;
//	fwrite(&totalObject, sizeof(WORD), 1, fp);
//
//	// Write TotalMapObjectID (can be set to 0 or used as needed)
//	WORD dummyObjectID = 0;
//	fwrite(&dummyObjectID, sizeof(WORD), 1, fp);
//
//	// Placeholder for unknown/int array section from LoadTOI
//	std::vector<int> dummyData(totalObject, 0);
//	fwrite(dummyData.data(), sizeof(int), totalObject, fp);
//
//	for (int i = 0; i < totalObject; ++i) {
//		fwrite(&Mo[i], sizeof(MAPOBJECT), 1, fp);
//	}
//
//	fclose(fp);
//	return 1;
//}
//
//bool g_bIsChristmas = false;
//int LoadTOI(char* filename)
//{
//	FILE* fp = 0;		// 021030 YGI
//	char szFilePath[FILENAME_MAX];
//	char temptoiname[FILENAME_MAX];
//	WORD i, j;
//	int  no;
//	int  count = 0;
//	int  ox, oy;
//	short id;
//	MAPOBJECT_old moold;
//	MAPOBJECT mo;
//	bool newtoi;
//
//	CharUpper(filename);
//
//	changemapobjectname(filename, temptoiname);
//	if (!fp)
//	{
//		if (true == g_bIsChristmas) // Use the christmas toi2 during december
//		{
//			sprintf(szFilePath, "%s/Object/%s/christmas_%s.toi2", path.c_str(), temptoiname, filename);
//			fp = fopen(szFilePath, "rb");
//			if (NULL == fp)
//			{
//				sprintf(szFilePath, "%s/Object/%s/%s.toi2", path.c_str(), temptoiname, filename);
//				fp = fopen(szFilePath, "rb");
//			}
//		}
//		else
//		{
//			sprintf(szFilePath, "%s/Object/%s/%s.toi2", path.c_str(), temptoiname, filename);
//			fp = fopen(szFilePath, "rb");
//		}
//	}
//
//	if (fp)
//	{
//		fread(&TotalMapObject, 2, 1, fp);
//
//		if (TotalMapObject == 0xffff)
//		{
//			newtoi = true;
//			fread(&TotalMapObject, 2, 1, fp);
//		}
//		else
//			newtoi = false;
//
//		// 021117 YGI
//		if (TotalMapObject > MAX_MAPOBJECT_)
//		{
//			//Error("Too Many MapObject");
//		}
//
//		fread(&TotalMapObjectID, 2, 1, fp);
//		fseek(fp, TotalMapObject * sizeof(int), SEEK_CUR);
//
//		for (i = 0; i < TotalMapObject; i++)
//		{
//			if (count == 140)
//			{
//				_asm nop;
//			}
//
//
//			if (newtoi == true)
//			{
//				fread(&mo, sizeof(MAPOBJECT), 1, fp);
//			}
//			else
//			{
//				fread(&moold, sizeof(MAPOBJECT_old), 1, fp);
//				memcpy(&mo, &moold, sizeof(MAPOBJECT_old));
//				mo.soundno = 0;
//				mo.soundframe = 0;
//				mo.sounddelay = 0;
//			}
//
//			// object ¸¦ ¼­¹ö¿Í ¿¬°á ¾ÈÇÏ°í º¸¿©ÁÖ·Á¸é.....
//			if (mo.objectoritem == 0 ||
//				mo.objectoritem == ITEMTYPE_WORKTABLE_MINING ||	// ÀÛ¾÷´ë ¿ë±¤·Î(71)
//				mo.objectoritem == ITEMTYPE_WORKTABLE_BATTLE ||
//				mo.objectoritem == ITEMTYPE_WORKTABLE_SUNBAN ||
//				mo.objectoritem == ITEMTYPE_WORKTABLE_MORU ||
//				mo.objectoritem == ITEMTYPE_WORKTABLE_HWADUK ||
//				mo.objectoritem == ITEMTYPE_WORKTABLE_SIHUMKWAN ||
//				mo.objectoritem == ITEMTYPE_WORKTABLE_MULAE ||
//				mo.objectoritem == ITEMTYPE_WORKTABLE_BULDAE ||
//				mo.objectoritem == ITEMTYPE_SIGNBOARD ||
//				mo.objectoritem == ITEMTYPE_DIRECTBOARD ||
//				mo.objectoritem == ITEMTYPE_HP_UP ||
//				mo.objectoritem == ITEMTYPE_MP_UP ||
//				mo.objectoritem == ITEMTYPE_SCALE ||// 010614 khs
//				mo.objectoritem == ITEMTYPE_NATION_STONE ||
//				mo.objectoritem == ITEMTYPE_GUILD_STONE ||
//				mo.objectoritem == ITEMTYPE_SENARIO_STONE ||
//				mo.objectoritem == ITEMTYPE_COLOSSUS_STONE)
//			{
//				//if( mo.id == 52428 ) continue;
//
//				memcpy(&Mo[count], &mo, sizeof(MAPOBJECT));
//
//				int frame = LoadMapObjectCSP(Mo[count].id, &MoImage[Mo[count].id - 1]);
//				if (frame) MoImage[Mo[count].id - 1].nFrameCount = frame;
//
//				if (MoImage[Mo[count].id - 1].nFrameCount != 0)
//				{
//					Mo[count].curframe = Random(MoImage[Mo[count].id - 1].nFrameCount);
//				}
//				else
//				{
//					Mo[count].curframe = 0;
//				}
//
//				count++;
//			}
//
//			/*	if (HandleRunning(&connections) <= 0)
//				{
//					Release(&connections);
//					return(0);
//				}*/
//		}
//		fclose(fp);
//
//		TotalMapObject = count;
//
//		return 1;
//	}
//	else
//	{
//		sprintf(szFilePath, "%s/Object/%d/%s.toi", path.c_str(), temptoiname, filename);
//		fp = fopen(szFilePath, "rb");
//		if (fp == NULL)
//		{
//			TotalMapObject = 0;
//		}
//		return 0;
//	}
//
//	fread(&TotalMapObjectID, 2, 1, fp);
//	fseek(fp, 4 + 4 + 2, SEEK_CUR);
//
//	for (i = 0; i < TotalMapObjectID; i++)
//	{
//		fread(&no, 4, 1, fp);
//		fseek(fp, sizeof(SIZE) + sizeof(WORD) + sizeof(POINT) + sizeof(int) + sizeof(LPOBJECT) * 2 + sizeof(struct tagOBJECTHEADER*) * 2 + 2, SEEK_CUR);
//
//		TempMoImageID[i] = no;
//		for (j = 0; j < no; j++)
//		{
//			//			fread( &sizeof(t), sizeof( OBJECT ),1, fp);
//			fread(&id, 2, 1, fp);
//
//			Mo[count].id = id;
//
//			fread(&Mo[count].objectoritem, 2, 1, fp);
//			fread(&Mo[count].dum, 2, 1, fp);
//			fread(&Mo[count].animationType, 1, 1, fp);
//			fread(&Mo[count].attr, 1, 1, fp);
//			fread(&Mo[count].attr1, 1, 1, fp);
//			fread(&Mo[count].attr2, 1, 1, fp);
//
//			fseek(fp, 2, SEEK_CUR);
//			fread(&Mo[count].dir, 2, 1, fp);
//			fseek(fp, 2, SEEK_CUR);
//			fread(&Mo[count].x, 4, 1, fp);
//			fread(&Mo[count].y, 4, 1, fp);
//			fread(&Mo[count].offx, 4, 1, fp);
//			fread(&Mo[count].offy, 4, 1, fp);
//			fread(&ox, 4, 1, fp);
//			fread(&oy, 4, 1, fp);
//			fseek(fp, 8, SEEK_CUR);
//
//			Mo[count].x = Mo[count].x * TILE_SIZE;
//			Mo[count].y = Mo[count].y * TILE_SIZE;
//
//			Mo[count].objectoritem = 0; // Mapobject... 
//
//			if (j == 0)
//			{
//				//				MoImage[ Mo[count].id-1].spr[0].ox = ox;
//				//				MoImage[ Mo[count].id-1].spr[0].oy = oy;
//
//				MoImage[Mo[count].id - 1].nFrameCount = LoadMapObjectCSP(Mo[count].id, &MoImage[Mo[count].id - 1]);
//			}
//
//			Mo[count].curframe = Random(MoImage[Mo[count].id - 1].nFrameCount);
//
//			count++;
//		}
//	}
//
//	TotalMapObject = count;
//	fclose(fp);
//
//	return 1;
//}
//
//
