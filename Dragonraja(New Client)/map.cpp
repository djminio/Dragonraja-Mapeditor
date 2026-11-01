#include "map.h"
#include <fstream>
#include <vector>
#include <iostream>
#include "Sprite.h"
#include <cstdio>
#include <windows.h>

TILE Tilemap[720][720];

char* g_DestBackBuf, * g_OldDestBackBuf;

std::vector<TILE> g_tileMap;
MAP g_Mapinfo;

Spr Sou[3600];
int TotalTileNo = 0;

char GameServerDataPath[FILENAME_MAX] = "D:\\Map";
uint8_t* SouBuf = nullptr;
DWORD				g_Inside;
char mapsourcename[11][20] =
{
    "MA-IN","K_SUNG2","MANDUN1","FIREDUN1","GRAY",
    "SUNG_TILE_00","HU_VM","ICE-W01","SOURCE","10","11"
};

// ------------------------------
// MAP LOADING
// ------------------------------
void LoadMap(LPMAP lpmap, std::string mapname)
{
    g_Mapinfo.wScreenWidth = 20;
    g_Mapinfo.wScreenHeight = 11;

    char filename[256];
    snprintf(filename, sizeof(filename), "%s",
        mapname.c_str());

    if (!ReadMapFile(filename, &lpmap->file))
    {
        printf("Loading error\n");
    }
}

bool ReadMapFile(const char* filename, LPMAPFILE map)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
        return false;


    DWORD ver;

    file.read(
        reinterpret_cast<char*>(&ver),
        sizeof(ver)
    );


    if (ver != 0xffff)
    {
        // old format
        file.seekg(0, std::ios::beg);
    }


    file.read(
        reinterpret_cast<char*>(&map->wType),
        sizeof(map->wType)
    );


    file.read(
        reinterpret_cast<char*>(&map->wWidth),
        sizeof(map->wWidth)
    );


    file.read(
        reinterpret_cast<char*>(&map->wHeight),
        sizeof(map->wHeight)
    );


    size_t total =
        map->wWidth *
        map->wHeight;


    g_tileMap.resize(total);



    int checksum = 0;


    for (int y = 0; y < map->wHeight; y++)
    {
        for (int x = 0; x < map->wWidth; x++)
        {
            TILE tile;


            file.read(
                reinterpret_cast<char*>(&tile),
                sizeof(TILE)
            );

            Tilemap[x][y] = tile;
            g_tileMap[y * map->wWidth + x] = tile;


            checksum += *(int*)&tile;
        }
    }



    int storedChecksum;

    file.read(
        reinterpret_cast<char*>(&storedChecksum),
        sizeof(storedChecksum)
    );


    map->cOdd = 0;
    map->cEven = 0;


    file.read(
        reinterpret_cast<char*>(&map->cOdd),
        sizeof(map->cOdd)
    );


    file.read(
        reinterpret_cast<char*>(&map->cEven),
        sizeof(map->cEven)
    );



    printf(
        "Map Type:%d Width:%d Height:%d Tiles:%zu Check:%d\n",
        map->wType,
        map->wWidth,
        map->wHeight,
        g_tileMap.size(),
        storedChecksum - checksum
    );


    return true;
}


uint32_t ReturnSizeOfSource2(const char* source2name)
{
	char tempsourcename[FILENAME_MAX];

	std::ifstream sou2fp;
	std::ifstream fp[11];

	SOU2 sou2{};

	uint32_t totalsize = 0;
	uint32_t size = 0;
	uint32_t Off = 0;

	int i = 0;


	sou2fp.open(source2name, std::ios::binary);

	if (!sou2fp.is_open())
	{
		printf("Cannot open: %s\n", source2name);
		return 0;
	}


	for (i = 0; i < 11; i++)
	{
		sprintf_s(
			tempsourcename,
			sizeof(tempsourcename),
			"../map/%s.sou",
			mapsourcename[i]
		);

		fp[i].open(tempsourcename, std::ios::binary);

		if (!fp[i].is_open())
		{
			printf("Cannot open: %s\n", tempsourcename);
		}
	}


	uint32_t fs = 0;
	uint32_t no = 0;

	sou2fp.read(reinterpret_cast<char*>(&fs), sizeof(fs));
	sou2fp.read(reinterpret_cast<char*>(&no), sizeof(no));

	sou2fp.seekg(6, std::ios::cur);


	for (i = 0; i < no; i++)
	{
		short tt = 0;
		short ts = 0;

		sou2fp.read(reinterpret_cast<char*>(&tt), sizeof(tt));
		sou2fp.read(reinterpret_cast<char*>(&ts), sizeof(ts));

		sou2.tile_no = tt;
		sou2.source_no = ts - 1;


		if (sou2.tile_no < 0 || sou2.tile_no >= 3600)
			continue;

		if (sou2.source_no < 0 || sou2.source_no >= 11)
			continue;


		int source = sou2.source_no;
		int tile = sou2.tile_no;


		if (!fp[source].is_open())
			continue;


		fp[source].seekg(4 * tile + 2, std::ios::beg);

		fp[source].read(
			reinterpret_cast<char*>(&Off),
			sizeof(Off)
		);


		fp[source].seekg(Off, std::ios::beg);


		fp[source].read(
			reinterpret_cast<char*>(&size),
			sizeof(size)
		);


		totalsize += size;
	}


	sou2fp.close();

	for (i = 0; i < 11; i++)
	{
		if (fp[i].is_open())
			fp[i].close();
	}


	return totalsize;
}

