#define NOMINMAX
#include <algorithm>
#include <imgui.h>
#include <Windows.h>
#include "LuminaHelper.h"

namespace LuminaHelper
{
    ImVec4 LightenColor(const ImVec4& color, float percent)
    {
        float factor = 1.0f + percent;
        return ImVec4(
            std::min(color.x * factor, 1.0f),
            std::min(color.y * factor, 1.0f),
            std::min(color.z * factor, 1.0f),
            color.w
        );
    }

    ImVec4 DarkenColor(const ImVec4& color, float percent)
    {
        float factor = 1.0f - percent;
        return ImVec4(
            std::max(color.x * factor, 0.0f),
            std::max(color.y * factor, 0.0f),
            std::max(color.z * factor, 0.0f),
            color.w
        );
    }

    float GetMenuBarPosY()
    {
        static float padding = 6.0f;
        return ImGui::GetFontSize() + padding;
    }

    std::string WideStringToUtf8(const std::wstring& wstr)
    {
        if (wstr.empty())
        {
            return {};
        }

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
        return strTo;
    }
}

