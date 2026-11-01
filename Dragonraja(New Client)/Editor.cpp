#define _CRT_SECURE_NO_WARNINGS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <commdlg.h>
#include "Editor.h"
#include "EdMap.h"
#include "Theme.h"
#include <imgui.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <climits>
#include <cstring>
#include <map>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// =====================================================================
// Editor.cpp
//   view    : pan (middle mouse drag / space + drag / arrows / WASD), zoom (wheel, + -), grid, layers
//   tools   : 1 Select  2 Paint  3 Erase  4 Fill (drag a box)  5 Attribute  6 Pick (or right-click)
//   undo    : every stroke / box / property change is one step (Ctrl+Z / Ctrl+Y)
//   files   : Ctrl+O open, Ctrl+S save (the .map in the layout it had; first save keeps a .bak)
// =====================================================================
namespace
{
	SDL_Window* s_window = nullptr;
	SDL_Renderer* s_ren = nullptr;
	EdMap s_map;
	std::string s_status = "Open a map: File > Open (Ctrl+O), or drop a .map file on the window. Or start one: File > New map (Ctrl+N).";
	bool s_statusBad = false;
	void Status(const std::string& m, bool bad = false) { s_status = m; s_statusBad = bad; }

	// ------------------------------------------------------------ textures (one per tile picture)
	struct Tex { SDL_Texture* t = nullptr; int w = 0, h = 0; ImU32 avg = 0; bool tried = false; bool solid = false; };		// solid: covers its whole 32x32 square
	std::vector<Tex> s_tex;
	void ClearObjTextures();
	void SeamReset();
	void ClearTextures() { for (Tex& t : s_tex) if (t.t) SDL_DestroyTexture(t.t); s_tex.clear(); ClearObjTextures(); SeamReset(); }
	Tex* GetTexOf(const EdMap& m, std::vector<Tex>& cache, int i)
	{
		const EdSprite* sp = m.Sprite(i);
		if (!sp) return nullptr;
		if ((int)cache.size() < m.SpriteCount()) cache.resize(m.SpriteCount());
		Tex& t = cache[i];
		if (!t.tried)
		{
			t.tried = true;
			std::vector<uint8_t> px;
			if (DecodeSprite(*sp, px, &t.w, &t.h))
			{
				t.t = SDL_CreateTexture(s_ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, t.w, t.h);
				if (t.t) { SDL_UpdateTexture(t.t, nullptr, px.data(), t.w * 4); SDL_SetTextureBlendMode(t.t, SDL_BLENDMODE_BLEND); }
				unsigned long long r = 0, g = 0, b = 0, n = 0;			// the average colour (minimap)
				for (size_t k = 0; k < px.size(); k += 4) if (px[k + 3]) { r += px[k]; g += px[k + 1]; b += px[k + 2]; ++n; }
				if (n) t.avg = IM_COL32((int)(r / n), (int)(g / n), (int)(b / n), 255);
				if (t.w >= 32 && t.h >= 32 && sp->ox == 0 && sp->oy == 0)
				{
					t.solid = true;
					for (int yy = 0; yy < 32 && t.solid; ++yy) for (int xx = 0; xx < 32; ++xx) if (!px[((size_t)yy * t.w + xx) * 4 + 3]) { t.solid = false; break; }
				}
			}
		}
		return t.t ? &t : nullptr;
	}
	Tex* GetTex(int i) { return GetTexOf(s_map, s_tex, i); }
	// another map to take tile pictures from (the "From map" tab)
	EdMap s_src;
	std::vector<Tex> s_srcTex;
	void ClearSrcTextures() { for (Tex& t : s_srcTex) if (t.t) SDL_DestroyTexture(t.t); s_srcTex.clear(); }
	ImTextureID TexId(Tex* t) { return (ImTextureID)(intptr_t)t->t; }
	std::unordered_map<int, Tex> s_objTex;			// id * 16 + frame
	void ClearObjTextures() { for (auto& kv : s_objTex) if (kv.second.t) SDL_DestroyTexture(kv.second.t); s_objTex.clear(); }
	Tex* GetObjTexOf(EdMap& m, std::unordered_map<int, Tex>& cache, int id, int frame)
	{
		Tex& t = cache[id * 16 + frame];
		if (!t.tried)
		{
			t.tried = true;
			const std::vector<EdSprite>& fr = m.ObjectFrames(id);
			std::vector<uint8_t> px;
			if (frame < (int)fr.size() && DecodeSprite(fr[frame], px, &t.w, &t.h))
			{
				t.t = SDL_CreateTexture(s_ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, t.w, t.h);
				if (t.t) { SDL_UpdateTexture(t.t, nullptr, px.data(), t.w * 4); SDL_SetTextureBlendMode(t.t, SDL_BLENDMODE_BLEND); }
			}
		}
		return t.t ? &t : nullptr;
	}
	Tex* GetObjTex(int id, int frame) { return GetObjTexOf(s_map, s_objTex, id, frame); }
	std::unordered_map<int, Tex> s_srcObjTex;		// the other map's object pictures (From map tab)
	void ClearSrcObjTextures() { for (auto& kv : s_srcObjTex) if (kv.second.t) SDL_DestroyTexture(kv.second.t); s_srcObjTex.clear(); }
	// where an object is drawn (frame 0), in map pixels; a 32x32 box at its spot when it has no picture
	struct ORect { float x, y, w, h; };
	ORect ObjRect(const MapObj& o)
	{
		const std::vector<EdSprite>& fr = s_map.ObjectFrames(o.id);
		const float ax = (float)(o.x + o.offx), ay = (float)(o.y + o.offy);
		if (fr.empty()) return { ax - 16, ay - 16, 32, 32 };
		return { ax - fr[0].ox, ay - fr[0].oy, (float)fr[0].W(), (float)fr[0].H() };
	}

	// ------------------------------------------------------------ attributes
	struct AttrDef { const char* name; const char* tip; ImU32 color; int maxv; uint32_t(*get)(const EdTile&); void(*set)(EdTile&, uint32_t); };
	const AttrDef ATTRS[] = {
		{ "Blocked", "attr_dont: can't walk here", IM_COL32(230, 40, 30, 110), 1, [](const EdTile& t) -> uint32_t { return t.attr_dont; }, [](EdTile& t, uint32_t v) { t.attr_dont = v; } },
		{ "Event", "attr_map: an event / warp tile", IM_COL32(250, 210, 40, 120), 1, [](const EdTile& t) -> uint32_t { return t.attr_map; }, [](EdTile& t, uint32_t v) { t.attr_map = v; } },
		{ "Inside", "attr_inside: indoors (no weather, roofs open)", IM_COL32(60, 110, 240, 100), 1, [](const EdTile& t) -> uint32_t { return t.attr_inside; }, [](EdTile& t, uint32_t v) { t.attr_inside = v; } },
		{ "River", "attr_river: water (fishing)", IM_COL32(40, 200, 230, 110), 1, [](const EdTile& t) -> uint32_t { return t.attr_river; }, [](EdTile& t, uint32_t v) { t.attr_river = v; } },
		{ "No battle", "attr_no_battle: no fighting here", IM_COL32(60, 210, 80, 100), 1, [](const EdTile& t) -> uint32_t { return t.attr_no_battle; }, [](EdTile& t, uint32_t v) { t.attr_no_battle = v; } },
		{ "No PK", "canmatch: fighting here is not PK", IM_COL32(240, 140, 40, 100), 1, [](const EdTile& t) -> uint32_t { return t.canmatch; }, [](EdTile& t, uint32_t v) { t.canmatch = v; } },
		{ "Magic", "attr_magic", IM_COL32(170, 70, 230, 100), 1, [](const EdTile& t) -> uint32_t { return t.attr_magic; }, [](EdTile& t, uint32_t v) { t.attr_magic = v; } },
		{ "Light", "attr_light", IM_COL32(255, 255, 210, 90), 1, [](const EdTile& t) -> uint32_t { return t.attr_light; }, [](EdTile& t, uint32_t v) { t.attr_light = v; } },
		{ "Skill", "attr_skill: a job skill spot", IM_COL32(240, 90, 170, 100), 1, [](const EdTile& t) -> uint32_t { return t.attr_skill; }, [](EdTile& t, uint32_t v) { t.attr_skill = v; } },
		{ "Room", "attr_room (0-3)", IM_COL32(40, 170, 150, 100), 3, [](const EdTile& t) -> uint32_t { return t.attr_room; }, [](EdTile& t, uint32_t v) { t.attr_room = v; } },
		{ "Roof type", "rooftype: 1 = the ground under the roof stays drawn", IM_COL32(150, 120, 90, 90), 1, [](const EdTile& t) -> uint32_t { return t.rooftype; }, [](EdTile& t, uint32_t v) { t.rooftype = v; } },
		{ "Show roof", "show_roof: the roof is hidden (open) here", IM_COL32(120, 160, 90, 90), 1, [](const EdTile& t) -> uint32_t { return t.show_roof; }, [](EdTile& t, uint32_t v) { t.show_roof = v; } },
	};
	const int NUM_ATTRS = (int)(sizeof(ATTRS) / sizeof(ATTRS[0]));
	bool s_overlay[NUM_ATTRS] = { true, true, false, false, false, false, false, false, false, false, false, false };

	// ------------------------------------------------------------ view / tools
	enum Tool { T_SELECT, T_PAINT, T_ERASE, T_FILL, T_ATTR, T_PICK, T_OBJECT, T_STAMP };
	enum Layer { L_BACK, L_FRONT, L_ROOF };
	const char* LAYER_NAMES[3] = { "Back", "Front", "Roof" };
	int s_tool = T_SELECT, s_layer = L_BACK, s_brush = 1, s_attr = 0, s_attrValue = 1;
	int s_selSprite = 1;
	bool s_show[3] = { true, true, true };
	bool s_grid = false, s_hideRoofsUnderMouse = false, s_fadeAbove = true;
	float s_camX = 0, s_camY = 0, s_zoom = 1.0f;
	int s_selX = -1, s_selY = -1, s_hoverX = -1, s_hoverY = -1;
	bool s_boxing = false; int s_boxX = 0, s_boxY = 0;
	bool s_dirty = false, s_minimapDirty = true;
	int s_gen = 0;					// bumped on every tile change: the map picture is drawn again
	SDL_Texture* s_canvas = nullptr; int s_canvasW = 0, s_canvasH = 0;
	SDL_Texture* s_minimap = nullptr; int s_mmW = 0, s_mmH = 0; Uint32 s_mmAt = 0;
	char s_paletteFilter[16] = "";
	bool s_palAll = false;				// Tiles list: every picture, not only the ones this layer uses
	// the pictures each layer of the map uses (the Tiles list shows the chosen layer's), worked out again after edits
	std::vector<int> s_layerPics[3]; int s_layerPicsGen = -1;
	void LayerPics()
	{
		if (s_layerPicsGen == s_gen) return;
		s_layerPicsGen = s_gen;
		const int n = s_map.SpriteCount();
		std::vector<char> used[3];
		for (int l = 0; l < 3; ++l) { used[l].assign(std::max(n, 1), 0); s_layerPics[l].clear(); }
		for (const EdTile& t : s_map.Tiles())
		{
			if (t.back && (int)t.back < n) used[0][t.back] = 1;
			if (t.front && (int)t.front < n) used[1][t.front] = 1;
			if (t.roof && (int)t.roof < n) used[2][t.roof] = 1;
		}
		for (int l = 0; l < 3; ++l) for (int i = 1; i < n; ++i) if (used[l][i]) s_layerPics[l].push_back(i);
	}
	// objects
	bool s_showObj = true, s_objDirty = false;
	int s_selObj = -1, s_placeId = 0;
	MapObj s_placeTpl = {};				// placing an id taken from an object (right-click / From map): copies keep its settings
	bool s_objDrag = false; float s_dragDX = 0, s_dragDY = 0;
	std::vector<MapObj> s_dragBefore;
	int s_leftTab = 0;					// 0 tiles, 1 objects, 2 tiles from another map
	char s_objFilter[16] = "";
	bool s_quitAsked = false, s_quitOk = false;
	enum class Pending { None, Open, New, Quit, OpenPath } s_pending = Pending::None;
	std::string s_pendingPath;
	bool s_askNew = false, s_openUnsaved = false; int s_newW = 200, s_newH = 200;

	// ------------------------------------------------------------ undo
	struct Change { int idx; EdTile before, after; };
	struct Step { std::vector<Change> tiles; bool obj = false; std::vector<MapObj> objBefore, objAfter; };
	std::vector<Step> s_undo, s_redo;
	void PushStep(Step&& st) { s_undo.push_back(std::move(st)); s_redo.clear(); if (s_undo.size() > 300) s_undo.erase(s_undo.begin()); }
	// objects: the whole list before and after (a few hundred KB at most)
	void ObjStep(std::vector<MapObj>&& before, const char* what)
	{
		Step st; st.obj = true; st.objBefore = std::move(before); st.objAfter = s_map.Objects;
		PushStep(std::move(st));
		s_objDirty = true; ++s_gen;
		if (what) s_status = what;
	}
	std::unordered_map<int, EdTile> s_stroke;		// tile index -> before, while a stroke is going on
	void Touch(int x, int y) { const int i = y * s_map.Width() + x; if (!s_stroke.count(i)) s_stroke[i] = s_map.Tiles()[i]; }
	void EndStroke()
	{
		std::vector<Change> ch;
		for (auto& kv : s_stroke) if (!SameTile(kv.second, s_map.Tiles()[kv.first])) ch.push_back({ kv.first, kv.second, s_map.Tiles()[kv.first] });
		s_stroke.clear();
		if (ch.empty()) return;
		Step st; st.tiles = std::move(ch); PushStep(std::move(st));
		s_dirty = true; s_minimapDirty = true; ++s_gen;
	}
	void Undo()
	{
		if (s_undo.empty()) return;
		Step& st = s_undo.back();
		if (st.obj) { s_map.Objects = st.objBefore; s_objDirty = true; if (s_selObj >= (int)s_map.Objects.size()) s_selObj = -1; Status("Undone an object change."); }
		else { for (const Change& c : st.tiles) s_map.Tiles()[c.idx] = c.before; Status("Undone " + std::to_string(st.tiles.size()) + " tile(s)."); }
		s_redo.push_back(std::move(s_undo.back())); s_undo.pop_back(); s_dirty = true; s_minimapDirty = true; ++s_gen;
	}
	void Redo()
	{
		if (s_redo.empty()) return;
		Step& st = s_redo.back();
		if (st.obj) { s_map.Objects = st.objAfter; s_objDirty = true; if (s_selObj >= (int)s_map.Objects.size()) s_selObj = -1; Status("Redone an object change."); }
		else { for (const Change& c : st.tiles) s_map.Tiles()[c.idx] = c.after; Status("Redone " + std::to_string(st.tiles.size()) + " tile(s)."); }
		s_undo.push_back(std::move(s_redo.back())); s_redo.pop_back(); s_dirty = true; s_minimapDirty = true; ++s_gen;
	}

