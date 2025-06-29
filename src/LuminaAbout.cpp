#include "LuminaAbout.h"
#include <imgui.h>

void LuminaAbout::Show()
{
	m_Show = true;
}

void LuminaAbout::Close()
{
	m_Show = false;
}

bool LuminaAbout::CanShow()
{
	return m_Show;
}

void LuminaAbout::Render()
{
	// About modal popup - render after main window to ensure proper layering
	ImGui::SetNextWindowSize(ImVec2(260, 180), ImGuiCond_Appearing);
	ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("AboutModal", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("BT-Lumina");
		ImGui::Separator();
		ImGui::Text("Version: 1.0.0");
		ImGui::Text("Source: hchia93/bt-lumina");
		ImGui::Separator();
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Back").x) * 0.5f);

		if (ImGui::Button("Back"))
		{
			ImGui::CloseCurrentPopup();
			m_Show = false;
		}
		ImGui::EndPopup();
	}
}