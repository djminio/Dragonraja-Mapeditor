#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>
#include "EdMap.h"
#include <cstdio>
#include <cstring>
#include <fstream>

namespace
{
	// the 11 picture sources, in the order the .sou2 numbers them (client map.cpp)
	const char* SOURCES[11] = { "MA-IN", "K_SUNG2", "MANDUN1", "FIREDUN1", "GRAY", "SUNG_TILE_00", "HU_VM", "ICE-W01", "SOURCE", "10", "11" };

	std::string FolderOf(const std::string& p) { const size_t s = p.find_last_of("\\/"); return s == std::string::npos ? std::string(".") : p.substr(0, s); }
	std::string FileOf(const std::string& p) { const size_t s = p.find_last_of("\\/"); return s == std::string::npos ? p : p.substr(s + 1); }
	std::string StemOf(const std::string& p) { std::string f = FileOf(p); const size_t d = f.find_last_of('.'); return d == std::string::npos ? f : f.substr(0, d); }
	bool Exists(const std::string& p) { std::ifstream f(p, std::ios::binary); return f.good(); }
	bool DirExists(const std::string& p) { std::error_code ec; return std::filesystem::is_directory(p, ec); }

	std::string Upper(std::string s) { for (char& c : s) if (c >= 'a' && c <= 'z') c = (char)(c - 32); return s; }
	std::string ObjectDirOf(const std::string& mapName);
	template <class T> bool Rd(std::istream& f, T& v) { f.read(reinterpret_cast<char*>(&v), sizeof(T)); return (bool)f; }
	template <class T> void Wr(std::ostream& f, const T& v) { f.write(reinterpret_cast<const char*>(&v), sizeof(T)); }
}

bool DecodeSprite(const EdSprite& s, std::vector<uint8_t>& rgba, int* outW, int* outH)
{
	int w = s.W(), h = s.H();
	if (w <= 0 || h <= 0 || w > 2048 || h > 2048) return false;
	const uint8_t* const begin = s.data.data();
	const uint8_t* const end = begin + s.data.size();
	if (s.raw)
	{
		if (s.data.size() < 32 * 32 * 2) return false;
		rgba.assign((size_t)32 * 32 * 4, 0);
		for (int i = 0; i < 32 * 32; ++i) Rgb565ToRgba((uint16_t)(begin[i * 2] | (begin[i * 2 + 1] << 8)), &rgba[(size_t)i * 4]);
		if (outW) *outW = 32;
		if (outH) *outH = 32;
		return true;
	}
	// RLE: each row is (skip bytes, count bytes, count bytes of pixels)... ended by 0xFFFF.
	// First pass: how wide the rows really are (the client draws whole rows, past xl if they go there).
	{
		const uint8_t* p = begin; int maxX = 0;
		for (int y = 0; y < h && p + 2 <= end; ++y)
		{
			int x = 0;
			for (;;)
			{
				if (p + 2 > end) break;
				const uint16_t skip = (uint16_t)(p[0] | (p[1] << 8)); p += 2;
				if (skip == 0xFFFF) break;
				if (p + 2 > end) break;
				const uint16_t count = (uint16_t)(p[0] | (p[1] << 8)); p += 2;
				x += skip / 2 + count / 2; p += count;
			}
			if (x > maxX) maxX = x;
		}
		if (maxX > w && maxX <= 2048) w = maxX;
	}
	rgba.assign((size_t)w * h * 4, 0);
	const uint8_t* p = begin;
	for (int y = 0; y < h; ++y)
	{
		int x = 0;
		for (;;)
		{
			if (p + 2 > end) { if (outW) *outW = w; if (outH) *outH = h; return y > 0; }
			const uint16_t skip = (uint16_t)(p[0] | (p[1] << 8)); p += 2;
			if (skip == 0xFFFF) break;
			if (p + 2 > end) return false;
			const uint16_t count = (uint16_t)(p[0] | (p[1] << 8)); p += 2;
			x += skip / 2;
			if (p + count > end) return false;
			for (int i = 0; i + 1 < count; i += 2, ++x)
				if (x >= 0 && x < w) Rgb565ToRgba((uint16_t)(p[i] | (p[i + 1] << 8)), &rgba[((size_t)y * w + x) * 4]);
			p += count;
		}
	}
	if (outW) *outW = w;
	if (outH) *outH = h;
	return true;
}

