#define _CRT_SECURE_NO_WARNINGS
#include "Theme.h"
#include <algorithm>
#include <cfloat>
#include <cstdarg>
#include <cstdio>
#include <Windows.h>

namespace Theme
{
	ImFont* Body = nullptr;
	ImFont* Title = nullptr;
	ImFont* Heading = nullptr;
	ImTextureID Dragon = (ImTextureID)0;

	const ImU32 Gold = IM_COL32(222, 178, 92, 255);
	const ImU32 GoldDim = IM_COL32(140, 104, 48, 255);
	const ImU32 Parchment = IM_COL32(238, 222, 186, 255);
	const ImU32 Blood = IM_COL32(110, 22, 14, 255);
	const ImU32 Leather = IM_COL32(52, 34, 22, 255);
	const ImU32 LeatherDark = IM_COL32(24, 16, 11, 255);
	const ImU32 Ember = IM_COL32(196, 92, 34, 255);

	static ImFont* TryFont(ImGuiIO& io, const char* const* files, float size)
	{
		char win[MAX_PATH] = { 0 };
		GetWindowsDirectoryA(win, MAX_PATH);
		for (int i = 0; files[i]; ++i)
		{
			char path[MAX_PATH];
			std::snprintf(path, sizeof(path), "%s\\Fonts\\%s", win, files[i]);
			if (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES)
				if (ImFont* f = io.Fonts->AddFontFromFileTTF(path, size)) return f;
		}
		return nullptr;
	}