	uint32_t LayerGet(const EdTile& t, int l) { return l == L_BACK ? t.back : l == L_FRONT ? t.front : t.roof; }
	void LayerSet(EdTile& t, int l, uint32_t v) { v &= 0xFFF; if (l == L_BACK) t.back = v; else if (l == L_FRONT) t.front = v; else t.roof = v; }

	// ------------------------------------------------------------ settings (recent maps) next to the exe
	std::vector<std::string> s_recent;
	std::string IniPath() { char p[MAX_PATH] = { 0 }; GetModuleFileNameA(nullptr, p, MAX_PATH); std::string s = p; const size_t k = s.find_last_of("\\/"); return (k == std::string::npos ? std::string(".") : s.substr(0, k)) + "\\mapeditor.ini"; }
	std::vector<std::string> s_clientLines;		// client= lines of the ini, kept when it is saved
	void LoadIni()
	{
		std::ifstream f(IniPath()); std::string l;
		std::vector<std::string> clients;
		s_clientLines.clear();
		while (std::getline(f, l))
		{
			if (l.compare(0, 7, "recent=") == 0 && s_recent.size() < 10) s_recent.push_back(l.substr(7));
			else if (l.compare(0, 7, "client=") == 0) { clients.push_back(l.substr(7)); s_clientLines.push_back(l); }		// client=D:\MoonClient - where the object pictures are
		}
		// where to find object pictures for a map opened outside a client folder (the server's map folder):
		// client= lines, the folders of maps opened before, then the usual places
		for (const std::string& r : s_recent) { const size_t k = r.find_last_of("\\/"); if (k != std::string::npos) { const std::string d = r.substr(0, k); const size_t k2 = d.find_last_of("\\/"); if (k2 != std::string::npos) clients.push_back(d.substr(0, k2)); } }
		for (const char* c : { "D:\\MoonClient", "F:\\MoonClient", "C:\\MoonClient", "E:\\MoonClient" }) clients.push_back(c);
		EdMap::ClientRoots = clients;
	}
	void SaveIni() { std::ofstream f(IniPath()); for (const std::string& r : s_recent) f << "recent=" << r << "\n"; for (const std::string& c : s_clientLines) f << c << "\n"; }
	void AddRecent(const std::string& p) { s_recent.erase(std::remove(s_recent.begin(), s_recent.end(), p), s_recent.end()); s_recent.insert(s_recent.begin(), p); if (s_recent.size() > 10) s_recent.resize(10); SaveIni(); }

	std::string FileDialog(bool save, const std::string& start)
	{
		char file[MAX_PATH] = { 0 };
		if (!start.empty()) strncpy(file, start.c_str(), MAX_PATH - 1);
		OPENFILENAMEA ofn = {};
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = "Dragon Raja maps (*.map)\0*.map\0All files\0*.*\0";
		ofn.lpstrFile = file; ofn.nMaxFile = MAX_PATH;
		ofn.lpstrDefExt = "map";
		ofn.Flags = save ? (OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR) : (OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR);
		const BOOL ok = save ? GetSaveFileNameA(&ofn) : GetOpenFileNameA(&ofn);
		return ok ? std::string(file) : std::string();
	}

	void CenterOn(int tx, int ty, float viewW, float viewH)
	{
		s_camX = tx * 32.0f + 16 - viewW / (2 * s_zoom);
		s_camY = ty * 32.0f + 16 - viewH / (2 * s_zoom);
	}
	float s_viewW = 800, s_viewH = 600;

	bool DoOpen(const std::string& path)
	{
		EdMap m;
		std::string err;
		if (!m.Load(path, err)) { Status(err, true); return false; }
		ClearTextures(); ++s_gen;
		s_map = std::move(m);
		s_undo.clear(); s_redo.clear(); s_stroke.clear(); s_dirty = false; s_minimapDirty = true; ++s_gen;
		s_selX = s_selY = -1; s_zoom = 1.0f; s_selObj = -1; s_objDirty = false; s_objDrag = false;
		CenterOn(s_map.Width() / 2, s_map.Height() / 2, s_viewW, s_viewH);
		if (s_selSprite >= s_map.SpriteCount()) s_selSprite = 1;
		AddRecent(path);
		std::string w;
		for (const std::string& x : s_map.Warnings) w += " " + x;
		Status("Opened " + s_map.Name() + " (" + std::to_string(s_map.Width()) + " x " + std::to_string(s_map.Height()) + ", " + std::to_string(s_map.SpriteCount()) + " tile pictures, " + std::to_string(s_map.Objects.size()) + " objects)." + w, !s_map.Warnings.empty());
		if (s_window) SDL_SetWindowTitle(s_window, ("Dragon Raja Map Editor - " + s_map.Name()).c_str());
		return true;
	}
	bool DoSave(bool as)
	{
		if (!s_map.Loaded()) return false;
		std::string path = s_map.Path();
		if (as || path.empty()) { path = FileDialog(true, path.empty() ? s_map.Name() + ".map" : path); if (path.empty()) return false; }
		std::string err;
		const std::string oldObjPath = s_map.ObjPath;
		if (!s_map.Save(path, err)) { Status("Not saved: " + err, true); return false; }
		s_dirty = false; AddRecent(path);
		std::string objMsg;
		if (s_objDirty || s_map.ObjPath != oldObjPath)		// the objects go in their own file (<client>\object\<dir>\<MAP>.toi2)
		{
			const size_t k = s_map.ObjPath.find_last_of("\\/");
			if (k != std::string::npos)
			{
				const std::string dir = s_map.ObjPath.substr(0, k), up = dir.substr(0, dir.find_last_of("\\/"));
				CreateDirectoryA(up.c_str(), nullptr);		// <client>\object (a new map in a folder that has none)
				CreateDirectoryA(dir.c_str(), nullptr);
			}
			if (s_map.SaveObjects(err)) { s_objDirty = false; objMsg = " and " + std::to_string(s_map.Objects.size()) + " objects (" + s_map.ObjPath + ")"; }
			else { Status("The map is saved but the objects are not: " + err, true); return false; }
		}
		Status("Saved " + path + objMsg + (std::ifstream(path + ".bak").good() ? "  (the originals are kept as .bak)" : ""));
		if (s_window) SDL_SetWindowTitle(s_window, ("Dragon Raja Map Editor - " + s_map.Name()).c_str());
		return true;
	}
	void Request(Pending p, const std::string& path = "")
	{
		s_pending = p; s_pendingPath = path;
		if (s_dirty || s_objDirty) { s_openUnsaved = true; return; }		// opened in Dialogs() (a popup opened inside a menu would get the menu's id)
		// nothing to lose: do it now
		Pending q = s_pending; s_pending = Pending::None;
		if (q == Pending::Open) { const std::string f = FileDialog(false, s_map.Path()); if (!f.empty()) DoOpen(f); }
		else if (q == Pending::OpenPath) DoOpen(path);
		else if (q == Pending::New) s_askNew = true;
		else if (q == Pending::Quit) s_quitOk = true;
	}
	void RunPending()
	{
		Pending q = s_pending; s_pending = Pending::None; s_dirty = false; s_objDirty = false;
		if (q == Pending::Open) { const std::string f = FileDialog(false, s_map.Path()); if (!f.empty()) DoOpen(f); }
		else if (q == Pending::OpenPath) DoOpen(s_pendingPath);
		else if (q == Pending::New) s_askNew = true;
		else if (q == Pending::Quit) s_quitOk = true;
	}

	// ------------------------------------------------------------ painting
	void ApplyAt(int cx, int cy, bool clear)
	{
		const int r0 = -(s_brush - 1) / 2, r1 = s_brush / 2;
		for (int dy = r0; dy <= r1; ++dy)
			for (int dx = r0; dx <= r1; ++dx)
			{
				const int x = cx + dx, y = cy + dy;
				if (!s_map.In(x, y)) continue;
				Touch(x, y);
				EdTile& t = s_map.At(x, y);
				if (s_tool == T_PAINT) { if (s_layer == L_ROOF && !t.roof) t.rooftype = 1; LayerSet(t, s_layer, (uint32_t)s_selSprite); }
				else if (s_tool == T_ERASE) LayerSet(t, s_layer, 0);
				else if (s_tool == T_ATTR) ATTRS[s_attr].set(t, clear ? 0u : (uint32_t)s_attrValue);
			}
		++s_gen;
	}
	void FillBox(int x0, int y0, int x1, int y1)
	{
		if (x0 > x1) std::swap(x0, x1);
		if (y0 > y1) std::swap(y0, y1);
		for (int y = y0; y <= y1; ++y) for (int x = x0; x <= x1; ++x) if (s_map.In(x, y)) { Touch(x, y); EdTile& t = s_map.At(x, y); if (s_layer == L_ROOF && !t.roof) t.rooftype = 1; LayerSet(t, s_layer, (uint32_t)s_selSprite); }
		EndStroke();
		Status("Filled " + std::to_string((x1 - x0 + 1) * (y1 - y0 + 1)) + " tiles with " + LAYER_NAMES[s_layer] + " #" + std::to_string(s_selSprite) + ".");
	}
	void Pick(int x, int y)
	{
		if (!s_map.In(x, y)) return;
		const int v = (int)LayerGet(s_map.At(x, y), s_layer);
		if (v) { s_selSprite = v; Status(std::string("Picked ") + LAYER_NAMES[s_layer] + " tile #" + std::to_string(v) + "."); }
		else Status(std::string("No ") + LAYER_NAMES[s_layer] + " tile there.");
	}


