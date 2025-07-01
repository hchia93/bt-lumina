#include "LuminaErrorMessageInfo.h"
#include <imgui.h>

void LuminaErrorMessageInfo::Show(const std::string& message)
{
    m_Message = message;
    m_Visible = true;
}

void LuminaErrorMessageInfo::Hide()
{
    m_Visible = false;
}

bool LuminaErrorMessageInfo::IsVisible() const
{
    return m_Visible;
}

void LuminaErrorMessageInfo::Render()
{
    if (!m_Visible)
    {
        return;
    }

    float windowWidth = 320.0f;
    float textRegionWidth = windowWidth - 40.0f; // 20px padding on each side

    float heightPadding = 40.0f; // top + bottom
    float frameHeight = ImGui::GetFrameHeight();

    ImVec2 textSize = ImGui::CalcTextSize(m_Message.c_str(), nullptr, false, textRegionWidth);
    float textHeight = textSize.y;

    float totalHeight = (frameHeight * 2) + textHeight + (heightPadding * 2);

    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, totalHeight), ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImVec4 lightTitle = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, lightTitle);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, lightTitle);

    ImGui::OpenPopup("Error");
    if (ImGui::BeginPopupModal("Error", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove))
    {
        // Center the error message text
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        float textWidth = ImGui::CalcTextSize(m_Message.c_str()).x;
        float availWidth = ImGui::GetWindowSize().x;
        float textStartX = (availWidth - textWidth) * 0.5f;
        if (textStartX > 0)
        {
            ImGui::SetCursorPosX(textStartX);
        }
        ImGui::TextWrapped("%s", m_Message.c_str());
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        float buttonWidth = ImGui::CalcTextSize("OK").x + 40.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - buttonWidth) * 0.5f);

        if (ImGui::Button("OK", ImVec2(buttonWidth, 0)))
        {
            ImGui::CloseCurrentPopup();
            m_Visible = false;
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor(2);
}