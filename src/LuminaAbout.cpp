#include <imgui.h>
#include "LuminaAbout.h"

namespace LuminaConfig
{
    constexpr int WindowWidth = 1200;
    constexpr int WindowHeight = 720;
}

void LuminaAbout::Show()
{
    m_IsDisplayed = true;
}

void LuminaAbout::Close()
{
    m_IsDisplayed = false;
}

bool LuminaAbout::CanShow()
{
    return m_IsDisplayed;
}

void LuminaAbout::Render()
{
    // About modal popup - render after main window to ensure proper layering
    ImGui::SetNextWindowSize(ImVec2(240, 180), ImGuiCond_Appearing);
    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Push lighter title bar color
    ImVec4 lightTitle = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, lightTitle);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, lightTitle);

    if (ImGui::BeginPopupModal("About", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Project Lumina");
        ImGui::Separator();
        ImGui::Text("GitHub: hchia93/bt-lumina");
        ImGui::Separator();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Back").x) * 0.5f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);

        if (ImGui::Button("Back"))
        {
            ImGui::CloseCurrentPopup();
            m_IsDisplayed = false;
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor(2);
}