#pragma once
#include <imgui.h>

namespace LuminaHelper
{
    ImVec4 LightenColor(const ImVec4& color, float percent);
    float GetMenuBarPosY();
}