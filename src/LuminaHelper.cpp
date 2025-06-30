#define NOMINMAX
#include <algorithm>
#include <imgui.h>
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
}

namespace ImGui
{
    void ButtonDisabled(const char* label, const ImVec2& size, ImVec4& color)
    {
        ImVec4 disabledColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        disabledColor = LuminaHelper::DarkenColor(disabledColor, 0.2f);
        ImGui::PushStyleColor(ImGuiCol_Button, disabledColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, disabledColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, disabledColor);
        ImGui::Button(label, size);
        ImGui::PopStyleColor(3);
    }
}