	void Apply()
	{
		ImGuiIO& io = ImGui::GetIO();
		static const char* bodyFiles[] = { "georgia.ttf", "times.ttf", "segoeui.ttf", nullptr };
		static const char* titleFiles[] = { "MTCORSVA.TTF", "georgiai.ttf", "timesi.ttf", nullptr };
		static const char* headFiles[] = { "georgiab.ttf", "timesbd.ttf", "segoeuib.ttf", nullptr };
		Body = TryFont(io, bodyFiles, 16.0f);
		if (!Body) Body = io.Fonts->AddFontDefault();
		Title = TryFont(io, titleFiles, 40.0f);
		Heading = TryFont(io, headFiles, 18.0f);
		if (!Title) Title = Body;
		if (!Heading) Heading = Body;
		io.FontDefault = Body;

		ImGuiStyle& s = ImGui::GetStyle();
		s.WindowRounding = 3.0f; s.ChildRounding = 3.0f; s.FrameRounding = 2.0f; s.PopupRounding = 3.0f;
		s.ScrollbarRounding = 2.0f; s.GrabRounding = 2.0f; s.TabRounding = 2.0f;
		s.WindowBorderSize = 1.0f; s.ChildBorderSize = 1.0f; s.FrameBorderSize = 1.0f; s.PopupBorderSize = 1.0f;
		s.WindowPadding = ImVec2(10, 10); s.FramePadding = ImVec2(7, 4); s.ItemSpacing = ImVec2(8, 6);
		s.ScrollbarSize = 14.0f; s.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		ImVec4* c = s.Colors;
		const ImVec4 parchment(0.93f, 0.87f, 0.73f, 1.0f), gold(0.87f, 0.70f, 0.36f, 1.0f), goldDim(0.55f, 0.41f, 0.19f, 1.0f);
		c[ImGuiCol_Text] = parchment;
		c[ImGuiCol_TextDisabled] = ImVec4(0.58f, 0.50f, 0.38f, 1.0f);
		c[ImGuiCol_WindowBg] = ImVec4(0.105f, 0.072f, 0.050f, 1.0f);
		c[ImGuiCol_ChildBg] = ImVec4(0.135f, 0.093f, 0.064f, 1.0f);
		c[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.08f, 0.055f, 0.98f);
		c[ImGuiCol_Border] = goldDim;
		c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
		c[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.042f, 0.03f, 1.0f);
		c[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.13f, 0.07f, 1.0f);
		c[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.18f, 0.09f, 1.0f);
		c[ImGuiCol_TitleBg] = ImVec4(0.22f, 0.07f, 0.045f, 1.0f);
		c[ImGuiCol_TitleBgActive] = ImVec4(0.40f, 0.11f, 0.06f, 1.0f);
		c[ImGuiCol_TitleBgCollapsed] = ImVec4(0.18f, 0.06f, 0.04f, 1.0f);
		c[ImGuiCol_MenuBarBg] = ImVec4(0.16f, 0.105f, 0.07f, 1.0f);
		c[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.05f, 0.035f, 1.0f);
		c[ImGuiCol_ScrollbarGrab] = ImVec4(0.38f, 0.26f, 0.13f, 1.0f);
		c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.36f, 0.17f, 1.0f);
		c[ImGuiCol_ScrollbarGrabActive] = gold;
		c[ImGuiCol_CheckMark] = gold;
		c[ImGuiCol_SliderGrab] = goldDim;
		c[ImGuiCol_SliderGrabActive] = gold;
		c[ImGuiCol_Button] = ImVec4(0.33f, 0.21f, 0.11f, 1.0f);
		c[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.32f, 0.14f, 1.0f);
		c[ImGuiCol_ButtonActive] = ImVec4(0.66f, 0.45f, 0.19f, 1.0f);
		c[ImGuiCol_Header] = ImVec4(0.36f, 0.12f, 0.07f, 1.0f);
		c[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.18f, 0.09f, 1.0f);
		c[ImGuiCol_HeaderActive] = ImVec4(0.62f, 0.24f, 0.11f, 1.0f);
		c[ImGuiCol_Separator] = goldDim;
		c[ImGuiCol_SeparatorHovered] = gold;
		c[ImGuiCol_SeparatorActive] = gold;
		c[ImGuiCol_ResizeGrip] = ImVec4(0.55f, 0.41f, 0.19f, 0.35f);
		c[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.70f, 0.36f, 0.6f);
		c[ImGuiCol_ResizeGripActive] = gold;
		c[ImGuiCol_Tab] = ImVec4(0.22f, 0.14f, 0.08f, 1.0f);
		c[ImGuiCol_TabHovered] = ImVec4(0.50f, 0.18f, 0.09f, 1.0f);
		c[ImGuiCol_TabSelected] = ImVec4(0.40f, 0.13f, 0.07f, 1.0f);
		c[ImGuiCol_TabDimmed] = ImVec4(0.18f, 0.12f, 0.07f, 1.0f);
		c[ImGuiCol_TabDimmedSelected] = ImVec4(0.30f, 0.11f, 0.06f, 1.0f);
		c[ImGuiCol_TableHeaderBg] = ImVec4(0.30f, 0.10f, 0.06f, 1.0f);
		c[ImGuiCol_TableBorderStrong] = goldDim;
		c[ImGuiCol_TableBorderLight] = ImVec4(0.30f, 0.22f, 0.12f, 1.0f);
		c[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
		c[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 0.85f, 0.55f, 0.035f);
		c[ImGuiCol_TextSelectedBg] = ImVec4(0.62f, 0.24f, 0.11f, 0.6f);
		c[ImGuiCol_DragDropTarget] = gold;
		c[ImGuiCol_NavCursor] = gold;
		c[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
		c[ImGuiCol_PlotHistogram] = gold;
	}

	// gold double rule with a diamond in the middle
	static void Ornament(ImDrawList* dl, float x0, float x1, float y)
	{
		const float mid = (x0 + x1) * 0.5f;
		dl->AddLine(ImVec2(x0, y), ImVec2(mid - 10, y), Gold, 1.0f);
		dl->AddLine(ImVec2(mid + 10, y), ImVec2(x1, y), Gold, 1.0f);
		dl->AddLine(ImVec2(x0, y + 3), ImVec2(mid - 10, y + 3), GoldDim, 1.0f);
		dl->AddLine(ImVec2(mid + 10, y + 3), ImVec2(x1, y + 3), GoldDim, 1.0f);
		dl->AddQuadFilled(ImVec2(mid, y - 4), ImVec2(mid + 7, y + 1.5f), ImVec2(mid, y + 7), ImVec2(mid - 7, y + 1.5f), Gold);
		dl->AddQuadFilled(ImVec2(mid, y - 1), ImVec2(mid + 3, y + 1.5f), ImVec2(mid, y + 4), ImVec2(mid - 3, y + 1.5f), Blood);
	}

	void Banner(const char* title, const char* subtitle, float height)
	{
		ImDrawList* dl = ImGui::GetWindowDrawList();
		const ImVec2 p0 = ImGui::GetCursorScreenPos();
		const float w = ImGui::GetContentRegionAvail().x;
		const ImVec2 p1(p0.x + w, p0.y + height);
		// ember-to-leather glow like the old "Dragonraja International" banner
		dl->AddRectFilledMultiColor(p0, p1, IM_COL32(92, 40, 16, 255), IM_COL32(58, 30, 14, 255), IM_COL32(20, 12, 8, 255), IM_COL32(44, 20, 10, 255));
		dl->AddRectFilledMultiColor(ImVec2(p0.x, p0.y), ImVec2(p0.x + w * 0.5f, p1.y), IM_COL32(150, 92, 30, 70), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0), IM_COL32(150, 92, 30, 40));
		dl->AddRect(p0, p1, Gold, 2.0f, 0, 1.5f);
		dl->AddRect(ImVec2(p0.x + 3, p0.y + 3), ImVec2(p1.x - 3, p1.y - 3), GoldDim, 2.0f);
		ImFont* f = Title ? Title : ImGui::GetFont();
		const float fs = f->LegacySize;
		const ImVec2 ts = f->CalcTextSizeA(fs, FLT_MAX, 0.0f, title);
		const float subH = subtitle ? ImGui::GetFontSize() + 2 : 0.0f;
		const ImVec2 tp(p0.x + (w - ts.x) * 0.5f, p0.y + (height - ts.y - subH) * 0.5f);
		if (Dragon)		// the red dragon either side of the title
		{
			const float d = (std::min)(height - 10.0f, 64.0f), dy = p0.y + (height - d) * 0.5f;
			dl->AddImage(Dragon, ImVec2(tp.x - d - 14, dy), ImVec2(tp.x - 14, dy + d));
			dl->AddImage(Dragon, ImVec2(tp.x + ts.x + 14, dy), ImVec2(tp.x + ts.x + 14 + d, dy + d), ImVec2(1, 0), ImVec2(0, 1));	// mirrored
		}
		dl->AddText(f, fs, ImVec2(tp.x + 2, tp.y + 2), IM_COL32(0, 0, 0, 200), title);		// shadow
		dl->AddText(f, fs, tp, Gold, title);
		if (subtitle)
		{
			const ImVec2 ss = ImGui::CalcTextSize(subtitle);
			dl->AddText(ImVec2(p0.x + (w - ss.x) * 0.5f, tp.y + ts.y), Parchment, subtitle);
		}
		ImGui::Dummy(ImVec2(w, height));
	}

