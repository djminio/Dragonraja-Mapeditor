#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>
#include "EdMap.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <sstream>

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
	m_sou2Path.clear(); m_sou2FileCount = 0; m_sou2Dirty = false; m_singleSource = false;
	std::ifstream s2(sou2, std::ios::binary);
	if (!s2) { err = "No " + FileOf(sou2); return false; }
	m_sou2Path = sou2;
	uint32_t fs = 0, no = 0;
	Rd(s2, fs); Rd(s2, no);
	if (no == 0 || no > 4096) { err = FileOf(sou2) + ": bad picture count " + std::to_string(no); return false; }
	m_sou2FileCount = (int)no;
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
	m_sou2Path.clear(); m_singleSource = true;
	return true;
}

void EdMap::NewMap(int w, int h, const EdTile& fill, bool keepPictures)
{
	if (!keepPictures || m_spr.empty())		// an empty picture list: pictures come from other maps (AddSprite)
	{
		m_spr.assign(1, EdSprite()); m_newSheetPics.clear();
		m_sou2Path.clear(); m_sou2FileCount = 0; m_sou2Dirty = true; m_singleSource = false;
	}
	m_w = w; m_h = h; m_tiles.assign((size_t)w * h, fill);
	Objects.clear(); ObjExtra.clear(); ObjTotalId = 0; ObjPath.clear();
	newFormat = true; checksumError = 0; cOdd = cEven = 0; wType = 0;
	m_path.clear(); m_name = "untitled";
}

bool EdMap::Save(const std::string& mapPath, std::string& err)
{
	// recoloured pictures first: the map must not point at sheet slots that were never written
	if (!SaveSheetPictures(err)) { err = "Nothing saved - the recolored tiles could not be written: " + err; return false; }
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
	if (m_sou2Dirty)		// pictures taken from another map: write the longer list
	{
		if (!SaveSou2(newSou2, err)) { err = "The map is saved but its picture list is not: " + err; return false; }
	}
	else if (!m_sou2Path.empty() && !Exists(newSou2))
	{
		std::ifstream in(m_sou2Path, std::ios::binary); std::ofstream out(newSou2, std::ios::binary);
		out << in.rdbuf();
		m_sou2Path = newSou2;
	}
	const bool renamed = Upper(StemOf(mapPath)) != Upper(m_name) || ObjPath.empty();
	m_path = mapPath; m_folder = FolderOf(mapPath); m_name = StemOf(mapPath); checksumError = 0;
	if (m_root.empty()) m_root = FolderOf(m_folder);
	if (m_objRoot.empty()) m_objRoot = m_root;		// a new map no map was opened before
	if (renamed) ObjPath = m_objRoot + "\\object\\" + ObjectDirOf(m_name) + "\\" + Upper(m_name) + ".toi2";		// Save as: the objects go with the new name
	return true;
}

// ===================================================================== pictures from another map
// the .map holds numbers into this map's own .sou2 list, so a picture from another map is found by
// (source, tile) here, or added to the end of the list
int EdMap::FindSprite(const EdSprite& sp) const
{
	for (int i = 1; i < (int)m_spr.size(); ++i)
		if (m_spr[i].tileNo == sp.tileNo && m_spr[i].sourceNo == sp.sourceNo && Sprite(i)) return i;
	return -1;
}

int EdMap::AddSprite(const EdSprite& sp, std::string& err)
{
	if (sp.sourceNo < 0 || sp.sourceNo >= 11 || sp.tileNo <= 0 || sp.tileNo >= 3600) { err = "That picture has no source / tile number (its map has no .sou2)"; return -1; }
	const int have = FindSprite(sp);
	if (have > 0) return have;
	if (m_singleSource || m_spr.empty()) { err = m_name + " has no .sou2 picture list to add to"; return -1; }
	if (m_spr.size() >= 3600) { err = m_name + " already uses 3600 pictures, the most the client loads"; return -1; }
	m_spr.push_back(sp);
	m_sou2Dirty = true;
	return (int)m_spr.size() - 1;
}

