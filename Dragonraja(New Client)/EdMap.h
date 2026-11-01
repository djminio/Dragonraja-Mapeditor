#pragma once
// =====================================================================
// EdMap.h - a Dragon Raja map as the editor holds it
//
//  * <name>.map  : the tiles (wType, wWidth, wHeight, w*h TILE, [checksum], cOdd, cEven).
//                  Two layouts: the old one, and the newer one that starts with the DWORD 0xffff
//                  and has a checksum after the tiles (client map.cpp MapReadFile). Saving keeps
//                  the layout the file had.
//  * <name>.sou2 : which tile pictures the map uses: entry i = (tile no, source no) - the map's
//                  back / front / roof numbers are indexes into this list.
//  * MA-IN.sou, K_SUNG2.sou, ... (11 sources) : the pictures, next to the map.
// =====================================================================
#include <cstdint>
#include <string>
#include <vector>

// the game's TILE (client map.h) - 8 bytes, the same bit layout
struct EdTile
{
	uint32_t back : 12;
	uint32_t front : 12;
	uint32_t empty1 : 1;		// fronttype
	uint32_t rooftype : 1;
	uint32_t attr_dont : 1;		// can't walk here
	uint32_t attr_map : 1;		// event tile
	uint32_t attr_room : 2;
	uint32_t empty2 : 1;
	uint32_t attr_skill : 1;

	uint32_t roof : 12;
	uint32_t show_roof : 1;
	uint32_t frontoffset : 10;
	uint32_t occupied : 1;
	uint32_t canmatch : 1;		// fighting here is not PK
	uint32_t empty4 : 1;
	uint32_t attr_light : 1;
	uint32_t attr_inside : 1;
	uint32_t attr_river : 1;
	uint32_t attr_magic : 1;
	uint32_t attr_no_battle : 1;
	uint32_t empty6 : 1;
};
static_assert(sizeof(EdTile) == 8, "EdTile must be the game's 8-byte TILE");

inline bool SameTile(const EdTile& a, const EdTile& b) { return reinterpret_cast<const uint64_t&>(a) == reinterpret_cast<const uint64_t&>(b); }

// one tile picture: a raw 32x32 tile (xl == -1) or a compressed sprite (RLE, RGB565)
struct EdSprite
{
	short xl = 0, yl = 0, ox = 0, oy = 0;
	bool raw = false;
	short tileNo = 0, sourceNo = 0;		// where it came from (sou2 entry)
	std::vector<uint8_t> data;
	int W() const { return raw ? 32 : xl; }
	int H() const { return raw ? 32 : yl; }
};

// one map object (the client's MAPOBJECT, object.h) - 132 bytes as stored in <map>.toi2
struct MapObj
{
	uint16_t id;				// picture: object/<id/1000>/<id>.csp (+ <id>01.csp ... more frames)
	uint8_t  status;
	uint16_t objectoritem;		// 0 = an object, else an item / work table type
	int8_t   curframe;
	uint16_t dum;
	uint8_t  animationType;		// 1 none 2 return loop 3 loop 4 1+return loop 5 1+loop 6 toggle
	int32_t  x, y;				// map pixels
	uint8_t  attr;
	uint8_t  attr1;				// bits 0-1: 0 normal 1 trans 2 alpha; bits 3-7: the alpha (0-31)
	uint8_t  attr2;				// 1 = sorted with the characters (has a "don't box")
	uint16_t dir;
	int32_t  offx, offy;
	int16_t  dx0[10], dy0[10], dx1[10], dy1[10];
	uint8_t  light;
	int16_t  lightx, lighty;
	int8_t   delay;
	int8_t   anidir;
	uint16_t soundno, soundframe, sounddelay;
};
static_assert(sizeof(MapObj) == 132, "MapObj must be the client's 132-byte MAPOBJECT");
const size_t MAPOBJ_OLD_SIZE = 124;		// the older .toi2 records have no sound fields

class EdMap
{
public:
	bool Load(const std::string& mapPath, std::string& err);
	bool Save(const std::string& mapPath, std::string& err);			// writes the .map (and copies the .sou2 when saving under a new name)
	void NewMap(int w, int h, const EdTile& fill);						// keeps the loaded pictures

	bool Loaded() const { return m_w > 0 && m_h > 0; }
	int Width() const { return m_w; }
	int Height() const { return m_h; }
	bool In(int x, int y) const { return x >= 0 && y >= 0 && x < m_w && y < m_h; }
	EdTile& At(int x, int y) { return m_tiles[(size_t)y * m_w + x]; }
	const EdTile& At(int x, int y) const { return m_tiles[(size_t)y * m_w + x]; }
	std::vector<EdTile>& Tiles() { return m_tiles; }

	const std::vector<EdSprite>& Sprites() const { return m_spr; }
	int SpriteCount() const { return (int)m_spr.size(); }
	const EdSprite* Sprite(int i) const { return i > 0 && i < (int)m_spr.size() && (m_spr[i].raw || !m_spr[i].data.empty()) ? &m_spr[i] : nullptr; }

	const std::string& Path() const { return m_path; }
	const std::string& Name() const { return m_name; }
	const std::string& Folder() const { return m_folder; }
	// ---- objects (<client>/object/<dir>/<NAME>.toi2, client object.cpp LoadTOI)
	std::vector<MapObj> Objects;
	std::vector<int32_t> ObjExtra;		// the per-object int table the client writes and skips
	uint16_t ObjTotalId = 0;
	std::string ObjPath;				// "" = this map has no .toi2 (Save makes one)
	bool LoadObjects(std::string& err);
	bool SaveObjects(std::string& err);
	// the pictures of an object id (frames 0..9); loaded once, empty when there are no files
	const std::vector<EdSprite>& ObjectFrames(int id);
	const std::string& ClientRoot() const { return m_objRoot; }
	static std::vector<std::string> ClientRoots;	// other client folders to take object pictures from (set by the editor)
	std::vector<std::string> Warnings;		// sources that could not be opened, ...
	int wType = 0, cOdd = 0, cEven = 0;
	bool newFormat = true;					// the 0xffff layout with a checksum
	int checksumError = 0;					// as the client works it out (0 = fine)

private:
	bool LoadSources(const std::string& sou2, std::string& err);
	bool LoadSingleSource(std::string& err);		// no .sou2: the whole <name>.sou, like the client
	int m_w = 0, m_h = 0;
	std::vector<EdTile> m_tiles;
	std::vector<EdSprite> m_spr;
	std::string m_path, m_name, m_folder, m_sou2Path;
	std::string m_root, m_objRoot;										// the client folder (the map folder's parent)
	std::vector<std::vector<EdSprite>> m_objFrames;			// by id
	std::vector<char> m_objTried;
};

// RGB565 -> RGBA (8 bits each)
inline void Rgb565ToRgba(uint16_t p, uint8_t* o)
{
	const int r = (p >> 11) & 31, g = (p >> 5) & 63, b = p & 31;
	o[0] = (uint8_t)((r * 255 + 15) / 31); o[1] = (uint8_t)((g * 255 + 31) / 63); o[2] = (uint8_t)((b * 255 + 15) / 31); o[3] = 255;
}
// a sprite as RGBA pixels (W*H*4); transparent where an RLE sprite skips. false = broken data
// outW / outH: the real size - the game's RLE rows often run one pixel past xl (the client draws them, so do we)
bool DecodeSprite(const EdSprite& s, std::vector<uint8_t>& rgba, int* outW = nullptr, int* outH = nullptr);