void LoadMapSource(std::string mapname)
{
    uint32_t no = 0;
    uint32_t nOff = 0;

    std::ifstream fp[11];
    std::ifstream sou2fp;

    uint32_t filesize = 0;
    uint32_t offset = 0;

    Spr* s;


    //
    // Convert .MAP / .map -> .sou2
    //
    std::string sou2name = mapname;

    size_t pos = sou2name.find_last_of('.');

    if (pos != std::string::npos)
    {
        sou2name.replace(
            pos,
            std::string::npos,
            ".sou2"
        );
    }


    printf(
        "Opening SOU2: %s\n",
        sou2name.c_str()
    );


    sou2fp.open(
        sou2name,
        std::ios::binary
    );


    if (!sou2fp.is_open())
    {
        printf(
            "Cannot open %s\n",
            sou2name.c_str()
        );

        return;
    }


    //
    // Check current directory
    //
    char cwd[MAX_PATH];

    GetCurrentDirectoryA(
        MAX_PATH,
        cwd
    );

    printf(
        "Current Directory: %s\n",
        cwd
    );


    //
    // Open .sou source files
    //
    for (int i = 0; i < 11; i++)
    {
        char sourceName[FILENAME_MAX];


        sprintf_s(
            sourceName,
            sizeof(sourceName),
            "%s.sou",
            mapsourcename[i]
        );


        fp[i].open(
            sourceName,
            std::ios::binary
        );


        if (!fp[i].is_open())
        {
            printf(
                "Failed opening %s\n",
                sourceName
            );
        }
        else
        {
            printf(
                "Opened %s\n",
                sourceName
            );
        }
    }



    //
    // Allocate SOU buffer
    //
    filesize = ReturnSizeOfSource2(
        sou2name.c_str()
    );


    printf(
        "SOU2 size: %u\n",
        filesize
    );


    if (filesize == 0)
    {
        printf(
            "Invalid SOU2 size\n"
        );

        return;
    }


    if (SouBuf)
    {
        delete[] SouBuf;
        SouBuf = nullptr;
    }


    SouBuf = new uint8_t[filesize];


    memset(
        SouBuf,
        0,
        filesize
    );



    //
    // Read SOU2 header
    //
    uint32_t fs;


    sou2fp.read(
        (char*)&fs,
        sizeof(fs)
    );


    sou2fp.read(
        (char*)&no,
        sizeof(no)
    );


    printf(
        "fs=%u sprites=%u\n",
        fs,
        no
    );


    if (no == 0 || no > 100000)
    {
        printf(
            "Invalid sprite count\n"
        );

        return;
    }



    //
    // Skip remaining header
    //
    sou2fp.seekg(
        6,
        std::ios::cur
    );



    memset(
        Sou,
        0,
        sizeof(Sou)
    );



    //
    // Load sprites
    //
    for (uint32_t i = 0; i < no; i++)
    {
        short tileNo;
        short sourceNo;


        sou2fp.read(
            (char*)&tileNo,
            sizeof(short)
        );


        sou2fp.read(
            (char*)&sourceNo,
            sizeof(short)
        );


        sourceNo--;



        if (tileNo < 0 ||
            tileNo >= 3600)
        {
            continue;
        }


        if (sourceNo < 0 ||
            sourceNo >= 11)
        {
            continue;
        }



        int tile = tileNo;
        int source = sourceNo;



        //
        // Find sprite offset
        //
        fp[source].seekg(
            4 * tile + 2,
            std::ios::beg
        );


        fp[source].read(
            (char*)&offset,
            sizeof(offset)
        );


        fp[source].seekg(
            offset,
            std::ios::beg
        );



        uint32_t entrySize;


        fp[source].read(
            (char*)&entrySize,
            sizeof(entrySize)
        );



        s = &Sou[i];



        fp[source].read(
            (char*)&s->xl,
            sizeof(short)
        );


        fp[source].read(
            (char*)&s->yl,
            sizeof(short)
        );



        printf(
            "sprite=%u tile=%d source=%d size=%u xl=%d yl=%d\n",
            i,
            tile,
            source,
            entrySize,
            s->xl,
            s->yl
        );



        //
        // Raw tile
        //
        if (s->xl == -1)
        {
            s->size =
                TILE_SIZE *
                TILE_SIZE *
                2;


            s->ox = 0;
            s->oy = 0;



            fp[source].read(
                (char*)(SouBuf + nOff),
                s->size
            );


            s->img =
                (char*)SouBuf + nOff;



            convert565to555RawTile(
                s->img
            );


            nOff += s->size;
        }


        //
        // Compressed sprite
        //
        else
        {
            fp[source].read(
                (char*)&s->ox,
                sizeof(short)
            );


            fp[source].read(
                (char*)&s->oy,
                sizeof(short)
            );


            fp[source].read(
                (char*)&s->size,
                sizeof(uint32_t)
            );



            fp[source].read(
                (char*)(SouBuf + nOff),
                s->size
            );



            s->img =
                (char*)SouBuf + nOff;



            convert565to555(
                s
            );


            nOff += s->size;
        }
    }



    TotalTileNo = no;


    printf(
        "Total sprites loaded %d\n",
        TotalTileNo
    );



    sou2fp.close();



    for (int i = 0; i < 11; i++)
    {
        if (fp[i].is_open())
            fp[i].close();
    }
}
// ------------------------------
// MAP DISPLAY
// ------------------------------

