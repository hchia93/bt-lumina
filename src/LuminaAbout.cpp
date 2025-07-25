#include <imgui.h>
#include "LuminaAbout.h"



void LuminaAbout::Show()
{
    m_Visible = true;
}

void LuminaAbout::Hide()
{
    m_Visible = false;
}

bool LuminaAbout::IsVisible() const
{
    return m_Visible;
}

void LuminaAbout::Render()
{
    if (!m_Visible)
    {
        return;
    }

    float windowWidth = 320.0f;
    float heightPadding = 40.0f; // top + bottom
    float frameHeight = ImGui::GetFrameHeight();
    float lineHeight = ImGui::GetTextLineHeightWithSpacing();
    float textHeight = lineHeight * 2.0f;
    float totalHeight = (frameHeight * 2) + textHeight + (heightPadding * 2);

    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, totalHeight), ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImVec4 lightTitle = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, lightTitle);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, lightTitle);

    if (ImGui::BeginPopupModal("About", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove))
    {
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        // Center the first line
        float text1Width = ImGui::CalcTextSize("Project Lumina").x;
        float availWidth = ImGui::GetWindowSize().x;
        float text1StartX = (availWidth - text1Width) * 0.5f;
        if (text1StartX > 0)
        {
            ImGui::SetCursorPosX(text1StartX);
        }
        
        ImGui::Text("Project Lumina");
        // Center the second line
        float text2Width = ImGui::CalcTextSize("GitHub: hchia93/bt-lumina").x;
        float text2StartX = (availWidth - text2Width) * 0.5f;
        if (text2StartX > 0)
        {
            ImGui::SetCursorPosX(text2StartX);
        }
        ImGui::Text("GitHub: hchia93/bt-lumina");
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        float buttonWidth = ImGui::CalcTextSize("Back").x + 40.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - buttonWidth) * 0.5f);

        if (ImGui::Button("Back", ImVec2(buttonWidth, 0)))
        {
            ImGui::CloseCurrentPopup();
            m_Visible = false;
        }
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor(2);
}