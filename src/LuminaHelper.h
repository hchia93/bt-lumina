#pragma once
#include <imgui.h>

namespace LuminaHelper
{
    ImVec4 DarkenColor(const ImVec4& color, float percent);
    ImVec4 LightenColor(const ImVec4& color, float percent);
    float GetMenuBarPosY();
}

namespace ImGui
{
    void ButtonDisabled(const char* label, const ImVec2& size, ImVec4& color);
}