bool EdMap::Load(const std::string& mapPath, std::string& err)
{
	Warnings.clear(); checksumError = 0;
	std::ifstream f(mapPath, std::ios::binary);
	if (!f) { err = "Can't open " + mapPath; return false; }
	f.seekg(0, std::ios::end); const std::streamoff fileSize = f.tellg(); f.seekg(0);

	uint32_t ver = 0; Rd(f, ver);
	bool nf = ver == 0xffff;
	if (!nf) f.seekg(0);
	uint16_t type = 0, w = 0, h = 0;
	if (!Rd(f, type) || !Rd(f, w) || !Rd(f, h) || w == 0 || h == 0 || w > 2000 || h > 2000) { err = "Not a Dragon Raja map (bad size) - " + mapPath; return false; }
	const std::streamoff need = (nf ? 4 : 0) + 6 + (std::streamoff)w * h * 8 + 2 + (nf ? 4 : 0);
	if (fileSize < need) { err = "The map file is cut short (" + std::to_string((long long)fileSize) + " of " + std::to_string((long long)need) + " bytes)"; return false; }

	std::vector<EdTile> tiles((size_t)w * h);
	int sum = 0;
	for (int y = 0; y < h; ++y)			// the client reads row by row (y outer)
		for (int x = 0; x < w; ++x)
		{
			EdTile& t = tiles[(size_t)y * w + x];
			f.read(reinterpret_cast<char*>(&t), sizeof(EdTile));
			sum += *reinterpret_cast<const int*>(&t);
		}
	int stored = 0;
	if (nf) { Rd(f, stored); checksumError = stored - sum; }
	char odd = 0, even = 0; Rd(f, odd); Rd(f, even);
	if (!f) { err = "Could not read the whole map"; return false; }

	m_tiles.swap(tiles); m_w = w; m_h = h; wType = type; cOdd = odd; cEven = even; newFormat = nf;
	m_path = mapPath; m_folder = FolderOf(mapPath); m_name = StemOf(mapPath);
	if (checksumError) Warnings.push_back("The map's checksum doesn't match its tiles (the game reports a checksum error for this file).");

	// the pictures: <name>.sou2 next to the map
	std::string sou2 = m_folder + "\\" + m_name + ".sou2";
	if (!Exists(sou2)) sou2 = m_folder + "\\" + m_name + ".SOU2";
	m_spr.clear();
	std::string e2;
	if (!LoadSources(sou2, e2) && !LoadSingleSource(e2)) Warnings.push_back(e2 + " - the tiles show as numbers only.");
	m_root = FolderOf(m_folder);
	m_objFrames.clear(); m_objTried.clear();
	std::string e3;
	if (!LoadObjects(e3) && !e3.empty()) Warnings.push_back(e3);
	return true;
}