	// ------------------------------------------------------------ pattern paint (Paint, Back layer)
	// Dragon Raja ground is cut from bigger pictures: the cobbles are a 3x3 block of tiles repeated, a road 2x2,
	// ... The tile you pick stands for its block (learned from where the map uses it), and painting lays the block
	// in the right order, lined up with the same pattern next to the stroke - so it continues it seamlessly.
	// Only the tiles under the brush change. (Soft edges in the game maps are overlay tiles on the Front layer:
	// copy a real edge with the Select tool and stamp it - Ctrl+C / Ctrl+V.)
	struct PatternData { int forTile = -1, pw = 1, ph = 1; std::vector<int> t; std::unordered_map<int, int> where; };
	PatternData s_pat;
	bool s_seamless = true;
	bool s_patPhase = false; int s_patOX = 0, s_patOY = 0;		// where the block starts, for this stroke
	void SeamReset() { s_pat.forTile = -1; }
	const PatternData& LearnPattern(int A)
	{
		PatternData& P = s_pat;
		if (P.forTile == A) return P;
		P = PatternData(); P.forTile = A;
		const int W = s_map.Width(), H = s_map.Height();
		std::vector<std::pair<int, int>> occ;
		for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) if ((int)s_map.At(x, y).back == A) occ.push_back({ x, y });
		auto Self = [&](int dx, int dy)
		{
			int same = 0, tot = 0;
			for (const auto& o : occ) { if (!s_map.In(o.first + dx, o.second + dy)) continue; ++tot; if ((int)s_map.At(o.first + dx, o.second + dy).back == A) ++same; }
			return tot ? same * 100 / tot : 0;
		};
		if (occ.size() >= 4)
		{
			for (int d = 1; d <= 8; ++d) if (Self(d, 0) >= 35) { P.pw = d; break; }
			for (int d = 1; d <= 8; ++d) if (Self(0, d) >= 35) { P.ph = d; break; }
			if (Self(P.pw, 0) < 35) P.pw = 1;
			if (Self(0, P.ph) < 35) P.ph = 1;
		}
		P.t.assign(P.pw * P.ph, A);
		for (int j = 0; j < P.ph; ++j)
			for (int i = 0; i < P.pw; ++i)
			{
				if (!i && !j) continue;
				std::unordered_map<int, int> votes;
				for (const auto& o : occ) if (s_map.In(o.first + i, o.second + j)) votes[(int)s_map.At(o.first + i, o.second + j).back]++;
				int best = A, bc = 0;
				for (const auto& kv : votes) if (kv.second > bc && kv.first > 0) { bc = kv.second; best = kv.first; }
				P.t[j * P.pw + i] = best;
			}
		for (int k = 0; k < (int)P.t.size(); ++k) if (!P.where.count(P.t[k])) P.where[P.t[k]] = k;
		return P;
	}
	bool SeamUsable() { return s_seamless && s_layer == L_BACK && s_map.Sprite(s_selSprite) != nullptr; }
	void SeamDab(int cx, int cy)
	{
		const PatternData& P = LearnPattern(s_selSprite);
		const int W = s_map.Width();
		const int r0 = -(s_brush - 1) / 2, r1 = s_brush / 2;
		if (!s_patPhase)
		{
			// line the block up with the same pattern already on the map next to the stroke (else start it here)
			s_patPhase = true; s_patOX = cx + r0; s_patOY = cy + r0;
			bool found = false;
			for (int rad = 1; rad <= 4 && !found; ++rad)
				for (int j = -rad; j <= rad && !found; ++j)
					for (int i = -rad; i <= rad && !found; ++i)
					{
						const int x = cx + i, y = cy + j;
						if (!s_map.In(x, y) || std::max(std::abs(i), std::abs(j)) != rad) continue;
						const auto it = P.where.find((int)s_map.At(x, y).back);
						if (it == P.where.end()) continue;
						s_patOX = x - it->second % P.pw; s_patOY = y - it->second / P.pw; found = true;
					}
		}
		for (int dy = r0; dy <= r1; ++dy)
			for (int dx = r0; dx <= r1; ++dx)
			{
				const int x = cx + dx, y = cy + dy;
				if (!s_map.In(x, y)) continue;
				const int i = ((x - s_patOX) % P.pw + P.pw) % P.pw, j = ((y - s_patOY) % P.ph + P.ph) % P.ph;
				const int t = P.t[j * P.pw + i];
				if ((int)s_map.At(x, y).back != t) { Touch(x, y); s_map.At(x, y).back = (uint32_t)t; }
			}
		(void)W;
		++s_gen;
	}

	// ------------------------------------------------------------ copy / stamp (Select: drag a box, Ctrl+C, Ctrl+V)
	struct Clip { int w = 0, h = 0; std::vector<EdTile> t; } s_clip;
	bool s_rect = false; int s_rx0 = 0, s_ry0 = 0, s_rx1 = 0, s_ry1 = 0;		// the Select tool's box
	bool s_pasteLayer[3] = { true, true, true }; bool s_pasteAttr = false, s_pasteSkipEmpty = true;
	void CopySelection()
	{
		int x0 = s_selX, y0 = s_selY, x1 = s_selX, y1 = s_selY;
		if (s_rect) { x0 = std::min(s_rx0, s_rx1); x1 = std::max(s_rx0, s_rx1); y0 = std::min(s_ry0, s_ry1); y1 = std::max(s_ry0, s_ry1); }
		if (!s_map.In(x0, y0) || !s_map.In(x1, y1)) { Status("Select a tile or drag a box with the Select tool first.", true); return; }
		s_clip.w = x1 - x0 + 1; s_clip.h = y1 - y0 + 1; s_clip.t.clear();
		for (int y = y0; y <= y1; ++y) for (int x = x0; x <= x1; ++x) s_clip.t.push_back(s_map.At(x, y));
		Status("Copied " + std::to_string(s_clip.w) + " x " + std::to_string(s_clip.h) + " tiles - Ctrl+V, then click to stamp them.");
	}
	void StampAt(int ox, int oy)
	{
		for (int j = 0; j < s_clip.h; ++j)
			for (int i = 0; i < s_clip.w; ++i)
			{
				const int x = ox + i, y = oy + j;
				if (!s_map.In(x, y)) continue;
				const EdTile& c = s_clip.t[(size_t)j * s_clip.w + i];
				EdTile t = s_map.At(x, y);
				if (s_pasteAttr) { const uint32_t b = t.back, f = t.front, r = t.roof; t = c; t.back = b; t.front = f; t.roof = r; }
				for (int l = 0; l < 3; ++l)
				{
					if (!s_pasteLayer[l]) continue;
					const uint32_t v = LayerGet(c, l);
					if (!v && s_pasteSkipEmpty) continue;
					LayerSet(t, l, v);
					if (l == L_FRONT) t.frontoffset = c.frontoffset;
					if (l == L_ROOF) { t.rooftype = c.rooftype; t.show_roof = c.show_roof; }
				}
				if (!SameTile(t, s_map.At(x, y))) { Touch(x, y); s_map.At(x, y) = t; }
			}
		++s_gen;
	}

	// ------------------------------------------------------------ minimap
	void RebuildMinimap()
	{
		s_minimapDirty = false; s_mmAt = SDL_GetTicks();
		if (!s_map.Loaded()) return;
		const int w = s_map.Width(), h = s_map.Height();
		if (!s_minimap || s_mmW != w || s_mmH != h)
		{
			if (s_minimap) SDL_DestroyTexture(s_minimap);
			s_minimap = SDL_CreateTexture(s_ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w, h);
			s_mmW = w; s_mmH = h;
		}
		if (!s_minimap) return;
		std::vector<uint32_t> px((size_t)w * h);
		for (int y = 0; y < h; ++y)
			for (int x = 0; x < w; ++x)
			{
				const EdTile& t = s_map.At(x, y);
				int idx = t.roof && !t.show_roof && s_show[L_ROOF] ? (int)t.roof : (int)t.back;
				Tex* tx = GetTex(idx);
				ImU32 c = tx && tx->avg ? tx->avg : IM_COL32(20, 14, 10, 255);
				if (t.attr_dont && s_overlay[0]) c = IM_COL32(((c & 0xFF) + 230) / 2, (((c >> 8) & 0xFF) + 40) / 2, (((c >> 16) & 0xFF) + 30) / 2, 255);
				px[(size_t)y * w + x] = c;
			}
		SDL_UpdateTexture(s_minimap, nullptr, px.data(), w * 4);
	}

	// ------------------------------------------------------------ the map view
	void MapView()
	{
		ImGuiIO& io = ImGui::GetIO();
		const ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (size.x < 50) size.x = 50;
		if (size.y < 50) size.y = 50;
		s_viewW = size.x; s_viewH = size.y;
		ImGui::InvisibleButton("##mapview", size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);
		const bool hovered = ImGui::IsItemHovered(), active = ImGui::IsItemActive();
		ImDrawList* dl = ImGui::GetWindowDrawList();
		const ImVec2 p1(p0.x + size.x, p0.y + size.y);
		dl->PushClipRect(p0, p1, true);
		dl->AddRectFilled(p0, p1, IM_COL32(12, 8, 6, 255));
		if (!s_map.Loaded())
		{
			const char* t = "No map open - File > Open (Ctrl+O) or drop a .map file here";
			const ImVec2 ts = ImGui::CalcTextSize(t);
			dl->AddText(ImVec2(p0.x + (size.x - ts.x) * 0.5f, p0.y + (size.y - ts.y) * 0.5f), Theme::GoldDim, t);
			if (Theme::Dragon) dl->AddImage(Theme::Dragon, ImVec2(p0.x + size.x * 0.5f - 64, p0.y + size.y * 0.5f - 160), ImVec2(p0.x + size.x * 0.5f + 64, p0.y + size.y * 0.5f - 32));
			dl->PopClipRect();
			return;
		}

		// ---- input: zoom (wheel, around the mouse), pan (middle drag / space + drag / keys)
		const float Z = s_zoom;
		if (hovered && io.MouseWheel != 0)
		{
			const float wx = s_camX + (io.MousePos.x - p0.x) / Z, wy = s_camY + (io.MousePos.y - p0.y) / Z;
			float nz = Z * (io.MouseWheel > 0 ? 1.25f : 0.8f);
			nz = std::max(0.5f, std::min(4.0f, nz));
			s_zoom = nz;
			s_camX = wx - (io.MousePos.x - p0.x) / nz; s_camY = wy - (io.MousePos.y - p0.y) / nz;
		}
		const bool spaceDrag = ImGui::IsKeyDown(ImGuiKey_Space) && !io.WantTextInput;
		if ((hovered || active) && (ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0) || (spaceDrag && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0))))
		{ s_camX -= io.MouseDelta.x / s_zoom; s_camY -= io.MouseDelta.y / s_zoom; }
		if (!io.WantTextInput && !io.KeyCtrl)
		{
			const float sp = 900.0f * io.DeltaTime / s_zoom * (io.KeyShift ? 3.0f : 1.0f);
			const bool arrows = !(s_tool == T_OBJECT && s_selObj >= 0);		// with an object selected the arrows nudge it
			if ((arrows && ImGui::IsKeyDown(ImGuiKey_LeftArrow)) || ImGui::IsKeyDown(ImGuiKey_A)) s_camX -= sp;
			if ((arrows && ImGui::IsKeyDown(ImGuiKey_RightArrow)) || ImGui::IsKeyDown(ImGuiKey_D)) s_camX += sp;
			if ((arrows && ImGui::IsKeyDown(ImGuiKey_UpArrow)) || ImGui::IsKeyDown(ImGuiKey_W)) s_camY -= sp;
			if ((arrows && ImGui::IsKeyDown(ImGuiKey_DownArrow)) || ImGui::IsKeyDown(ImGuiKey_S)) s_camY += sp;
		}
		const float worldW = s_map.Width() * 32.0f, worldH = s_map.Height() * 32.0f;
		s_camX = std::max(-size.x / s_zoom * 0.5f, std::min(worldW - size.x / s_zoom * 0.5f, s_camX));
		s_camY = std::max(-size.y / s_zoom * 0.5f, std::min(worldH - size.y / s_zoom * 0.5f, s_camY));
		const float z = s_zoom;
		auto SX = [&](float wx) { return p0.x + (wx - s_camX) * z; };
		auto SY = [&](float wy) { return p0.y + (wy - s_camY) * z; };

		// ---- the tile under the mouse
		s_hoverX = s_hoverY = -1;
		if (hovered)
		{
			const int hx = (int)std::floor((s_camX + (io.MousePos.x - p0.x) / z) / 32.0f), hy = (int)std::floor((s_camY + (io.MousePos.y - p0.y) / z) / 32.0f);
			if (s_map.In(hx, hy)) { s_hoverX = hx; s_hoverY = hy; }
		}

		// ---- tools
		if (!spaceDrag)
		{
			const bool lClick = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			const bool lDown = active && ImGui::IsMouseDown(ImGuiMouseButton_Left);
			const bool rClick = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right);
			const bool rDown = active && ImGui::IsMouseDown(ImGuiMouseButton_Right);
			static int lastX = -9999, lastY = -9999;
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				if (s_boxing && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) { s_boxing = false; if (s_hoverX >= 0) FillBox(s_boxX, s_boxY, s_hoverX, s_hoverY); else s_stroke.clear(); }
				if (!s_stroke.empty()) EndStroke();
				s_patPhase = false;
				lastX = lastY = -9999;
			}
			switch (s_tool)
			{
			case T_SELECT:
				if (lClick && s_hoverX >= 0) { s_selX = s_hoverX; s_selY = s_hoverY; s_rect = false; s_rx0 = s_rx1 = s_hoverX; s_ry0 = s_ry1 = s_hoverY; }
				if (lDown && !lClick && s_hoverX >= 0 && (s_hoverX != s_rx1 || s_hoverY != s_ry1)) { s_rx1 = s_hoverX; s_ry1 = s_hoverY; s_rect = s_rx0 != s_rx1 || s_ry0 != s_ry1; }
				if (rClick && s_hoverX >= 0) Pick(s_hoverX, s_hoverY);
				break;
			case T_PICK:
				if ((lClick || rClick) && s_hoverX >= 0) Pick(s_hoverX, s_hoverY);
				break;
			case T_STAMP:
				if (lClick && s_hoverX >= 0 && s_clip.w) { StampAt(s_hoverX, s_hoverY); Status("Stamped " + std::to_string(s_clip.w) + " x " + std::to_string(s_clip.h) + " tiles."); }
				if (rClick) s_tool = T_SELECT;
				break;
			case T_OBJECT:
			{
				const float mx = s_camX + (io.MousePos.x - p0.x) / z, my = s_camY + (io.MousePos.y - p0.y) / z;
				auto HitTest = [&]() -> int		// the object on top under the mouse
				{
					int best = -1; int bestKey = INT_MIN;
					for (int i = 0; i < (int)s_map.Objects.size(); ++i)
					{
						const MapObj& o = s_map.Objects[i];
						const ORect r = ObjRect(o);
						if (mx < r.x || my < r.y || mx >= r.x + r.w || my >= r.y + r.h) continue;
						const int k = o.y + o.offy;
						if (k >= bestKey) { bestKey = k; best = i; }
					}
					return best;
				};
				if (lClick && hovered)
				{
					const int hit = HitTest();
					if (hit >= 0)
					{
						s_selObj = hit; s_objDrag = true; s_dragBefore = s_map.Objects;
						s_dragDX = mx - s_map.Objects[hit].x; s_dragDY = my - s_map.Objects[hit].y;
					}
					else if (s_placeId > 0)		// place a new one where the mouse is
					{
						std::vector<MapObj> before = s_map.Objects;
						MapObj o; std::memset(&o, 0, sizeof(o));
						if (s_placeTpl.id == s_placeId) o = s_placeTpl;		// animation, light, sound, boxes ... of the object it came from
						else { o.id = (uint16_t)s_placeId; o.animationType = 1; }
						o.x = (int)mx; o.y = (int)my;
						if (io.KeyShift) { o.x = (o.x / 16) * 16; o.y = (o.y / 16) * 16; }
						s_map.Objects.push_back(o); s_map.ObjExtra.push_back(0);
						s_selObj = (int)s_map.Objects.size() - 1;
						ObjStep(std::move(before), nullptr);
						Status("Placed object #" + std::to_string(s_placeId) + " at " + std::to_string(o.x) + ", " + std::to_string(o.y) + ".");
					}
					else s_selObj = -1;
				}
				if (s_objDrag && s_selObj >= 0 && s_selObj < (int)s_map.Objects.size())
				{
					if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
					{
						MapObj& o = s_map.Objects[s_selObj];
						int nx = (int)std::lround(mx - s_dragDX), ny = (int)std::lround(my - s_dragDY);
						if (io.KeyShift) { nx = (nx / 16) * 16; ny = (ny / 16) * 16; }		// Shift: steps of 16 pixels
						if (nx != o.x || ny != o.y) { o.x = nx; o.y = ny; ++s_gen; }
					}
					else
					{
						s_objDrag = false;
						const MapObj& o = s_map.Objects[s_selObj]; const MapObj& b = s_dragBefore[s_selObj];
						if (o.x != b.x || o.y != b.y) ObjStep(std::move(s_dragBefore), nullptr), Status("Moved object #" + std::to_string(o.id) + " to " + std::to_string(o.x) + ", " + std::to_string(o.y) + ".");
						s_dragBefore.clear();
					}
				}
				if (rClick && hovered) { const int hit = HitTest(); if (hit >= 0) { s_placeId = s_map.Objects[hit].id; s_placeTpl = s_map.Objects[hit]; Status("Picked object #" + std::to_string(s_placeId) + " - click an empty spot to place copies."); } }
				break;
			}
			case T_FILL:
				if (lClick && s_hoverX >= 0) { s_boxing = true; s_boxX = s_hoverX; s_boxY = s_hoverY; }
				if (rClick && s_hoverX >= 0) Pick(s_hoverX, s_hoverY);
				break;
			case T_PAINT: case T_ERASE:
				if (rClick && s_hoverX >= 0) { Pick(s_hoverX, s_hoverY); break; }
				// fall through
			case T_ATTR:
				if ((lDown || (s_tool == T_ATTR && rDown)) && s_hoverX >= 0 && (s_hoverX != lastX || s_hoverY != lastY))
				{
					if (s_tool == T_PAINT && SeamUsable()) SeamDab(s_hoverX, s_hoverY);
					else ApplyAt(s_hoverX, s_hoverY, s_tool == T_ATTR && rDown);
					lastX = s_hoverX; lastY = s_hoverY;
				}
				break;
			}
		}

		// ---- draw: back, front, roof (the client's order and rules, map.cpp MapDisplay)
		// The tiles go into one texture with SDL directly, and only when something changed (camera, zoom,
		// layers, an edit) - not thousands of ImGui images every frame, so the UI stays quick.
		const bool peek = s_hideRoofsUnderMouse && s_hoverX >= 0;
		struct Key { float cx, cy, z; int w, h, gen, px, py; int b, f, r, o, el; };		// all 4-byte fields: no padding, so memcmp compares only what matters
		Key key; std::memset(&key, 0, sizeof(key));
		key.cx = s_camX; key.cy = s_camY; key.z = z; key.w = (int)size.x; key.h = (int)size.y; key.gen = s_gen;
		key.px = peek ? s_hoverX : -1; key.py = peek ? s_hoverY : -1; key.b = s_show[0]; key.f = s_show[1]; key.r = s_show[2]; key.o = s_showObj;
		// painting a layer: the layers drawn over it are faded, so you can see what you are painting (a back tile under a roof)
		const bool layerTool = s_tool == T_PAINT || s_tool == T_ERASE || s_tool == T_FILL || s_tool == T_PICK;
		key.el = layerTool && s_fadeAbove ? s_layer : 3;
		static Key last = {};
		if (!s_canvas || s_canvasW != key.w || s_canvasH != key.h)
		{
			if (s_canvas) SDL_DestroyTexture(s_canvas);
			s_canvas = SDL_CreateTexture(s_ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, key.w, key.h);
			s_canvasW = key.w; s_canvasH = key.h; last.gen = -1;
		}
		if (s_canvas && std::memcmp(&key, &last, sizeof(Key)) != 0)
		{
			last = key;
			SDL_Texture* prev = SDL_GetRenderTarget(s_ren);
			SDL_SetRenderTarget(s_ren, s_canvas);
			SDL_SetRenderDrawColor(s_ren, 12, 8, 6, 255);
			SDL_RenderClear(s_ren);
			const int margin = 6;			// front / roof pictures reach outside their tile
			const int tx0 = std::max(0, (int)std::floor(s_camX / 32.0f) - margin), ty0 = std::max(0, (int)std::floor(s_camY / 32.0f) - margin);
			const int tx1 = std::min(s_map.Width() - 1, (int)std::floor((s_camX + size.x / z) / 32.0f) + margin);
			const int ty1 = std::min(s_map.Height() - 1, (int)std::floor((s_camY + size.y / z) / 32.0f) + margin);
			int fadeFrom = key.el;		// layers above the one being painted are see-through
			Uint8 picAlpha = 255;
			auto DrawPic = [&](int idx, int x, int y)
			{
				Tex* t = GetTex(idx);
				const EdSprite* sp = s_map.Sprite(idx);
				if (!t || !sp) return;
				const float wx = x * 32.0f - sp->ox, wy = y * 32.0f - sp->oy;
				const SDL_FRect d = { std::floor((wx - s_camX) * z), std::floor((wy - s_camY) * z), std::ceil(t->w * z), std::ceil(t->h * z) };
				if (picAlpha != 255) SDL_SetTextureAlphaMod(t->t, picAlpha);
				SDL_RenderCopyF(s_ren, t->t, nullptr, &d);
				if (picAlpha != 255) SDL_SetTextureAlphaMod(t->t, 255);
			};
			// objects: after the ground and front tiles, under the roofs; lowest first (client: sorted by their base)
			// which objects go where (client gameproc.cpp): attr 2 (back below) under the ground tiles,
			// attr 1 (back above) on the ground under the front tiles, the rest after the front tiles
			// sorted by y like the characters, and the roofs over everything
			auto DrawObjects = [&](int pass)
			{
				const float vx0 = s_camX - 800, vy0 = s_camY - 800, vx1 = s_camX + size.x / z + 800, vy1 = s_camY + size.y / z + 800;
				std::vector<int> vis;
				for (int i = 0; i < (int)s_map.Objects.size(); ++i)
				{
					const MapObj& o = s_map.Objects[i];
					const int group = o.attr == 2 ? 0 : o.attr == 1 ? 1 : 2;
					if (group != pass) continue;
					const float ax = (float)(o.x + o.offx), ay = (float)(o.y + o.offy);
					if (ax >= vx0 && ax <= vx1 && ay >= vy0 && ay <= vy1) vis.push_back(i);
				}
				std::stable_sort(vis.begin(), vis.end(), [](int a, int b) { const MapObj& A = s_map.Objects[a]; const MapObj& B = s_map.Objects[b]; return A.y < B.y; });
				for (int i : vis)
				{
					const MapObj& o = s_map.Objects[i];
					const std::vector<EdSprite>& fr = s_map.ObjectFrames(o.id);
					if (fr.empty()) continue;
					const int mode = o.attr1 & 3, alpha = o.attr1 >> 3;
					auto Put = [&](int frame, bool fx)
					{
						if (frame >= (int)fr.size()) return;
						Tex* t = GetObjTex(o.id, frame);
						if (!t) return;
						const float wx = (float)(o.x + o.offx - fr[frame].ox), wy = (float)(o.y + o.offy - fr[frame].oy);
						const SDL_FRect d = { std::floor((wx - s_camX) * z), std::floor((wy - s_camY) * z), std::ceil(t->w * z), std::ceil(t->h * z) };
						if (fx && mode == 1) SDL_SetTextureAlphaMod(t->t, (Uint8)std::min(255, alpha * 8 + 7));		// see-through
						if (fx && mode == 2) { SDL_SetTextureBlendMode(t->t, SDL_BLENDMODE_ADD); SDL_SetTextureAlphaMod(t->t, (Uint8)std::min(255, alpha * 8 + 7)); }	// light / glow
						SDL_RenderCopyF(s_ren, t->t, nullptr, &d);
						if (fx && mode) { SDL_SetTextureAlphaMod(t->t, 255); SDL_SetTextureBlendMode(t->t, SDL_BLENDMODE_BLEND); }
					};
					if (o.animationType == 4 || o.animationType == 5) { Put(0, false); Put(fr.size() > 1 ? 1 : 0, true); }		// a base picture with an animation on it
					else Put(0, true);
					// the client (object.cpp DisplayMapObject) then puts back the front tiles that stand in front of
					// the object (frontoffset * 32 >= its y) - walls and roof edges are not cut by what is behind them
					if (s_show[L_FRONT])
					{
						const EdSprite& sp = fr[0];
						const int sx0 = o.x + o.offx - sp.ox, sy0 = o.y + o.offy - sp.oy;
						int ex = sx0 + sp.W() + 31, ey = sy0 + sp.H() + 31;
						ex += (ex - sx0) / 2;
						const int ax = std::max(0, sx0 / 32), ay = std::max(0, sy0 / 32);
						const int bx = std::min(s_map.Width(), ex / 32), by = std::min(s_map.Height(), ey / 32);
						for (int ty = ay; ty < by; ++ty)
							for (int tx = ax; tx < bx; ++tx)
							{
								const EdTile& t = s_map.At(tx, ty);
								if (t.front && (int)t.frontoffset * 32 >= o.y) { const Uint8 keep = picAlpha; picAlpha = L_FRONT > fadeFrom ? 70 : 255; DrawPic(t.front, tx, ty); picAlpha = keep; }
							}
					}
				}
			};
			for (int layer = 0; layer < 3; ++layer)
			{
				picAlpha = 255;
				if (s_showObj) DrawObjects(layer);		// 0: before the ground, 1: before the front, 2: before the roofs
				if (!s_show[layer]) continue;
				picAlpha = layer > fadeFrom ? 70 : 255;
				for (int y = ty0; y <= ty1; ++y)
					for (int x = tx0; x <= tx1; ++x)
					{
						const EdTile& t = s_map.At(x, y);
						const bool roofHides = t.roof && !t.show_roof && !t.rooftype && s_show[L_ROOF] && fadeFrom >= L_ROOF;		// (not while painting under it)
						if (layer == L_BACK && !roofHides)		// also back == 0 inside the map: the game shows the ground around it there
						{
							// a ground tile that doesn't cover its square (a missing picture, or an empty / cut-out one -
							// BARANTAN has fully see-through ground tiles under its front tiles): the game draws nothing
							// under it, so you see the ground around it there, not black. Put the nearest solid ground first.
							Tex* bt = t.back ? GetTex(t.back) : nullptr;
							bool hole = t.back != 0;
							if (!t.back)		// no ground: only a hole in the ground (ground on most sides), not the black edge of a dungeon
							{
								int around = 0;
								for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx)
									if ((dx || dy) && s_map.In(x + dx, y + dy)) { const int nb = s_map.At(x + dx, y + dy).back; Tex* nt = nb ? GetTex(nb) : nullptr; if (nt && nt->solid) ++around; }
								hole = around >= 5;
							}
							if (hole && (!bt || !bt->solid))
							{
								bool done = false;
								for (int r = 1; r <= 3 && !done; ++r)
									for (int dy = -r; dy <= r && !done; ++dy)
										for (int dx = -r; dx <= r && !done; ++dx)
										{
											if (std::max(std::abs(dx), std::abs(dy)) != r || !s_map.In(x + dx, y + dy)) continue;
											const int nb = s_map.At(x + dx, y + dy).back;
											Tex* nt = nb ? GetTex(nb) : nullptr;
											if (nt && nt->solid) { DrawPic(nb, x, y); done = true; }
										}
							}
							if (t.back) DrawPic(t.back, x, y);
						}
						else if (layer == L_FRONT) { if (t.front && !roofHides) DrawPic(t.front, x, y); }
						else if (t.roof && !t.show_roof)
						{
							if (peek && std::abs(x - s_hoverX) <= 6 && std::abs(y - s_hoverY) <= 6) continue;
							DrawPic(t.roof, x, y);
						}
					}
			}
			SDL_SetRenderTarget(s_ren, prev);
		}
		if (s_canvas) dl->AddImage((ImTextureID)(intptr_t)s_canvas, p0, ImVec2(p0.x + key.w, p0.y + key.h));

		// ---- attribute overlays, grid, hover, selection, box
		const int vx0 = std::max(0, (int)std::floor(s_camX / 32.0f)), vy0 = std::max(0, (int)std::floor(s_camY / 32.0f));
		const int vx1 = std::min(s_map.Width() - 1, (int)std::floor((s_camX + size.x / z) / 32.0f));
		const int vy1 = std::min(s_map.Height() - 1, (int)std::floor((s_camY + size.y / z) / 32.0f));
		bool anyOverlay = false; for (int a = 0; a < NUM_ATTRS; ++a) anyOverlay |= s_overlay[a];
		if (s_tool == T_ATTR) anyOverlay = true;
		if (anyOverlay)
			for (int y = vy0; y <= vy1; ++y)
				for (int x = vx0; x <= vx1; ++x)
				{
					const EdTile& t = s_map.At(x, y);
					for (int a = 0; a < NUM_ATTRS; ++a)
						if ((s_overlay[a] || (s_tool == T_ATTR && a == s_attr)) && ATTRS[a].get(t))
							dl->AddRectFilled(ImVec2(SX(x * 32.0f), SY(y * 32.0f)), ImVec2(SX(x * 32.0f + 32), SY(y * 32.0f + 32)), ATTRS[a].color);
				}
		if (s_grid && z >= 0.5f)
		{
			const ImU32 gc = IM_COL32(222, 178, 92, 50);
			for (int x = vx0; x <= vx1 + 1; ++x) dl->AddLine(ImVec2(SX(x * 32.0f), SY(vy0 * 32.0f)), ImVec2(SX(x * 32.0f), SY((vy1 + 1) * 32.0f)), gc);
			for (int y = vy0; y <= vy1 + 1; ++y) dl->AddLine(ImVec2(SX(vx0 * 32.0f), SY(y * 32.0f)), ImVec2(SX((vx1 + 1) * 32.0f), SY(y * 32.0f)), gc);
		}
		dl->AddRect(ImVec2(SX(0), SY(0)), ImVec2(SX(worldW), SY(worldH)), Theme::GoldDim, 0, 0, 2.0f);		// the map's edge
		if (s_boxing && s_hoverX >= 0)
		{
			const int ax = std::min(s_boxX, s_hoverX), ay = std::min(s_boxY, s_hoverY), bx = std::max(s_boxX, s_hoverX), by = std::max(s_boxY, s_hoverY);
			dl->AddRectFilled(ImVec2(SX(ax * 32.0f), SY(ay * 32.0f)), ImVec2(SX(bx * 32.0f + 32), SY(by * 32.0f + 32)), IM_COL32(222, 178, 92, 50));
			dl->AddRect(ImVec2(SX(ax * 32.0f), SY(ay * 32.0f)), ImVec2(SX(bx * 32.0f + 32), SY(by * 32.0f + 32)), Theme::Gold, 0, 0, 2.0f);
		}
		else if (s_hoverX >= 0 && s_tool != T_OBJECT)
		{
			const int n = (s_tool == T_PAINT || s_tool == T_ERASE || s_tool == T_ATTR) ? s_brush : 1;
			const int r0 = -(n - 1) / 2, r1 = n / 2;
			const float ax = (s_hoverX + r0) * 32.0f, ay = (s_hoverY + r0) * 32.0f, bx = (s_hoverX + r1 + 1) * 32.0f, by = (s_hoverY + r1 + 1) * 32.0f;
			if (s_tool == T_PAINT && s_show[s_layer]) { if (Tex* t = GetTex(s_selSprite)) { const EdSprite* sp = s_map.Sprite(s_selSprite); dl->AddImage(TexId(t), ImVec2(SX(s_hoverX * 32.0f - sp->ox), SY(s_hoverY * 32.0f - sp->oy)), ImVec2(SX(s_hoverX * 32.0f - sp->ox + t->w), SY(s_hoverY * 32.0f - sp->oy + t->h)), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 150)); } }
			dl->AddRect(ImVec2(SX(ax), SY(ay)), ImVec2(SX(bx), SY(by)), s_tool == T_ERASE ? IM_COL32(240, 80, 60, 255) : Theme::Gold, 0, 0, 2.0f);
		}
		if (s_showObj && s_tool == T_OBJECT)
		{
			const float mx = s_camX + (io.MousePos.x - p0.x) / z, my = s_camY + (io.MousePos.y - p0.y) / z;
			for (int i = 0; i < (int)s_map.Objects.size(); ++i)
			{
				const MapObj& o = s_map.Objects[i];
				const float ax = (float)(o.x + o.offx), ay = (float)(o.y + o.offy);
				if (ax < s_camX - 64 || ay < s_camY - 64 || ax > s_camX + size.x / z + 64 || ay > s_camY + size.y / z + 64) continue;
				const ORect r = ObjRect(o);
				const bool hov = hovered && mx >= r.x && my >= r.y && mx < r.x + r.w && my < r.y + r.h;
				const bool none = s_map.ObjectFrames(o.id).empty();
				if (i == s_selObj || hov || none)
					dl->AddRect(ImVec2(SX(r.x), SY(r.y)), ImVec2(SX(r.x + r.w), SY(r.y + r.h)), i == s_selObj ? Theme::Gold : none ? IM_COL32(230, 60, 40, 200) : IM_COL32(238, 222, 186, 120), 0, 0, i == s_selObj ? 2.0f : 1.0f);
				dl->AddCircleFilled(ImVec2(SX(ax), SY(ay)), i == s_selObj ? 4.0f : 2.5f, i == s_selObj ? Theme::Ember : IM_COL32(222, 178, 92, 160));
			}
			if (s_placeId > 0 && hovered && s_selObj < 0)		// a ghost of what a click places
			{
				const std::vector<EdSprite>& fr = s_map.ObjectFrames(s_placeId);
				if (Tex* t = fr.empty() ? nullptr : GetObjTex(s_placeId, 0))
					dl->AddImage(TexId(t), ImVec2(SX(mx - fr[0].ox), SY(my - fr[0].oy)), ImVec2(SX(mx - fr[0].ox + t->w), SY(my - fr[0].oy + t->h)), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 140));
			}
		}
		if (s_rect && s_tool == T_SELECT)
		{
			const int ax = std::min(s_rx0, s_rx1), ay = std::min(s_ry0, s_ry1), bx = std::max(s_rx0, s_rx1), by = std::max(s_ry0, s_ry1);
			dl->AddRectFilled(ImVec2(SX(ax * 32.0f), SY(ay * 32.0f)), ImVec2(SX(bx * 32.0f + 32), SY(by * 32.0f + 32)), IM_COL32(120, 180, 255, 40));
			dl->AddRect(ImVec2(SX(ax * 32.0f), SY(ay * 32.0f)), ImVec2(SX(bx * 32.0f + 32), SY(by * 32.0f + 32)), IM_COL32(140, 200, 255, 230), 0, 0, 2.0f);
		}
		if (s_tool == T_STAMP && s_clip.w && s_hoverX >= 0)
		{
			for (int l = 0; l < 3; ++l)
			{
				if (!s_pasteLayer[l]) continue;
				for (int j = 0; j < s_clip.h; ++j)
					for (int i = 0; i < s_clip.w; ++i)
					{
						const int v = (int)LayerGet(s_clip.t[(size_t)j * s_clip.w + i], l);
						Tex* t = v ? GetTex(v) : nullptr; const EdSprite* sp = v ? s_map.Sprite(v) : nullptr;
						if (!t || !sp) continue;
						const float wx = (s_hoverX + i) * 32.0f - sp->ox, wy = (s_hoverY + j) * 32.0f - sp->oy;
						dl->AddImage(TexId(t), ImVec2(SX(wx), SY(wy)), ImVec2(SX(wx + t->w), SY(wy + t->h)), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 170));
					}
			}
			dl->AddRect(ImVec2(SX(s_hoverX * 32.0f), SY(s_hoverY * 32.0f)), ImVec2(SX((s_hoverX + s_clip.w) * 32.0f), SY((s_hoverY + s_clip.h) * 32.0f)), IM_COL32(140, 200, 255, 230), 0, 0, 2.0f);
		}
		if (s_map.In(s_selX, s_selY))
		{
			const float ax = s_selX * 32.0f, ay = s_selY * 32.0f;
			dl->AddRect(ImVec2(SX(ax) - 1, SY(ay) - 1), ImVec2(SX(ax + 32) + 1, SY(ay + 32) + 1), IM_COL32(255, 240, 200, 255), 0, 0, 2.5f);
			dl->AddRect(ImVec2(SX(ax) + 2, SY(ay) + 2), ImVec2(SX(ax + 32) - 2, SY(ay + 32) - 2), Theme::Ember, 0, 0, 1.5f);
		}
		dl->PopClipRect();

		if (hovered && s_hoverX >= 0 && ImGui::IsKeyDown(ImGuiKey_LeftAlt))
		{
			const EdTile& t = s_map.At(s_hoverX, s_hoverY);
			ImGui::SetTooltip("Tile %d, %d\nBack %u  Front %u  Roof %u", s_hoverX, s_hoverY, (unsigned)t.back, (unsigned)t.front, (unsigned)t.roof);
		}
	}

	// ------------------------------------------------------------ the tile palette (left)
	void Thumb(int idx, float box, bool selected)
	{
		ImDrawList* dl = ImGui::GetWindowDrawList();
		const ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::PushID(idx);
		if (ImGui::InvisibleButton("##t", ImVec2(box, box))) s_selSprite = idx;
		const bool hov = ImGui::IsItemHovered();
		dl->AddRectFilled(p, ImVec2(p.x + box, p.y + box), IM_COL32(8, 6, 4, 255));
		if (Tex* t = GetTex(idx))
		{
			const float s = std::min(1.0f, std::min((box - 4) / t->w, (box - 4) / t->h));
			const float w = t->w * s, h = t->h * s;
			const ImVec2 a(p.x + (box - w) * 0.5f, p.y + (box - h) * 0.5f);
			dl->AddImage(TexId(t), a, ImVec2(a.x + w, a.y + h));
			if (hov) ImGui::SetTooltip("#%d  %dx%d%s\nsource %d, tile %d", idx, t->w, t->h, s_map.Sprite(idx)->raw ? " (ground tile)" : "", s_map.Sprite(idx)->sourceNo + 1, s_map.Sprite(idx)->tileNo);
		}
		else
		{
			char n[8]; std::snprintf(n, sizeof(n), "%d", idx);
			dl->AddText(ImVec2(p.x + 4, p.y + 4), Theme::GoldDim, n);
		}
		dl->AddRect(p, ImVec2(p.x + box, p.y + box), selected ? Theme::Gold : hov ? Theme::GoldDim : IM_COL32(60, 44, 26, 255), 0, 0, selected ? 2.0f : 1.0f);
		ImGui::PopID();
	}
	// the object pictures used on this map (and any id typed in)
	void ObjThumb(int id, float box, int count)
	{
		ImDrawList* dl = ImGui::GetWindowDrawList();
		const ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::PushID(id);
		if (ImGui::InvisibleButton("##o", ImVec2(box, box))) { s_placeId = id; s_tool = T_OBJECT; s_selObj = -1; }
		const bool hov = ImGui::IsItemHovered();
		dl->AddRectFilled(p, ImVec2(p.x + box, p.y + box), IM_COL32(8, 6, 4, 255));
		if (Tex* t = GetObjTex(id, 0))
		{
			const float sc = std::min(1.0f, std::min((box - 4) / t->w, (box - 4) / t->h));
			const float w = t->w * sc, h = t->h * sc;
			const ImVec2 a(p.x + (box - w) * 0.5f, p.y + (box - h) * 0.5f);
			dl->AddImage(TexId(t), a, ImVec2(a.x + w, a.y + h));
			if (hov) ImGui::SetTooltip("Object #%d  %dx%d, %d frame(s)\n%d on this map - click, then click the map to place", id, t->w, t->h, (int)s_map.ObjectFrames(id).size(), count);
		}
		else if (hov) ImGui::SetTooltip("Object #%d - no picture (object\\%d\\%04d.csp missing)", id, id / 1000 == 1 ? 0 : id / 1000, id);
		char n[12]; std::snprintf(n, sizeof(n), "%d", id);
		dl->AddText(ImVec2(p.x + 3, p.y + box - 15), Theme::GoldDim, n);
		dl->AddRect(p, ImVec2(p.x + box, p.y + box), id == s_placeId ? Theme::Gold : hov ? Theme::GoldDim : IM_COL32(60, 44, 26, 255), 0, 0, id == s_placeId ? 2.0f : 1.0f);
		ImGui::PopID();
	}
	void ObjectPalette()
	{
		ImGui::SetNextItemWidth(-1); ImGui::InputTextWithHint("##of", "object id (any, e.g. 1906)", s_objFilter, IM_ARRAYSIZE(s_objFilter), ImGuiInputTextFlags_CharsDecimal);
		const int want = s_objFilter[0] ? std::atoi(s_objFilter) : 0;
		std::map<int, int> used;
		for (const MapObj& o : s_map.Objects) ++used[o.id];
		ImGui::Text("%d objects, %d kinds", (int)s_map.Objects.size(), (int)used.size());
		if (s_placeId > 0) { ImGui::SameLine(); Theme::Dim("placing #%d", s_placeId); ImGui::SameLine(); if (ImGui::SmallButton("stop")) s_placeId = 0; }
		const float box = 64.0f;
		if (ImGui::BeginChild("##objgrid", ImVec2(0, 0), false))
		{
			std::vector<std::pair<int, int>> list;
			if (want > 0) list.push_back({ want, used.count(want) ? used[want] : 0 });
			for (auto& kv : used) if (kv.first != want && (!want || std::to_string(kv.first).find(s_objFilter) == 0)) list.push_back(kv);
			const int cols = std::max(1, (int)((ImGui::GetContentRegionAvail().x + 4) / (box + 4)));
			const int rows = ((int)list.size() + cols - 1) / cols;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
			ImGuiListClipper clip; clip.Begin(rows, box + 4);
			while (clip.Step())
				for (int r = clip.DisplayStart; r < clip.DisplayEnd; ++r)
					for (int c = 0; c < cols; ++c)
					{
						const int k = r * cols + c;
						if (k >= (int)list.size()) break;
						if (c) ImGui::SameLine();
						ObjThumb(list[k].first, box, list[k].second);
					}
			ImGui::PopStyleVar();
			if (!s_map.Loaded()) Theme::Dim("Open a map to see its objects.");
		}
		ImGui::EndChild();
	}
	// objects of another map: object pictures are shared by id (<client>\object\<n>\<id>.csp), so a click just
	// places copies of the first one of that id, with its settings (animation, light, sound, boxes)
	void SourceObjects()
	{
		std::map<int, int> used, first;
		for (int i = 0; i < (int)s_src.Objects.size(); ++i) { const int id = s_src.Objects[i].id; if (!used[id]++) first[id] = i; }
		if (s_placeId > 0) { Theme::Dim("placing #%d", s_placeId); ImGui::SameLine(); if (ImGui::SmallButton("stop")) s_placeId = 0; }
		const float box = 64.0f;
		if (ImGui::BeginChild("##srcobj", ImVec2(0, 0), false))
		{
			std::vector<int> ids; for (auto& kv : used) ids.push_back(kv.first);
			const int cols = std::max(1, (int)((ImGui::GetContentRegionAvail().x + 4) / (box + 4)));
			const int rows = ((int)ids.size() + cols - 1) / cols;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
			ImGuiListClipper clip; clip.Begin(rows, box + 4);
			while (clip.Step())
				for (int r = clip.DisplayStart; r < clip.DisplayEnd; ++r)
					for (int c = 0; c < cols; ++c)
					{
						const int k = r * cols + c;
						if (k >= (int)ids.size()) break;
						const int id = ids[k];
						if (c) ImGui::SameLine();
						ImDrawList* dl = ImGui::GetWindowDrawList();
						const ImVec2 p = ImGui::GetCursorScreenPos();
						ImGui::PushID(id);
						const bool clicked = ImGui::InvisibleButton("##so", ImVec2(box, box));
						const bool hov = ImGui::IsItemHovered();
						dl->AddRectFilled(p, ImVec2(p.x + box, p.y + box), IM_COL32(8, 6, 4, 255));
						if (Tex* t = GetObjTexOf(s_src, s_srcObjTex, id, 0))
						{
							const float sc = std::min(1.0f, std::min((box - 4) / t->w, (box - 4) / t->h));
							const float w = t->w * sc, h = t->h * sc;
							const ImVec2 a(p.x + (box - w) * 0.5f, p.y + (box - h) * 0.5f);
							dl->AddImage(TexId(t), a, ImVec2(a.x + w, a.y + h));
						}
						if (hov)
						{
							const bool here = !s_map.Loaded() || !s_map.ObjectFrames(id).empty();
							ImGui::SetTooltip("Object #%d, %d on %s\nclick, then click the map to place copies%s", id, used[id], s_src.Name().c_str(),
								here ? "" : "\n(no picture for it in this map's client folder - the game may not have it)");
						}
						char n[12]; std::snprintf(n, sizeof(n), "%d", id);
						dl->AddText(ImVec2(p.x + 3, p.y + box - 15), Theme::GoldDim, n);
						dl->AddRect(p, ImVec2(p.x + box, p.y + box), id == s_placeId ? Theme::Gold : hov ? Theme::GoldDim : IM_COL32(60, 44, 26, 255), 0, 0, id == s_placeId ? 2.0f : 1.0f);
						ImGui::PopID();
						if (clicked)
						{
							if (!s_map.Loaded()) { Status("Open the map to place on, or make one (File > New map, Ctrl+N), first.", true); continue; }
							s_placeTpl = s_src.Objects[first[id]];
							s_placeId = id; s_tool = T_OBJECT; s_selObj = -1;
							Status("Placing object #" + std::to_string(id) + " from " + s_src.Name() + " - click the map to place copies (Shift: steps of 16).");
						}
					}
			ImGui::PopStyleVar();
			if (used.empty()) Theme::Dim("%s has no objects.", s_src.Name().c_str());
		}
		ImGui::EndChild();
	}
	// tiles of another map: a click adds the picture to this map's list (if it isn't there) and paints with it
	void SourcePalette()
	{
		if (ImGui::Button("Open map to take tiles from...", ImVec2(-1, 0)))
		{
			const std::string f = FileDialog(false, s_src.Loaded() ? s_src.Path() : s_map.Folder().empty() ? std::string() : s_map.Folder() + "\\");
			if (!f.empty())
			{
				EdMap m; std::string err;
				if (!m.Load(f, err)) Status(err, true);
				else { ClearSrcTextures(); ClearSrcObjTextures(); s_src = std::move(m); Status("From " + s_src.Name() + ": " + std::to_string(s_src.SpriteCount() - 1) + " tile pictures, " + std::to_string(s_src.Objects.size()) + " objects. Click one to use it."); }
			}
		}
		if (!s_src.Loaded()) { Theme::Dim("Pick another map: its tiles and objects show here.\nClicking a tile adds it to this map's\npicture list (.sou2) and selects it to paint.\nClicking an object places copies of it."); return; }
		ImGui::Text("%s", s_src.Name().c_str()); ImGui::SameLine(); Theme::Dim("%d pictures, %d objects", s_src.SpriteCount() - 1, (int)s_src.Objects.size());
		static int s_srcView = 0;		// 0 tiles, 1 objects
		ImGui::RadioButton("Tiles##sv", &s_srcView, 0); ImGui::SameLine(); ImGui::RadioButton("Objects##sv", &s_srcView, 1);
		if (s_srcView == 1) { SourceObjects(); return; }
		ImGui::SetNextItemWidth(110); ImGui::Combo("##srclayer", &s_layer, LAYER_NAMES, 3);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("The layer the picked tile is painted on");
		if (s_map.Loaded()) { ImGui::SameLine(); Theme::Dim("%d / 3600 used here", s_map.SpriteCount()); }
		const float box = 46.0f;
		if (ImGui::BeginChild("##srcgrid", ImVec2(0, 0), false))
		{
			const int count = s_src.SpriteCount() - 1;
			const int cols = std::max(1, (int)((ImGui::GetContentRegionAvail().x + 4) / (box + 4)));
			const int rows = (count + cols - 1) / cols;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
			ImGuiListClipper clip; clip.Begin(rows, box + 4);
			while (clip.Step())
				for (int r = clip.DisplayStart; r < clip.DisplayEnd; ++r)
					for (int c = 0; c < cols; ++c)
					{
						const int idx = r * cols + c + 1;
						if (idx > count) break;
						if (c) ImGui::SameLine();
						ImDrawList* dl = ImGui::GetWindowDrawList();
						const ImVec2 p = ImGui::GetCursorScreenPos();
						ImGui::PushID(idx);
						const bool clicked = ImGui::InvisibleButton("##s", ImVec2(box, box));
						const bool hov = ImGui::IsItemHovered();
						const EdSprite* sp = s_src.Sprite(idx);
						const int here = sp && s_map.Loaded() ? s_map.FindSprite(*sp) : -1;
						dl->AddRectFilled(p, ImVec2(p.x + box, p.y + box), IM_COL32(8, 6, 4, 255));
						if (Tex* t = GetTexOf(s_src, s_srcTex, idx))
						{
							const float s = std::min(1.0f, std::min((box - 4) / t->w, (box - 4) / t->h));
							const float w = t->w * s, h = t->h * s;
							const ImVec2 a(p.x + (box - w) * 0.5f, p.y + (box - h) * 0.5f);
							dl->AddImage(TexId(t), a, ImVec2(a.x + w, a.y + h));
							if (hov) ImGui::SetTooltip("%s #%d  %dx%d\nsource %d, tile %d\n%s", s_src.Name().c_str(), idx, t->w, t->h, sp->sourceNo + 1, sp->tileNo,
								here > 0 ? ("already on this map as #" + std::to_string(here)).c_str() : "new to this map - clicking adds it");
						}
						dl->AddRect(p, ImVec2(p.x + box, p.y + box), here > 0 && here == s_selSprite ? Theme::Gold : hov ? Theme::GoldDim : IM_COL32(60, 44, 26, 255), 0, 0, here > 0 && here == s_selSprite ? 2.0f : 1.0f);
						ImGui::PopID();
						if (clicked && sp)
						{
							if (!s_map.Loaded()) { Status("Open the map to paint on, or make one (File > New map, Ctrl+N), first.", true); continue; }
							std::string err;
							const int before = s_map.SpriteCount();
							const int got = s_map.AddSprite(*sp, err);
							if (got < 0) { Status("Can't use that tile: " + err, true); continue; }
							s_selSprite = got; s_tool = T_PAINT;
							if (s_map.SpriteCount() != before) { s_dirty = true; ++s_gen; Status(s_src.Name() + " #" + std::to_string(idx) + " added to this map as #" + std::to_string(got) + " - paint with it; Save writes the .sou2 too."); }
							else Status(s_src.Name() + " #" + std::to_string(idx) + " is #" + std::to_string(got) + " on this map - paint with it.");
						}
					}
			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
	void Palette()
	{
		Theme::BeginPanel("##pal", s_leftTab == 1 ? "Objects" : s_leftTab == 2 ? "From map" : "Tiles", ImVec2(0, 0));
		if (ImGui::BeginTabBar("##lefttabs"))
		{
			if (ImGui::BeginTabItem("Tiles")) { s_leftTab = 0; ImGui::EndTabItem(); }
			if (ImGui::BeginTabItem("Objects")) { s_leftTab = 1; ImGui::EndTabItem(); }
			if (ImGui::BeginTabItem("From map")) { s_leftTab = 2; ImGui::EndTabItem(); }
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Tiles from another map");
			ImGui::EndTabBar();
		}
		if (s_leftTab == 1) { ObjectPalette(); Theme::EndPanel(); return; }
		if (s_leftTab == 2) { SourcePalette(); Theme::EndPanel(); return; }
		ImGui::SetNextItemWidth(110); ImGui::Combo("##layer", &s_layer, LAYER_NAMES, 3);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("The layer Paint / Erase / Fill / Pick work on");
		ImGui::SameLine(); ImGui::SetNextItemWidth(-1); ImGui::InputTextWithHint("##pf", "go to #", s_paletteFilter, IM_ARRAYSIZE(s_paletteFilter), ImGuiInputTextFlags_CharsDecimal);
		if (s_map.Loaded())
		{
			LayerPics();
			ImGui::Checkbox("All pictures", &s_palAll);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Off: only the pictures this map uses on the chosen layer (%d %s pictures)", (int)s_layerPics[s_layer].size(), LAYER_NAMES[s_layer]);
			ImGui::SameLine(); Theme::Dim("%d shown", s_palAll ? s_map.SpriteCount() - 1 : (int)s_layerPics[s_layer].size());
			const EdSprite* sp = s_map.Sprite(s_selSprite);
			ImGui::Text("Selected #%d", s_selSprite); ImGui::SameLine();
			Theme::Dim("%s", sp ? (sp->raw ? "ground tile" : (std::to_string(sp->xl) + "x" + std::to_string(sp->yl)).c_str()) : "-");
		}
		const float box = 46.0f;
		if (ImGui::BeginChild("##palgrid", ImVec2(0, 0), false))
		{
			const int n = s_map.SpriteCount();
			// what the list shows: every picture, or the ones the chosen layer uses
			static std::vector<int> all; all.clear();
			if (s_palAll || !s_map.Loaded()) for (int i = 1; i < n; ++i) all.push_back(i);
			const std::vector<int>& list = s_palAll || !s_map.Loaded() ? all : s_layerPics[s_layer];
			const int count = (int)list.size();
			const int cols = std::max(1, (int)((ImGui::GetContentRegionAvail().x + 4) / (box + 4)));
			const int rows = (count + cols - 1) / cols;
			static int lastFilter = -1;
			const int goTo = s_paletteFilter[0] ? std::atoi(s_paletteFilter) : -1;
			if (goTo != lastFilter && goTo > 0 && goTo < n)
			{
				s_selSprite = goTo;
				const auto it = std::lower_bound(list.begin(), list.end(), goTo);
				if (it != list.end() && *it == goTo) ImGui::SetScrollY(((int)(it - list.begin()) / cols) * (box + 4));
				else if (!s_palAll) { s_palAll = true; ImGui::SetScrollY(((goTo - 1) / cols) * (box + 4)); }		// not on this layer: show all
			}
			lastFilter = goTo;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
			ImGuiListClipper clip; clip.Begin(rows, box + 4);
			while (clip.Step())
				for (int r = clip.DisplayStart; r < clip.DisplayEnd; ++r)
					for (int c = 0; c < cols; ++c)
					{
						const int k = r * cols + c;
						if (k >= count) break;
						const int idx = list[k];
						if (c) ImGui::SameLine();
						Thumb(idx, box, idx == s_selSprite);
					}
			ImGui::PopStyleVar();
			if (!s_map.Loaded()) Theme::Dim("Open a map to see its tiles.");
			else if (!count) Theme::Dim("This map has no %s tiles yet.\nTick All pictures to pick one.", LAYER_NAMES[s_layer]);
		}
		ImGui::EndChild();
		Theme::EndPanel();
	}

	// ------------------------------------------------------------ the right column
	bool EditBits(const char* label, uint32_t v, int maxv, uint32_t& out)
	{
		int x = (int)v;
		bool ch = false;
		if (maxv == 1) { bool b = v != 0; if (ImGui::Checkbox(label, &b)) { out = b ? 1u : 0u; ch = true; } }
		else { ImGui::SetNextItemWidth(90); if (ImGui::InputInt(label, &x)) { out = (uint32_t)std::max(0, std::min(maxv, x)); ch = true; } }
		return ch;
	}
	void ObjInspector()
	{
		Theme::BeginPanel("##oinsp", "Object", ImVec2(0, 0), true);
		if (s_selObj < 0 || s_selObj >= (int)s_map.Objects.size()) { Theme::Dim("Objects tool (7): click an object to select it,\ndrag to move it (Shift = 16-pixel steps).\nPick one on the left (or right-click one on the map)\nand click an empty spot to place it."); Theme::EndPanel(); return; }
		MapObj& o = s_map.Objects[s_selObj];
		const MapObj was = o;			// for undo (the whole list is only copied when something changed)
		bool ch = false;
		ImGui::Text("Object %d of %d", s_selObj + 1, (int)s_map.Objects.size());
		ImGui::SameLine(); if (ImGui::SmallButton("Center")) { s_camX = o.x + o.offx - s_viewW / (2 * s_zoom); s_camY = o.y + o.offy - s_viewH / (2 * s_zoom); }
		if (Tex* t = GetObjTex(o.id, 0)) { const float sc = std::min(1.0f, std::min(120.0f / t->w, 90.0f / t->h)); ImGui::Image(TexId(t), ImVec2(t->w * sc, t->h * sc)); }
		auto I = [&](const char* label, int v, int lo, int hi, auto set) { int x = v; ImGui::SetNextItemWidth(110); if (ImGui::InputInt(label, &x)) { set(std::max(lo, std::min(hi, x))); ch = true; } };
		I("Picture id", o.id, 1, 65535, [&](int v) { o.id = (uint16_t)v; });
		I("X", o.x, -100000, 1000000, [&](int v) { o.x = v; });
		I("Y", o.y, -100000, 1000000, [&](int v) { o.y = v; });
		I("Offset X", o.offx, -10000, 10000, [&](int v) { o.offx = v; });
		I("Offset Y", o.offy, -10000, 10000, [&](int v) { o.offy = v; });
		static const char* anis[] = { "0 ?", "1 still", "2 back and forth", "3 loop", "4 base + back and forth", "5 base + loop", "6 toggle (door)" };
		int an = std::min(6, (int)o.animationType); ImGui::SetNextItemWidth(170);
		if (ImGui::Combo("Animation", &an, anis, 7)) { o.animationType = (uint8_t)an; ch = true; }
		static const char* modes[] = { "normal", "see-through", "glow (light)" };
		int md = o.attr1 & 3; if (md > 2) md = 0; ImGui::SetNextItemWidth(130);
		if (ImGui::Combo("Draw", &md, modes, 3)) { o.attr1 = (uint8_t)((o.attr1 & ~3) | md); ch = true; }
		if (md) { int al = o.attr1 >> 3; ImGui::SetNextItemWidth(130); if (ImGui::SliderInt("Strength", &al, 0, 31)) { o.attr1 = (uint8_t)((o.attr1 & 7) | (al << 3)); ch = true; } }
		bool sorted = o.attr2 != 0; if (ImGui::Checkbox("Sorted with characters (attr2)", &sorted)) { o.attr2 = sorted ? 1 : 0; ch = true; }
		if (ImGui::TreeNode("More"))
		{
			I("Item type", o.objectoritem, 0, 65535, [&](int v) { o.objectoritem = (uint16_t)v; });
			I("Attr", o.attr, 0, 255, [&](int v) { o.attr = (uint8_t)v; });
			I("Dir", o.dir, 0, 65535, [&](int v) { o.dir = (uint16_t)v; });
			I("Light", o.light, 0, 255, [&](int v) { o.light = (uint8_t)v; });
			I("Light X", o.lightx, -32768, 32767, [&](int v) { o.lightx = (int16_t)v; });
			I("Light Y", o.lighty, -32768, 32767, [&](int v) { o.lighty = (int16_t)v; });
			I("Delay", o.delay, -128, 127, [&](int v) { o.delay = (int8_t)v; });
			I("Sound no", o.soundno, 0, 65535, [&](int v) { o.soundno = (uint16_t)v; });
			I("Sound frame", o.soundframe, 0, 65535, [&](int v) { o.soundframe = (uint16_t)v; });
			I("Sound delay", o.sounddelay, 0, 65535, [&](int v) { o.sounddelay = (uint16_t)v; });
			ImGui::TreePop();
		}
		if (ch) { std::vector<MapObj> b = s_map.Objects; b[s_selObj] = was; ObjStep(std::move(b), nullptr); }
		if (ImGui::Button("Duplicate (Ctrl+D)", ImVec2(150, 0)))
		{
			std::vector<MapObj> b = s_map.Objects; MapObj c = o; c.x += 16; c.y += 16;
			s_map.Objects.push_back(c); s_map.ObjExtra.push_back(0); s_selObj = (int)s_map.Objects.size() - 1; ObjStep(std::move(b), "Duplicated the object.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete (Del)", ImVec2(120, 0)))
		{
			std::vector<MapObj> b = s_map.Objects;
			s_map.Objects.erase(s_map.Objects.begin() + s_selObj);
			if (s_selObj < (int)s_map.ObjExtra.size()) s_map.ObjExtra.erase(s_map.ObjExtra.begin() + s_selObj);
			s_selObj = -1; ObjStep(std::move(b), "Deleted the object.");
		}
		Theme::EndPanel();
	}
	void Inspector()
	{
		if (s_tool == T_OBJECT) { ObjInspector(); return; }
		Theme::BeginPanel("##insp", "Tile", ImVec2(0, 0), true);
		if (!s_map.In(s_selX, s_selY)) { Theme::Dim("Select tool (1): click a tile to see and edit it."); Theme::EndPanel(); return; }
		EdTile& t = s_map.At(s_selX, s_selY);
		ImGui::Text("Tile %d, %d", s_selX, s_selY);
		ImGui::SameLine(); if (ImGui::SmallButton("Center")) CenterOn(s_selX, s_selY, s_viewW, s_viewH);
		EdTile before = t, edit = t;
		bool changed = false;
		for (int l = 0; l < 3; ++l)
		{
			int v = (int)LayerGet(edit, l);
			ImGui::PushID(l);
			if (Tex* tx = GetTex(v)) { ImGui::Image(TexId(tx), ImVec2(24, 24)); ImGui::SameLine(); }
			else { ImGui::Dummy(ImVec2(24, 24)); ImGui::SameLine(); }
			ImGui::SetNextItemWidth(100);
			if (ImGui::InputInt(LAYER_NAMES[l], &v)) { LayerSet(edit, l, (uint32_t)std::max(0, std::min(4095, v))); changed = true; }
			ImGui::SameLine(); if (ImGui::SmallButton("Use")) { s_selSprite = v ? v : s_selSprite; s_layer = l; }
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pick this picture for painting");
			ImGui::PopID();
		}
		int fo = (int)edit.frontoffset; ImGui::SetNextItemWidth(100);
		if (ImGui::InputInt("Front offset", &fo)) { edit.frontoffset = (uint32_t)std::max(0, std::min(1023, fo)); changed = true; }
		Theme::SectionHeader("Attributes");
		for (int a = 0; a < NUM_ATTRS; ++a)
		{
			uint32_t nv = 0;
			ImGui::PushID(a);
			ImGui::ColorButton("##c", ImGui::ColorConvertU32ToFloat4(ATTRS[a].color | IM_COL32(0, 0, 0, 255)), ImGuiColorEditFlags_NoTooltip, ImVec2(12, 12)); ImGui::SameLine();
			if (EditBits(ATTRS[a].name, ATTRS[a].get(edit), ATTRS[a].maxv, nv)) { ATTRS[a].set(edit, nv); changed = true; }
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", ATTRS[a].tip);
			ImGui::PopID();
			if (a % 2 == 0 && a + 1 < NUM_ATTRS && ATTRS[a].maxv == 1 && ATTRS[a + 1].maxv == 1) ImGui::SameLine(150);
		}
		if (changed && !SameTile(before, edit))
		{
			t = edit;
			Step st; st.tiles.push_back({ s_selY * s_map.Width() + s_selX, before, edit }); PushStep(std::move(st));
			s_dirty = true; s_minimapDirty = true; ++s_gen;
		}
		Theme::EndPanel();
	}
	void ToolOptions()
	{
		Theme::BeginPanel("##opts", "Tool", ImVec2(0, 0), true);
		static const char* names[] = { "Select", "Paint", "Erase", "Fill", "Attribute", "Pick", "Objects", "Stamp" };
		ImGui::TextColored(ImVec4(0.87f, 0.70f, 0.36f, 1), "%s", names[s_tool]);
		if (s_tool == T_PAINT || s_tool == T_ERASE || s_tool == T_ATTR)
		{
			ImGui::SetNextItemWidth(120); ImGui::SliderInt("Brush", &s_brush, 1, 15);
			Theme::Dim("[ and ] change the brush size.");
		}
		if (s_tool == T_PAINT || s_tool == T_ERASE || s_tool == T_FILL || s_tool == T_PICK) { ImGui::SetNextItemWidth(120); ImGui::Combo("Layer", &s_layer, LAYER_NAMES, 3); }
		if (s_tool == T_ATTR)
		{
			static const char* an[NUM_ATTRS];
			for (int a = 0; a < NUM_ATTRS; ++a) an[a] = ATTRS[a].name;
			ImGui::SetNextItemWidth(140); ImGui::Combo("Attribute", &s_attr, an, NUM_ATTRS);
			if (ATTRS[s_attr].maxv > 1) { ImGui::SetNextItemWidth(120); ImGui::SliderInt("Value", &s_attrValue, 1, ATTRS[s_attr].maxv); }
			else s_attrValue = 1;
			Theme::Dim("Left button sets it, right button clears it.");
		}
		if (s_tool == T_PAINT)
		{
			ImGui::Checkbox("Pattern paint", &s_seamless);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Back layer: the picked tile stands for the block of tiles it is cut from\n(cobbles 3x3, a road 2x2, ...). Painting lays the block in order,\nlined up with the same pattern next to it.");
			if (s_seamless && s_layer == L_BACK && s_pat.forTile == s_selSprite) Theme::Dim("Pattern block: %d x %d tiles", s_pat.pw, s_pat.ph);
			if (s_seamless && s_layer != L_BACK) Theme::Dim("Pattern paint works on the Back layer.");
			Theme::Dim("Right-click picks the tile under the mouse.");
		}
		if (s_tool == T_FILL) Theme::Dim("Drag a box: it fills with the selected tile.");
		if (s_tool == T_SELECT) Theme::Dim("Click a tile; edit it under Tile.\nDrag a box, Ctrl+C to copy it, Ctrl+V to stamp it.");
		if (s_tool == T_STAMP)
		{
			if (!s_clip.w) Theme::Dim("Nothing copied yet: Select tool,\ndrag a box, Ctrl+C.");
			else Theme::Dim("Copied %d x %d tiles. Click to stamp,\nright-click / Esc to stop.", s_clip.w, s_clip.h);
			ImGui::Checkbox("Back##pl", &s_pasteLayer[0]); ImGui::SameLine(); ImGui::Checkbox("Front##pl", &s_pasteLayer[1]); ImGui::SameLine(); ImGui::Checkbox("Roof##pl", &s_pasteLayer[2]);
			ImGui::Checkbox("Attributes too", &s_pasteAttr);
			ImGui::Checkbox("Keep what is under empty parts", &s_pasteSkipEmpty);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Where the copied tile has no front / roof picture, keep the map's");
		}
		if (s_tool == T_OBJECT)
		{
			ImGui::SetNextItemWidth(110); ImGui::InputInt("Place id", &s_placeId); if (s_placeId < 0) s_placeId = 0;
			Theme::Dim("0 = only select / move. Del deletes, Ctrl+D copies,\narrows nudge the selected one (Shift = 8 px).");
		}
		Theme::EndPanel();
	}
	void ViewOptions()
	{
		Theme::BeginPanel("##viewopt", "View", ImVec2(0, 0), true);
		ImGui::Checkbox("Back", &s_show[L_BACK]); ImGui::SameLine(); ImGui::Checkbox("Front", &s_show[L_FRONT]); ImGui::SameLine(); ImGui::Checkbox("Objects", &s_showObj); ImGui::SameLine(); ImGui::Checkbox("Roof", &s_show[L_ROOF]);
		ImGui::Checkbox("Grid (G)", &s_grid); ImGui::SameLine(); ImGui::Checkbox("Open roofs at mouse", &s_hideRoofsUnderMouse); ImGui::SameLine(); ImGui::Checkbox("Fade layers above", &s_fadeAbove); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Paint / Erase / Fill / Pick: the layers over the one you work on are see-through");
		ImGui::SetNextItemWidth(140); ImGui::SliderFloat("Zoom", &s_zoom, 0.5f, 4.0f, "%.2fx");
		if (ImGui::TreeNode("Show attributes"))
		{
			for (int a = 0; a < NUM_ATTRS; ++a)
			{
				ImGui::PushID(a);
				ImGui::ColorButton("##c", ImGui::ColorConvertU32ToFloat4(ATTRS[a].color | IM_COL32(0, 0, 0, 255)), ImGuiColorEditFlags_NoTooltip, ImVec2(12, 12)); ImGui::SameLine();
				if (ImGui::Checkbox(ATTRS[a].name, &s_overlay[a]) && a == 0) s_minimapDirty = true; ++s_gen;
				ImGui::PopID();
				if (a % 2 == 0) ImGui::SameLine(150);
			}
			ImGui::NewLine();
			ImGui::TreePop();
		}
		Theme::EndPanel();
	}
	void Minimap()
	{
		Theme::BeginPanel("##mm", "Map", ImVec2(0, 0), true);
		if (!s_map.Loaded()) { Theme::Dim("-"); Theme::EndPanel(); return; }
		if (s_minimapDirty && SDL_GetTicks() - s_mmAt > 400) RebuildMinimap();
		const float avail = ImGui::GetContentRegionAvail().x;
		const float sc = std::min(avail / s_map.Width(), 220.0f / s_map.Height());
		const ImVec2 sz(s_map.Width() * sc, s_map.Height() * sc);
		const ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::InvisibleButton("##mmb", sz);
		ImDrawList* dl = ImGui::GetWindowDrawList();
		if (s_minimap) dl->AddImage((ImTextureID)(intptr_t)s_minimap, p, ImVec2(p.x + sz.x, p.y + sz.y));
		const float vx = s_camX / 32.0f * sc, vy = s_camY / 32.0f * sc, vw = s_viewW / s_zoom / 32.0f * sc, vh = s_viewH / s_zoom / 32.0f * sc;
		dl->AddRect(ImVec2(p.x + vx, p.y + vy), ImVec2(p.x + vx + vw, p.y + vy + vh), Theme::Gold, 0, 0, 1.5f);
		dl->AddRect(p, ImVec2(p.x + sz.x, p.y + sz.y), Theme::GoldDim);
		if (ImGui::IsItemActive())
		{
			const ImVec2 m = ImGui::GetIO().MousePos;
			CenterOn((int)((m.x - p.x) / sc), (int)((m.y - p.y) / sc), s_viewW, s_viewH);
		}
		Theme::Dim("%d x %d tiles  -  click to jump", s_map.Width(), s_map.Height());
		Theme::EndPanel();
	}

	// ------------------------------------------------------------ menus, toolbar, shortcuts, dialogs
	void MenuBar()
	{
		if (!ImGui::BeginMenuBar()) return;
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New map...", "Ctrl+N")) Request(Pending::New);
			if (ImGui::MenuItem("Open...", "Ctrl+O")) Request(Pending::Open);
			if (ImGui::BeginMenu("Open recent", !s_recent.empty()))
			{
				for (const std::string& r : s_recent) if (ImGui::MenuItem(r.c_str())) { Request(Pending::OpenPath, r); break; }
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save", "Ctrl+S", false, s_map.Loaded())) DoSave(false);
			if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S", false, s_map.Loaded())) DoSave(true);
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) { s_quitAsked = true; Request(Pending::Quit); }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z", false, !s_undo.empty())) Undo();
			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, !s_redo.empty())) Redo();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Back layer", nullptr, &s_show[L_BACK]);
			ImGui::MenuItem("Front layer", nullptr, &s_show[L_FRONT]);
			ImGui::MenuItem("Objects", nullptr, &s_showObj);
			ImGui::MenuItem("Roof layer", nullptr, &s_show[L_ROOF]);
			ImGui::MenuItem("Grid", "G", &s_grid);
			ImGui::Separator();
			if (ImGui::MenuItem("Zoom 100%", "0")) s_zoom = 1.0f;
			if (ImGui::MenuItem("Center the map", "Home") && s_map.Loaded()) CenterOn(s_map.Width() / 2, s_map.Height() / 2, s_viewW, s_viewH);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::TextUnformatted("Tools: 1 Select  2 Paint  3 Erase  4 Fill  5 Attribute  6 Pick  7 Objects");
			ImGui::TextUnformatted("Objects: click select, drag move (Shift 16 px), Del delete, Ctrl+D copy, arrows nudge");
			ImGui::TextUnformatted("Right-click: pick the tile under the mouse (Attribute: clear)");
			ImGui::TextUnformatted("Pan: middle mouse drag, Space + drag, arrows / WASD (Shift = faster)");
			ImGui::TextUnformatted("Zoom: mouse wheel, + / -, 0 = 100%");
			ImGui::TextUnformatted("[ ] brush size   G grid   Alt = tile info under the mouse");
			ImGui::TextUnformatted("Ctrl+Z undo  Ctrl+Y redo  Ctrl+S save  Ctrl+O open");
			ImGui::EndMenu();
		}
		const std::string info = s_map.Loaded() ? (s_map.Name() + (s_dirty || s_objDirty ? " *" : "") + "   " + std::to_string(s_map.Width()) + " x " + std::to_string(s_map.Height())) : std::string("no map");
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(info.c_str()).x - 20);
		ImGui::TextColored(ImVec4(0.87f, 0.70f, 0.36f, 1), "%s", info.c_str());
		ImGui::EndMenuBar();
	}
	void Toolbar()
	{
		struct B { int tool; const char* id; const char* label; const char* glyph; };
		static const B b[] = { { T_SELECT, "sel", "Select", "1" }, { T_PAINT, "pnt", "Paint", "2" }, { T_ERASE, "era", "Erase", "3" },
			{ T_FILL, "fil", "Fill", "4" }, { T_ATTR, "att", "Attribute", "5" }, { T_PICK, "pck", "Pick", "6" }, { T_OBJECT, "obj", "Objects", "7" }, { T_STAMP, "stp", "Stamp", "8" } };
		for (const B& x : b)
		{
			if (Theme::ToolbarButton(x.id, x.label, x.glyph, s_tool == x.tool, 96.0f)) s_tool = x.tool;
			ImGui::SameLine();
		}
		ImGui::SameLine(0, 24);
		ImGui::BeginGroup();
		ImGui::BeginDisabled(s_undo.empty()); if (ImGui::Button("Undo", ImVec2(80, 0))) Undo(); ImGui::EndDisabled();
		ImGui::SameLine(); ImGui::BeginDisabled(s_redo.empty()); if (ImGui::Button("Redo", ImVec2(80, 0))) Redo(); ImGui::EndDisabled();
		ImGui::BeginDisabled(!s_map.Loaded()); if (ImGui::Button(s_dirty || s_objDirty ? "Save *" : "Save", ImVec2(168, 0))) DoSave(false); ImGui::EndDisabled();
		ImGui::EndGroup();
		ImGui::SameLine(0, 24);
		ImGui::BeginGroup();
		ImGui::Text("Layer"); ImGui::SameLine(); ImGui::SetNextItemWidth(100); ImGui::Combo("##tlayer", &s_layer, LAYER_NAMES, 3);
		ImGui::Text("Brush"); ImGui::SameLine(); ImGui::SetNextItemWidth(100); ImGui::SliderInt("##tbrush", &s_brush, 1, 15);
		ImGui::EndGroup();
		Theme::FrameOrnament(ImGui::GetWindowPos().x + 8, ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 8, ImGui::GetCursorScreenPos().y + 2);
		ImGui::Dummy(ImVec2(0, 8));
	}
	void Shortcuts()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantTextInput) return;
		if (io.KeyCtrl)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) Undo();
			if (ImGui::IsKeyPressed(ImGuiKey_Y, false)) Redo();
			if (ImGui::IsKeyPressed(ImGuiKey_C, false) && s_tool == T_SELECT) CopySelection();
			if (ImGui::IsKeyPressed(ImGuiKey_V, false)) { if (s_clip.w) s_tool = T_STAMP; else Status("Nothing copied yet: Select tool, drag a box, Ctrl+C.", true); }
			if (ImGui::IsKeyPressed(ImGuiKey_S, false)) DoSave(io.KeyShift);
			if (ImGui::IsKeyPressed(ImGuiKey_O, false)) Request(Pending::Open);
			if (ImGui::IsKeyPressed(ImGuiKey_N, false)) Request(Pending::New);
			if (ImGui::IsKeyPressed(ImGuiKey_D, false) && s_tool == T_OBJECT && s_selObj >= 0 && s_selObj < (int)s_map.Objects.size())
			{
				std::vector<MapObj> b = s_map.Objects; MapObj c = s_map.Objects[s_selObj]; c.x += 16; c.y += 16;
				s_map.Objects.push_back(c); s_map.ObjExtra.push_back(0); s_selObj = (int)s_map.Objects.size() - 1; ObjStep(std::move(b), "Duplicated the object.");
			}
			return;
		}
		static const ImGuiKey keys[8] = { ImGuiKey_1, ImGuiKey_2, ImGuiKey_3, ImGuiKey_4, ImGuiKey_5, ImGuiKey_6, ImGuiKey_7, ImGuiKey_8 };
		for (int i = 0; i < 8; ++i) if (ImGui::IsKeyPressed(keys[i], false)) s_tool = i;
		if (s_tool == T_OBJECT && s_selObj >= 0 && s_selObj < (int)s_map.Objects.size())
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
			{
				std::vector<MapObj> b = s_map.Objects;
				s_map.Objects.erase(s_map.Objects.begin() + s_selObj);
				if (s_selObj < (int)s_map.ObjExtra.size()) s_map.ObjExtra.erase(s_map.ObjExtra.begin() + s_selObj);
				s_selObj = -1; ObjStep(std::move(b), "Deleted the object.");
			}
			else
			{
				const int st = io.KeyShift ? 8 : 1; int dx = 0, dy = 0;
				if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) dx = -st; if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) dx = st;
				if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) dy = -st; if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) dy = st;
				if (dx || dy) { std::vector<MapObj> b = s_map.Objects; s_map.Objects[s_selObj].x += dx; s_map.Objects[s_selObj].y += dy; ObjStep(std::move(b), nullptr); }
			}
		}
		if (ImGui::IsKeyPressed(ImGuiKey_G, false)) s_grid = !s_grid;
		if (ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) s_brush = std::max(1, s_brush - 1);
		if (ImGui::IsKeyPressed(ImGuiKey_RightBracket)) s_brush = std::min(15, s_brush + 1);
		if (ImGui::IsKeyPressed(ImGuiKey_0, false)) s_zoom = 1.0f;
		if (ImGui::IsKeyPressed(ImGuiKey_Equal) || ImGui::IsKeyPressed(ImGuiKey_KeypadAdd)) s_zoom = std::min(4.0f, s_zoom * 1.25f);
		if (ImGui::IsKeyPressed(ImGuiKey_Minus) || ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract)) s_zoom = std::max(0.5f, s_zoom * 0.8f);
		if (ImGui::IsKeyPressed(ImGuiKey_Home, false) && s_map.Loaded()) CenterOn(s_map.Width() / 2, s_map.Height() / 2, s_viewW, s_viewH);
		if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) { if (s_tool == T_STAMP) s_tool = T_SELECT; s_rect = false; s_boxing = false; s_stroke.clear(); s_selX = s_selY = -1; s_selObj = -1; }
	}
	void Dialogs()
	{
		if (s_openUnsaved) { s_openUnsaved = false; ImGui::OpenPopup("Unsaved changes"); }
		if (ImGui::BeginPopupModal("Unsaved changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s has changes that are not saved.", s_map.Name().c_str());
			if (ImGui::Button("Save", ImVec2(110, 0))) { if (DoSave(false)) { ImGui::CloseCurrentPopup(); RunPending(); } }
			ImGui::SameLine();
			if (ImGui::Button("Don't save", ImVec2(110, 0))) { ImGui::CloseCurrentPopup(); RunPending(); }
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(110, 0))) { s_pending = Pending::None; s_quitAsked = false; ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		static bool s_newKeepPics = true;
		if (s_askNew) { s_askNew = false; s_newW = s_map.Loaded() ? s_map.Width() : 200; s_newH = s_map.Loaded() ? s_map.Height() : 200; s_newKeepPics = s_map.SpriteCount() > 1; ImGui::OpenPopup("New map"); }
		if (ImGui::BeginPopupModal("New map", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			const bool havePics = s_map.SpriteCount() > 1;
			ImGui::BeginDisabled(!havePics);
			ImGui::Checkbox(havePics ? ("Use the tile pictures of " + s_map.Name()).c_str() : "Use the tile pictures of the open map", &s_newKeepPics);
			ImGui::EndDisabled();
			const bool keep = havePics && s_newKeepPics;
			if (!keep) Theme::Dim("Starts with no tile pictures: take them from other maps\nin the \"From map\" tab, then paint (Fill for the ground).");
			ImGui::SetNextItemWidth(120); ImGui::InputInt("Width (tiles)", &s_newW); s_newW = std::max(20, std::min(720, s_newW));
			ImGui::SetNextItemWidth(120); ImGui::InputInt("Height (tiles)", &s_newH); s_newH = std::max(20, std::min(720, s_newH));
			if (keep) ImGui::Text("Ground: tile #%d (pick it in the tile list first)", s_selSprite);
			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				EdTile fill = {}; fill.back = keep ? (uint32_t)s_selSprite & 0xFFF : 0;
				if (!keep) ClearTextures();
				s_map.NewMap(s_newW, s_newH, fill, keep); SeamReset();
				if (!keep) { s_selSprite = 1; s_leftTab = 2; }
				s_undo.clear(); s_redo.clear(); s_dirty = true; s_minimapDirty = true; ++s_gen; s_selX = s_selY = -1; s_selObj = -1; s_objDirty = true;
				CenterOn(s_newW / 2, s_newH / 2, s_viewW, s_viewH);
				Status("New " + std::to_string(s_newW) + " x " + std::to_string(s_newH) + " map - " + (keep ? "Save as... to give it a name (its .sou2 is copied with it)." : "take tiles from another map in the From map tab, then Save as... to name it (a new .sou2 is written)."));
				if (s_window) SDL_SetWindowTitle(s_window, "Dragon Raja Map Editor - untitled");
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine(); if (ImGui::Button("Cancel", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}
	void StatusBar()
	{
		ImGui::Separator();
		if (s_hoverX >= 0)
		{
			const EdTile& t = s_map.At(s_hoverX, s_hoverY);
			ImGui::Text("%d, %d", s_hoverX, s_hoverY); ImGui::SameLine();
			Theme::Dim("back %u  front %u  roof %u%s%s", (unsigned)t.back, (unsigned)t.front, (unsigned)t.roof, t.attr_dont ? "  blocked" : "", t.attr_map ? "  event" : "");
			ImGui::SameLine(0, 30);
		}
		if (s_statusBad) Theme::Bad("%s", s_status.c_str()); else Theme::Dim("%s", s_status.c_str());
		ImGui::SameLine(); ImGui::SetCursorPosX(std::max(ImGui::GetCursorPosX(), ImGui::GetWindowWidth() - 170));
		Theme::Dim("%.0f%%   %.0f fps", s_zoom * 100, ImGui::GetIO().Framerate);
	}
}

namespace Editor
{
	bool Init(SDL_Window* window, SDL_Renderer* renderer) { s_window = window; s_ren = renderer; LoadIni(); return true; }
	void Shutdown() { ClearTextures(); ClearSrcTextures(); ClearSrcObjTextures(); if (s_minimap) SDL_DestroyTexture(s_minimap); s_minimap = nullptr; if (s_canvas) SDL_DestroyTexture(s_canvas); s_canvas = nullptr; }
	bool OpenMap(const std::string& path) { return DoOpen(path); }
	bool WantsQuit() { return s_quitOk; }

	void HandleEvent(const SDL_Event& e)
	{
		if (e.type == SDL_QUIT || (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)) { s_quitAsked = true; s_pending = Pending::Quit; s_pendingPath.clear(); }
		if (e.type == SDL_RENDER_TARGETS_RESET || e.type == SDL_RENDER_DEVICE_RESET) ++s_gen;		// the GPU dropped the map picture: draw it again
		if (e.type == SDL_DROPFILE && e.drop.file) { s_pending = Pending::OpenPath; s_pendingPath = e.drop.file; SDL_free(e.drop.file); }
	}

	void Frame()
	{
		const ImGuiViewport* vp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(vp->WorkPos);
		ImGui::SetNextWindowSize(vp->WorkSize);
		ImGui::Begin("##editor", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		MenuBar();
		// a quit / drop asked from outside ImGui (window close, file drop): ask about unsaved work here
		if (s_pending == Pending::Quit && !ImGui::IsPopupOpen("Unsaved changes")) { if (s_dirty || s_objDirty) ImGui::OpenPopup("Unsaved changes"); else s_quitOk = true; }
		if (s_pending == Pending::OpenPath && !ImGui::IsPopupOpen("Unsaved changes")) { const std::string p = s_pendingPath; Request(Pending::OpenPath, p); }
		Toolbar();
		Shortcuts();
		const float statusH = ImGui::GetFrameHeightWithSpacing() + 6;
		const float leftW = 290, rightW = 320;
		ImGui::BeginChild("##left", ImVec2(leftW, -statusH), false);
		Palette();
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild("##center", ImVec2(-rightW - 8, -statusH), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		MapView();
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild("##right", ImVec2(0, -statusH), false);
		ToolOptions();
		Inspector();
		ViewOptions();
		Minimap();
		ImGui::EndChild();
		StatusBar();
		Dialogs();
		ImGui::End();
	}
}
