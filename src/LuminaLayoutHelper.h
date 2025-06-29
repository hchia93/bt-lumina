#pragma once
#include <imgui.h>

namespace LuminaLayoutHelper
{
	static float m_PaddingSize = 6.0f;

	inline float GetMenuBarPosY()
	{
		return ImGui::GetFontSize() + m_PaddingSize;
	}
} 