bool EdMap::LoadSources(const std::string& sou2, std::string& err)
{
	m_sou2Path.clear();
	std::ifstream s2(sou2, std::ios::binary);
	if (!s2) { err = "No " + FileOf(sou2); return false; }
	m_sou2Path = sou2;
	uint32_t fs = 0, no = 0;
	Rd(s2, fs); Rd(s2, no);
	if (no == 0 || no > 4096) { err = FileOf(sou2) + ": bad picture count " + std::to_string(no); return false; }
	s2.seekg(6, std::ios::cur);

	std::ifstream src[11];
	for (int i = 0; i < 11; ++i)
	{
		const std::string tries[] = { m_folder + "\\" + SOURCES[i] + ".sou", m_folder + "\\..\\map\\" + SOURCES[i] + ".sou", std::string(SOURCES[i]) + ".sou" };
		for (const std::string& t : tries) { src[i].open(t, std::ios::binary); if (src[i]) break; src[i].clear(); }
	}
	bool warned[11] = { false };
	m_spr.assign(no, EdSprite());
	for (uint32_t i = 0; i < no; ++i)
	{
		short tileNo = 0, sourceNo = 0;
		if (!Rd(s2, tileNo) || !Rd(s2, sourceNo)) break;
		--sourceNo;
		EdSprite& sp = m_spr[i];
		sp.tileNo = tileNo; sp.sourceNo = sourceNo;
		if (tileNo < 0 || tileNo >= 3600 || sourceNo < 0 || sourceNo >= 11) continue;
		std::ifstream& f = src[sourceNo];
		if (!f.is_open()) { if (!warned[sourceNo]) { warned[sourceNo] = true; Warnings.push_back(std::string("Missing picture source ") + SOURCES[sourceNo] + ".sou"); } continue; }
		f.clear();
		f.seekg(4 * tileNo + 2, std::ios::beg);
		uint32_t off = 0, entry = 0;
		if (!Rd(f, off)) continue;
		f.seekg(off, std::ios::beg);
		if (!Rd(f, entry) || !Rd(f, sp.xl) || !Rd(f, sp.yl)) { sp = EdSprite(); continue; }
		if (sp.xl == -1)
		{
			sp.raw = true; sp.ox = sp.oy = 0;
			sp.data.resize(32 * 32 * 2);
			f.read(reinterpret_cast<char*>(sp.data.data()), sp.data.size());
			if (!f) { sp.data.clear(); sp.raw = false; }
		}
		else
		{
			uint32_t size = 0;
			if (!Rd(f, sp.ox) || !Rd(f, sp.oy) || !Rd(f, size) || size > 4 * 1024 * 1024 || sp.xl <= 0 || sp.yl <= 0) { sp.data.clear(); continue; }
			sp.data.resize(size);
			f.read(reinterpret_cast<char*>(sp.data.data()), size);
			if (!f) sp.data.clear();
		}
	}
	return true;
}

// no <name>.sou2: the client reads one whole source instead, ./map/<name>.sou (client map.cpp LoadMapSource,
// with changesourcename: ICE-W02/03 -> ICE-W01, MANDUN2/3 -> MANDUN1, FIREDUN2/3 -> FIREDUN1); picture i = tile i
bool EdMap::LoadSingleSource(std::string& err)
{
	std::string n = Upper(m_name);
	if (n == "ICE-W02" || n == "ICE-W03") n = "ICE-W01";
	else if (n == "MANDUN2" || n == "MANDUN3") n = "MANDUN1";
	else if (n == "FIREDUN2" || n == "FIREDUN3") n = "FIREDUN1";
	std::ifstream f(m_folder + "\\" + n + ".sou", std::ios::binary);
	if (!f) { err += " and no " + n + ".sou"; return false; }
	short no = 0;
	if (!Rd(f, no) || no <= 0 || no > 4096) { err = n + ".sou: bad picture count"; return false; }
	f.seekg((std::streamoff)no * 4, std::ios::cur);
	m_spr.assign(no, EdSprite());
	for (int i = 0; i < no; ++i)
	{
		EdSprite& sp = m_spr[i];
		sp.tileNo = (short)i; sp.sourceNo = -1;
		uint32_t entry = 0;
		if (!Rd(f, entry) || !Rd(f, sp.xl) || !Rd(f, sp.yl)) { sp = EdSprite(); break; }
		if (sp.xl == -1)
		{
			sp.raw = true;
			sp.data.resize(32 * 32 * 2);
			f.read(reinterpret_cast<char*>(sp.data.data()), sp.data.size());
			if (!f) { sp = EdSprite(); break; }
		}
		else
		{
			uint32_t size = 0;
			if (!Rd(f, sp.ox) || !Rd(f, sp.oy) || !Rd(f, size) || size > 4 * 1024 * 1024) { sp = EdSprite(); break; }
			sp.data.resize(size);
			f.read(reinterpret_cast<char*>(sp.data.data()), size);
			if (!f) { sp = EdSprite(); break; }
		}
	}
	m_sou2Path.clear();
	return true;
}

void EdMap::NewMap(int w, int h, const EdTile& fill)
{
	m_w = w; m_h = h; m_tiles.assign((size_t)w * h, fill);
	Objects.clear(); ObjExtra.clear(); ObjTotalId = 0; ObjPath.clear();
	newFormat = true; checksumError = 0; cOdd = cEven = 0; wType = 0;
	m_path.clear(); m_name = "untitled";
}