// .sou2: DWORD ?, DWORD count, 6 bytes (0, width, height), 3600 x (short tile, short source + 1), 2 bytes.
// The bytes as read are kept; only the count and the added entries change.
bool EdMap::SaveSou2(const std::string& sou2, std::string& err)
{
	std::vector<char> buf;
	if (!m_sou2Path.empty())
	{
		std::ifstream in(m_sou2Path, std::ios::binary);
		if (!in) { err = "Can't read " + m_sou2Path; return false; }
		buf.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	}
	const size_t need = 14 + 3600 * 4 + 2;
	if (buf.empty())		// a new map's list: the header holds its size
	{
		buf.assign(need, 0);
		const uint16_t w = (uint16_t)m_w, h = (uint16_t)m_h;
		std::memcpy(&buf[10], &w, 2); std::memcpy(&buf[12], &h, 2);
	}
	if (buf.size() < need) buf.resize(need, 0);
	const uint32_t count = (uint32_t)m_spr.size();
	std::memcpy(&buf[4], &count, 4);
	for (int i = m_sou2FileCount; i < (int)m_spr.size(); ++i)
	{
		const short e[2] = { m_spr[i].tileNo, (short)(m_spr[i].sourceNo + 1) };
		std::memcpy(&buf[14 + (size_t)i * 4], e, 4);
	}
	if (Exists(sou2) && !Exists(sou2 + ".bak"))
	{
		std::ifstream in(sou2, std::ios::binary); std::ofstream bak(sou2 + ".bak", std::ios::binary);
		bak << in.rdbuf();
	}
	const std::string tmp = sou2 + ".tmp";
	{
		std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
		if (!f) { err = "Can't write " + tmp; return false; }
		f.write(buf.data(), buf.size());
		if (!f) { err = "Writing " + tmp + " failed"; return false; }
	}
	std::remove(sou2.c_str());
	if (std::rename(tmp.c_str(), sou2.c_str()) != 0) { err = "Could not replace " + sou2; return false; }
	m_sou2Path = sou2; m_sou2FileCount = (int)m_spr.size(); m_sou2Dirty = false;
	return true;
}

