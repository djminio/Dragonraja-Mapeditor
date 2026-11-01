#pragma once
// =====================================================================
// Theme.h - the Dragon Raja look: dark leather / wood, gold trim, parchment text,
// a script title font like the game's logo.
// =====================================================================
#include <imgui.h>

namespace Theme
{
	void Apply();							// colours, sizes, fonts (call once after ImGui::CreateContext)
	extern ImFont* Body;					// normal text (Georgia)
	extern ImFont* Title;					// big script title (Monotype Corsiva / Georgia Italic)
	extern ImFont* Heading;					// section headings
	extern ImTextureID Dragon;				// the red dragon (Apps::Init makes it; 0 = none) - drawn in the banner

	// colours used by the drawing helpers
	extern const ImU32 Gold, GoldDim, Parchment, Blood, Leather, LeatherDark, Ember;

	// the gold "Dragon Raja" banner across the top of a window / login card
	void Banner(const char* title, const char* subtitle, float height);
	// a heading with a gold rule under it
	void SectionHeader(const char* text);
	// a big toolbar button like the old RMtool (label under an emblem); true when clicked
	bool ToolbarButton(const char* id, const char* label, const char* glyph, bool selected, float width = 118.0f);
	// a gold-framed panel (BeginChild with a title strip); EndPanel() after
	bool BeginPanel(const char* id, const char* title, const ImVec2& size, bool fitHeight = false);	// fitHeight: as tall as what is in it
	void EndPanel();
	// gold double rule with a diamond (window frame)
	void FrameOrnament(float x0, float x1, float y);
	// coloured text badges
	void Good(const char* fmt, ...);
	void Bad(const char* fmt, ...);
	void Dim(const char* fmt, ...);
}