	void SectionHeader(const char* text)
	{
		ImGui::PushFont(Heading, Heading->LegacySize);
		ImGui::TextColored(ImVec4(0.87f, 0.70f, 0.36f, 1.0f), "%s", text);
		ImGui::PopFont();
		const ImVec2 p = ImGui::GetCursorScreenPos();
		const float w = ImGui::GetContentRegionAvail().x;
		ImDrawList* dl = ImGui::GetWindowDrawList();
		dl->AddLine(ImVec2(p.x, p.y - 2), ImVec2(p.x + w, p.y - 2), GoldDim, 1.0f);
		ImGui::Dummy(ImVec2(0, 2));
	}

	bool ToolbarButton(const char* id, const char* label, const char* glyph, bool selected, float width)
	{
		const float h = 58.0f;
		ImGui::PushID(id);
		const ImVec2 p0 = ImGui::GetCursorScreenPos();
		const bool clicked = ImGui::InvisibleButton("##tb", ImVec2(width, h));
		const bool hover = ImGui::IsItemHovered();
		const ImVec2 p1(p0.x + width, p0.y + h);
		ImDrawList* dl = ImGui::GetWindowDrawList();
		const ImU32 top = selected ? IM_COL32(120, 36, 18, 255) : hover ? IM_COL32(86, 52, 24, 255) : IM_COL32(58, 38, 22, 255);
		dl->AddRectFilledMultiColor(p0, p1, top, top, LeatherDark, LeatherDark);
		dl->AddRect(p0, p1, selected ? Gold : GoldDim, 3.0f, 0, selected ? 2.0f : 1.0f);
		// emblem: a gold shield with the page's glyph
		const ImVec2 c(p0.x + width * 0.5f, p0.y + 19);
		dl->AddCircleFilled(c, 13.0f, selected ? Ember : IM_COL32(90, 30, 16, 255), 20);
		dl->AddCircle(c, 13.0f, Gold, 20, 1.5f);
		ImFont* f = Heading ? Heading : ImGui::GetFont();
		const ImVec2 gs = f->CalcTextSizeA(f->LegacySize, FLT_MAX, 0.0f, glyph);
		dl->AddText(f, f->LegacySize, ImVec2(c.x - gs.x * 0.5f, c.y - gs.y * 0.5f), Parchment, glyph);
		const ImVec2 ls = ImGui::CalcTextSize(label);
		dl->AddText(ImVec2(p0.x + (width - ls.x) * 0.5f, p0.y + 36), selected || hover ? Gold : Parchment, label);
		ImGui::PopID();
		return clicked;
	}