// ===================================================================== recoloured pictures
namespace
{
	// a .sou sheet: short count, DWORD offset[count], then per picture DWORD size + size bytes
	// (raw: short -1, short 0, 32*32 RGB565; RLE: short xl, yl, ox, oy, DWORD n, n bytes)
	bool ReadSheet(const std::string& path, std::vector<std::vector<char>>& recs, std::string& err)
	{
		std::ifstream f(path, std::ios::binary);
		if (!f) { err = "Can't open " + path; return false; }
		std::vector<char> b((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
		if (b.size() < 2) { err = FileOf(path) + " is empty"; return false; }
		short n = 0; std::memcpy(&n, b.data(), 2);
		if (n <= 0 || b.size() < 2 + (size_t)n * 4) { err = FileOf(path) + ": bad picture count"; return false; }
		recs.assign(n, std::vector<char>());
		for (int i = 0; i < n; ++i)
		{
			uint32_t off = 0, size = 0; std::memcpy(&off, &b[2 + (size_t)i * 4], 4);
			if ((size_t)off + 4 > b.size()) { err = FileOf(path) + ": picture " + std::to_string(i) + " is past the end"; return false; }
			std::memcpy(&size, &b[off], 4);
			if ((size_t)off + 4 + size > b.size()) { err = FileOf(path) + ": picture " + std::to_string(i) + " is cut short"; return false; }
			recs[i].assign(b.begin() + off, b.begin() + off + 4 + size);
		}
		return true;
	}
	bool WriteSheet(const std::string& path, const std::vector<std::vector<char>>& recs, std::string& err)
	{
		if (Exists(path) && !Exists(path + ".bak"))
		{
			std::ifstream in(path, std::ios::binary); std::ofstream bak(path + ".bak", std::ios::binary);
			bak << in.rdbuf();
		}
		const std::string tmp = path + ".tmp";
		{
			std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
			if (!f) { err = "Can't write " + tmp; return false; }
			Wr(f, (short)recs.size());
			uint32_t off = 2 + (uint32_t)recs.size() * 4;
			for (const std::vector<char>& r : recs) { Wr(f, off); off += (uint32_t)r.size(); }
			for (const std::vector<char>& r : recs) f.write(r.data(), r.size());
			if (!f) { err = "Writing " + tmp + " failed (disk full?)"; return false; }
		}
		std::remove(path.c_str());
		if (std::rename(tmp.c_str(), path.c_str()) != 0) { err = "Could not replace " + path + " (is the game running?)"; return false; }
		return true;
	}
	std::vector<char> SheetRecord(const EdSprite& s)
	{
		std::ostringstream o(std::ios::binary);
		if (s.raw) { Wr(o, (uint32_t)(4 + s.data.size())); Wr(o, (short)-1); Wr(o, (short)0); }
		else { Wr(o, (uint32_t)(12 + s.data.size())); Wr(o, s.xl); Wr(o, s.yl); Wr(o, s.ox); Wr(o, s.oy); Wr(o, (uint32_t)s.data.size()); }
		o.write(reinterpret_cast<const char*>(s.data.data()), s.data.size());
		const std::string str = o.str();
		return std::vector<char>(str.begin(), str.end());
	}

	uint16_t RecolorPixel(uint16_t p, const Recolor& rc)
	{
		if (p == 0) return 0;			// black stays black
		float r = ((p >> 11) & 31) / 31.0f, g = ((p >> 5) & 63) / 63.0f, b = (p & 31) / 31.0f;
		const float mx = std::max(r, std::max(g, b)), mn = std::min(r, std::min(g, b)), d = mx - mn;
		float h = 0, s = mx > 0 ? d / mx : 0, v = mx;
		if (d > 0)
		{
			if (mx == r) h = 60.0f * std::fmod((g - b) / d + 6.0f, 6.0f);
			else if (mx == g) h = 60.0f * ((b - r) / d + 2.0f);
			else h = 60.0f * ((r - g) / d + 4.0f);
		}
		h = std::fmod(h + rc.hue + 720.0f, 360.0f);
		s = std::min(1.0f, std::max(0.0f, s * rc.sat));
		v = std::min(1.0f, std::max(0.0f, v * rc.bright));
		const float c = v * s, x = c * (1 - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1)), m = v - c;
		const int k = (int)(h / 60.0f) % 6;
		const float rr[6] = { c, x, 0, 0, x, c }, gg[6] = { x, c, c, x, 0, 0 }, bb[6] = { 0, 0, x, c, c, x };
		r = rr[k] + m; g = gg[k] + m; b = bb[k] + m;
		if (rc.tintAmount > 0)		// towards the tint colour, keeping how light the pixel is
		{
			const float l = 0.299f * r + 0.587f * g + 0.114f * b, a = std::min(1.0f, rc.tintAmount);
			r += (std::min(1.0f, l * rc.tint[0] * 1.6f) - r) * a;
			g += (std::min(1.0f, l * rc.tint[1] * 1.6f) - g) * a;
			b += (std::min(1.0f, l * rc.tint[2] * 1.6f) - b) * a;
		}
		const int R = (int)std::lround(std::min(1.0f, std::max(0.0f, r)) * 31), G = (int)std::lround(std::min(1.0f, std::max(0.0f, g)) * 63), B = (int)std::lround(std::min(1.0f, std::max(0.0f, b)) * 31);
		const uint16_t out = (uint16_t)((R << 11) | (G << 5) | B);
		return out ? out : (uint16_t)0x0821;		// a changed pixel never turns into pure black
	}
}

EdSprite RecolorSprite(const EdSprite& s, const Recolor& rc)
{
	EdSprite o = s;
	uint8_t* p = o.data.data();
	uint8_t* const end = p + o.data.size();
	auto px = [&](uint8_t* q) { const uint16_t v = RecolorPixel((uint16_t)(q[0] | (q[1] << 8)), rc); q[0] = (uint8_t)v; q[1] = (uint8_t)(v >> 8); };
	if (s.raw) { for (; p + 1 < end; p += 2) px(p); return o; }
	// RLE rows: (skip, count, count bytes of pixels)... 0xFFFF - only the pixel bytes change
	for (int y = 0; y < s.yl && p + 2 <= end; ++y)
		for (;;)
		{
			if (p + 2 > end) break;
			const uint16_t skip = (uint16_t)(p[0] | (p[1] << 8)); p += 2;
			if (skip == 0xFFFF) break;
			if (p + 2 > end) break;
			const uint16_t count = (uint16_t)(p[0] | (p[1] << 8)); p += 2;
			if (p + count > end) { p = end; break; }
			for (int i = 0; i + 1 < count; i += 2) px(p + i);
			p += count;
		}
	return o;
}

std::string EdMap::SheetPath(int source) const
{
	if (source < 0 || source >= 11) return std::string();
	const std::string name = std::string(SOURCES[source]) + ".sou";
	std::vector<std::string> tries;
	if (!m_folder.empty()) { tries.push_back(m_folder + "\\" + name); tries.push_back(m_folder + "\\..\\map\\" + name); }
	for (const std::string& r : ClientRoots) if (!r.empty()) tries.push_back(r + "\\map\\" + name);
	tries.push_back(name);
	for (const std::string& t : tries) if (Exists(t)) return t;
	return std::string();
}

int EdMap::AddRecolored(const EdSprite& sp, std::string& err)
{
	const int SHEET = 10;		// 11.sou: about half its slots are unused (they hold slot 0's placeholder)
	if (m_singleSource || m_spr.empty()) { err = m_name + " has no .sou2 picture list to add to"; return -1; }
	if (m_spr.size() >= 3600) { err = m_name + " already uses 3600 pictures, the most the client loads"; return -1; }
	if (!sp.raw && sp.data.empty()) { err = "No picture"; return -1; }
	const std::string sheet = SheetPath(SHEET);
	if (sheet.empty()) { err = "Can't find 11.sou next to the map or in the client's map folder"; return -1; }
	std::vector<std::vector<char>> recs;
	if (!ReadSheet(sheet, recs, err)) return -1;

	std::vector<char> taken(recs.size(), 0);
	taken[0] = 1;
	for (size_t i = 1; i < recs.size(); ++i) if (recs[i] != recs[0]) taken[i] = 1;
	// placeholder slots some map still points at (sky1 / sky2 do) stay as they are
	std::error_code ec;
	for (const auto& e : std::filesystem::directory_iterator(FolderOf(sheet), ec))
	{
		std::string ext = Upper(e.path().extension().string());
		if (ext != ".SOU2") continue;
		std::ifstream f(e.path(), std::ios::binary);
		uint32_t fs = 0, no = 0;
		if (!Rd(f, fs) || !Rd(f, no)) continue;
		f.seekg(6, std::ios::cur);
		for (uint32_t i = 0; i < no && i < 3600; ++i)
		{
			short t = 0, src = 0;
			if (!Rd(f, t) || !Rd(f, src)) break;
			if (src - 1 == SHEET && t >= 0 && t < (int)taken.size()) taken[t] = 1;
		}
	}
	for (const EdSprite& s : m_spr) if (s.sourceNo == SHEET && s.tileNo >= 0 && s.tileNo < (int)taken.size()) taken[s.tileNo] = 1;
	int slot = -1;
	for (size_t i = 1; i < taken.size(); ++i) if (!taken[i]) { slot = (int)i; break; }
	if (slot < 0) { err = "11.sou has no free slot left"; return -1; }

	EdSprite n = sp;
	n.sourceNo = (short)SHEET; n.tileNo = (short)slot;
	m_spr.push_back(n);
	m_newSheetPics.push_back((int)m_spr.size() - 1);
	m_sou2Dirty = true;
	return (int)m_spr.size() - 1;
}

bool EdMap::SaveSheetPictures(std::string& err)
{
	if (m_newSheetPics.empty()) return true;
	const int SHEET = m_spr[m_newSheetPics[0]].sourceNo;
	const std::string sheet = SheetPath(SHEET);
	if (sheet.empty()) { err = std::string("Can't find ") + SOURCES[SHEET] + ".sou"; return false; }
	std::vector<std::vector<char>> recs;
	if (!ReadSheet(sheet, recs, err)) return false;
	for (int i : m_newSheetPics)
	{
		const EdSprite& s = m_spr[i];
		if (s.tileNo <= 0 || s.tileNo >= (int)recs.size()) { err = "Bad sheet slot " + std::to_string(s.tileNo); return false; }
		if (recs[s.tileNo] != recs[0]) { err = FileOf(sheet) + " slot " + std::to_string(s.tileNo) + " was filled by something else since - recolor the tile again"; return false; }
		recs[s.tileNo] = SheetRecord(s);
	}
	if (!WriteSheet(sheet, recs, err)) return false;
	m_newSheetPics.clear();
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
