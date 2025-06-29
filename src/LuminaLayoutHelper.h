#pragma once
#include <imgui.h>

class LuminaLayoutHelper
{
public:
	static float GetMenuBarHeight()
	{
		return ImGui::GetFontSize() + m_PaddingSize;
	}
private:
	static float m_PaddingSize;
}; 