	bool BeginPanel(const char* id, const char* title, const ImVec2& size, bool fitHeight)
	{
		const bool open = fitHeight ? ImGui::BeginChild(id, ImVec2(size.x, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY)
		                            : ImGui::BeginChild(id, size, true);
		if (title && *title)
		{
			ImDrawList* dl = ImGui::GetWindowDrawList();
			const ImVec2 p = ImGui::GetCursorScreenPos();
			const float w = ImGui::GetContentRegionAvail().x;
			const float h = ImGui::GetFontSize() + 8;
			dl->AddRectFilledMultiColor(ImVec2(p.x - 4, p.y - 4), ImVec2(p.x + w + 4, p.y + h - 4), IM_COL32(96, 26, 14, 255), IM_COL32(60, 18, 10, 255), IM_COL32(60, 18, 10, 255), IM_COL32(96, 26, 14, 255));
			dl->AddLine(ImVec2(p.x - 4, p.y + h - 4), ImVec2(p.x + w + 4, p.y + h - 4), Gold, 1.0f);
			ImGui::PushFont(Heading, Heading->LegacySize);
			ImGui::TextColored(ImVec4(0.87f, 0.70f, 0.36f, 1.0f), "%s", title);
			ImGui::PopFont();
			ImGui::Dummy(ImVec2(0, 2));
		}
		return open;
	}
	void EndPanel() { ImGui::EndChild(); }

	static void Colored(const ImVec4& col, const char* fmt, va_list ap)
	{
		char buf[512];
		std::vsnprintf(buf, sizeof(buf), fmt, ap);
		ImGui::TextColored(col, "%s", buf);
	}
	void Good(const char* fmt, ...) { va_list ap; va_start(ap, fmt); Colored(ImVec4(0.55f, 0.85f, 0.45f, 1.0f), fmt, ap); va_end(ap); }
	void Bad(const char* fmt, ...) { va_list ap; va_start(ap, fmt); Colored(ImVec4(0.95f, 0.42f, 0.30f, 1.0f), fmt, ap); va_end(ap); }
	void Dim(const char* fmt, ...) { va_list ap; va_start(ap, fmt); Colored(ImVec4(0.62f, 0.54f, 0.42f, 1.0f), fmt, ap); va_end(ap); }

	// (used by Apps.cpp for the window frame)
	void FrameOrnament(float x0, float x1, float y) { Ornament(ImGui::GetWindowDrawList(), x0, x1, y); }
}