bool EdMap::Save(const std::string& mapPath, std::string& err)
{
	// first save over an existing file: keep the original as <file>.bak
	if (Exists(mapPath) && !Exists(mapPath + ".bak"))
	{
		std::ifstream in(mapPath, std::ios::binary); std::ofstream bak(mapPath + ".bak", std::ios::binary);
		bak << in.rdbuf();
	}
	const std::string tmp = mapPath + ".tmp";
	{
		std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
		if (!f) { err = "Can't write " + tmp + " (read-only folder?)"; return false; }
		if (newFormat) Wr(f, (uint32_t)0xffff);
		Wr(f, (uint16_t)wType); Wr(f, (uint16_t)m_w); Wr(f, (uint16_t)m_h);
		int sum = 0;
		for (int y = 0; y < m_h; ++y)
			for (int x = 0; x < m_w; ++x)
			{
				const EdTile& t = At(x, y);
				f.write(reinterpret_cast<const char*>(&t), sizeof(EdTile));
				sum += *reinterpret_cast<const int*>(&t);
			}
		if (newFormat) Wr(f, sum);			// the checksum the client checks
		Wr(f, (char)cOdd); Wr(f, (char)cEven);
		if (!f) { err = "Writing " + tmp + " failed (disk full?)"; return false; }
	}
	std::remove(mapPath.c_str());
	if (std::rename(tmp.c_str(), mapPath.c_str()) != 0) { err = "Could not replace " + mapPath + " (is the file open in the game?)"; return false; }

	// saved under a new name: the map needs its picture list too
	const std::string newSou2 = FolderOf(mapPath) + "\\" + StemOf(mapPath) + ".sou2";
	if (!m_sou2Path.empty() && !Exists(newSou2))
	{
		std::ifstream in(m_sou2Path, std::ios::binary); std::ofstream out(newSou2, std::ios::binary);
		out << in.rdbuf();
		m_sou2Path = newSou2;
	}
	const bool renamed = Upper(StemOf(mapPath)) != Upper(m_name) || ObjPath.empty();
	m_path = mapPath; m_folder = FolderOf(mapPath); m_name = StemOf(mapPath); checksumError = 0;
	if (m_root.empty()) m_root = FolderOf(m_folder);
	if (renamed) ObjPath = m_objRoot + "\\object\\" + ObjectDirOf(m_name) + "\\" + Upper(m_name) + ".toi2";		// Save as: the objects go with the new name
	return true;
}

// ===================================================================== objects
namespace
{
	// client map.cpp changemapobjectname: the object folder of a map
	std::string ObjectDirOf(const std::string& mapName)
	{
		std::string n = Upper(mapName);
		if (n == "ICE-W02" || n == "ICE-W03") n = "ICE-W01";
		else if (n == "MANDUN2" || n == "MANDUN3") n = "MANDUN1";
		else if (n == "FIREDUN2" || n == "FIREDUN3") n = "FIREDUN1";
		else if (n == "HU_VM" || n == "GRAY") n = "SOURCE";
		if (n == "SOURCE") return "6";
		if (n == "K_SUNG2") return "7";
		if (n == "FIREDUN1") return "4";
		if (n == "ICE-W01") return "3";
		if (n == "MANDUN1") return "5";
		if (n == "SUNG_TILE_00") return "2";
		return "0";
	}
}

std::vector<std::string> EdMap::ClientRoots;