void  MapDisplay(int nType)
{
  //  if (g_StartMenuOn) return;			// 0201 YGI

    register int	x, y;
    int				nIndex;
    LPTILE			t;
    int				mapx, mapy;
    int				temp_X, temp_Y;

    int				mapTileWidth, mapTileHeight;
    int				displayPoint_X = 0, displayPoint_Y = 0;

    if (g_Mapinfo.oX == 0)
    {
        mapx = g_Mapinfo.x;
        displayPoint_X = 0;
        mapTileWidth = SCREEN_WIDTH;		//640
    }
    else if (g_Mapinfo.oX > 0)
    {
        mapx = g_Mapinfo.x - 1;
        displayPoint_X = -32 + g_Mapinfo.oX;
        mapTileWidth = displayPoint_X + (SCREEN_WIDTH+ TILE_SIZE);	//672
    }
    else		//À½¼ö(ÁÂÃø)
    {
        mapx = g_Mapinfo.x;
        displayPoint_X = g_Mapinfo.oX;
        mapTileWidth = displayPoint_X + (SCREEN_WIDTH + TILE_SIZE);
    }
    temp_X = mapx;

    if (g_Mapinfo.oY == 0)
    {
        mapy = g_Mapinfo.y;
        displayPoint_Y = 0;
        mapTileHeight = SCREEN_HEIGHT;
    }
    else if (g_Mapinfo.oY > 0)
    {
        mapy = g_Mapinfo.y - 1;
        displayPoint_Y = -32 + g_Mapinfo.oY;
        mapTileHeight = displayPoint_Y + (SCREEN_HEIGHT + TILE_SIZE);
    }
    else
    {
        mapy = g_Mapinfo.y;
        displayPoint_Y = g_Mapinfo.oY;
        mapTileHeight = displayPoint_Y + (SCREEN_HEIGHT + TILE_SIZE);
    }
    temp_Y = mapy;


    for (y = displayPoint_Y; y < mapTileHeight; y += TILE_SIZE, mapy++)
    {
        for (x = displayPoint_X; x < mapTileWidth; x += TILE_SIZE, mapx++)
        {

            t = &g_tileMap[mapy * g_Mapinfo.file.wWidth + mapx];
            switch (nType)
            {
            case 0:
            {
#ifdef _DEBUG
                //if (tool_DrawMap) return;
#endif
                if (nType == 0)
                {
                    // ¹Ù´Ú
                    if (t->roof == 0 || t->show_roof != 0 || t->rooftype == 1)
                    {
                        {
                            nIndex = t->back;
                            if (nIndex && nIndex < TotalTileNo)
                            {
                                if (Sou[nIndex].xl == -1)
                                {
                                  /*  printf(
                                        "PutTile x=%d y=%d img=%p dest=%p\n",
                                        x,
                                        y,
                                        Sou[nIndex].img,
                                        (void*)g_DestBackBuf
                                    );*/
                                    PutTile(x, y, Sou[nIndex].img, g_DestBackBuf);
                                   
                                }
                                else
                                {
                                    PutCompressedImage(x, y, &Sou[nIndex]);
                                 
                                }
                            }
                        }
                    }
                }
                break;
            }
            case 1:	// ÁöºØ
            {
                nIndex = t->roof;
                if (nIndex && nIndex < TotalTileNo)
                {
                    if (t->show_roof == 0)
                    {
                        if (Sou[nIndex].xl == -1)
                        {
                            PutTile(x, y, Sou[nIndex].img, g_DestBackBuf);
                        
                        }
                        else
                        {
                            PutCompressedImage(x, y, &Sou[nIndex]);
                       
                        }
                    }
                }
                break;
            }
            case 2:
            {	// º®
#ifdef _DEBUG
                //if (tool_DrawFrontMap) return;
#endif					
                if (t->roof == 0 || t->show_roof || t->rooftype == 1)
                {
                    nIndex = t->front;
                    if (nIndex && nIndex < TotalTileNo)
                    {
                        if (Sou[nIndex].xl == -1)
                            PutTile(x, y, Sou[nIndex].img, g_DestBackBuf);
                        else
                            PutCompressedImage(x, y, &Sou[nIndex]);
                    }
                }
                break;
            }
            }// switch
        }
        mapx = temp_X;
    }
}