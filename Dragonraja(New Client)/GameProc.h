#pragma once
extern void keyboardProc();
extern void MouseProc();
#define MAX_CHARACTER_SPRITE_ 200
#define MAX_ORDERTABLE 1000
#include"stdafx.h"
typedef struct tagORDERTABLE {
    short show;
    WORD wType;
    int x, y;
    int height;
    int offset;
    SIZE size;
    LPVOID lpvData;
    LPVOID* lppvData;
    int type;
    int alpha;
    DWORD rgb;
    int rank;
    int sort;
    void* p;
    int sprno;
} ORDERTABLE, * LPORDERTABLE;

typedef struct tagORDERINFO {
    int count;
    ORDERTABLE order[MAX_ORDERTABLE];
} ORDERINFO, * LPORDERINFO;
extern int BoxAndDotCrash(int dx, int dy, int dxl, int dyl, int x, int y);
extern int BoxAndBoxCrash_(int ax, int ay, int axl, int ayl, int bx, int by, int bxl, int byl);
extern int Determinant(int ax, int ay, int bx, int by, int X, int Y);
extern int SortWithObjectObject(LPORDERTABLE oi, LPORDERTABLE oj);
extern int SortWithObject(LPORDERTABLE oi, LPORDERTABLE oj);