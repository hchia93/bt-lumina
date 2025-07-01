#pragma once
#include <imgui.h>

namespace LuminaHelper
{
    ImVec4 DarkenColor(const ImVec4& color, float percent);
    ImVec4 LightenColor(const ImVec4& color, float percent);
    float GetMenuBarPosY();
}
namespace LuminaConfig
{
    constexpr int WindowWidth = 1200;
    constexpr int WindowHeight = 720;
}