bool EdMap::LoadObjects(std::string& err)
{
	Objects.clear(); ObjExtra.clear(); ObjTotalId = 0; ObjPath.clear();
	// the object pictures live in a client folder (<client>\\object\\<n>\\*.csp). A map opened from somewhere else
	// (the server's GameServerData\\map) has none next to it, so look in the other client folders we know about
	m_objRoot = m_root;
	if (!DirExists(m_root + "\\object"))
		for (const std::string& r : ClientRoots) if (!r.empty() && DirExists(r + "\\object")) { m_objRoot = r; break; }
	const std::string dir = m_objRoot + "\\object\\" + ObjectDirOf(m_name) + "\\";
	const std::string here = m_folder + "\\";		// the server keeps <name>.toi2 next to the map
	const std::string tries[] = { here + m_name + ".toi2", here + Upper(m_name) + ".toi2",
		dir + m_name + ".toi2", dir + Upper(m_name) + ".toi2", dir + Upper(m_name) + ".TOI2" };
	std::ifstream f;
	for (const std::string& t : tries) { f.open(t, std::ios::binary); if (f) { ObjPath = t; break; } f.clear(); }
	if (!f) { ObjPath = dir + Upper(m_name) + ".toi2"; return true; }		// no objects yet: saving makes the file
	uint16_t total = 0; Rd(f, total);
	bool nw = false;
	if (total == 0xffff) { nw = true; Rd(f, total); }
	if (total > 20000) { err = "Objects: bad count in " + FileOf(ObjPath); return false; }
	Rd(f, ObjTotalId);
	ObjExtra.resize(total);
	if (total) f.read(reinterpret_cast<char*>(ObjExtra.data()), total * 4);
	Objects.resize(total);
	for (uint16_t i = 0; i < total; ++i)
	{
		MapObj& o = Objects[i];
		std::memset(&o, 0, sizeof(o));
		f.read(reinterpret_cast<char*>(&o), nw ? sizeof(MapObj) : MAPOBJ_OLD_SIZE);
		if (!f) { Objects.resize(i); ObjExtra.resize(i); err = "Objects: " + FileOf(ObjPath) + " is cut short after " + std::to_string(i) + " objects"; return false; }
	}
	return true;
}

bool EdMap::SaveObjects(std::string& err)
{
	if (ObjPath.empty()) { err = "No object file name"; return false; }
	if (Exists(ObjPath) && !Exists(ObjPath + ".bak"))
	{
		std::ifstream in(ObjPath, std::ios::binary); std::ofstream bak(ObjPath + ".bak", std::ios::binary);
		bak << in.rdbuf();
	}
	const std::string tmp = ObjPath + ".tmp";
	{
		std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
		if (!f) { err = "Can't write " + tmp + " (does the object folder exist?)"; return false; }
		const uint16_t total = (uint16_t)Objects.size();
		Wr(f, (uint16_t)0xffff); Wr(f, total); Wr(f, ObjTotalId);			// the newer layout (with sound fields), like SaveTOI2
		ObjExtra.resize(total, 0);
		if (total) f.write(reinterpret_cast<const char*>(ObjExtra.data()), total * 4);
		for (const MapObj& o : Objects) f.write(reinterpret_cast<const char*>(&o), sizeof(MapObj));
		if (!f) { err = "Writing " + tmp + " failed"; return false; }
	}
	std::remove(ObjPath.c_str());
	if (std::rename(tmp.c_str(), ObjPath.c_str()) != 0) { err = "Could not replace " + ObjPath; return false; }
	return true;
}

const std::vector<EdSprite>& EdMap::ObjectFrames(int id)
{
	static const std::vector<EdSprite> none;
	if (id <= 0 || id > 65535) return none;
	if ((int)m_objFrames.size() <= id) { m_objFrames.resize(id + 1); m_objTried.resize(id + 1, 0); }
	if (!m_objTried[id])
	{
		m_objTried[id] = 1;
		int lev = id / 1000; if (lev == 1) lev = 0;
		for (int i = 0; i < 10; ++i)
		{
			char name[64];
			if (i == 0) std::snprintf(name, sizeof(name), "%04d.csp", id); else std::snprintf(name, sizeof(name), "%04d%02d.csp", id, i);
			std::ifstream f(m_objRoot + "\\object\\" + std::to_string(lev) + "\\" + name, std::ios::binary);
			if (!f) break;
			EdSprite sp; uint32_t size = 0;
			if (!Rd(f, sp.xl) || !Rd(f, sp.yl) || !Rd(f, sp.ox) || !Rd(f, sp.oy) || !Rd(f, size) || size > 8 * 1024 * 1024 || sp.xl <= 0 || sp.yl <= 0) break;
			sp.data.resize(size);
			f.read(reinterpret_cast<char*>(sp.data.data()), size);
			if (!f) break;
			m_objFrames[id].push_back(std::move(sp));
		}
	}
	return m_objFrames[id];
}
