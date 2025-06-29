#include "LuminaMainWindow.h"
#include <imgui.h>

void LuminaMainWindow::Render()
{
	if (m_About.CanShow())
	{
		ImGui::OpenPopup("AboutModal");
	}

	// Menu bar at the top
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New");
			ImGui::MenuItem("Open...");
			ImGui::MenuItem("Save");
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				m_About.Show();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// Main window below the menu bar
	float menuBarHeight = ImGui::GetFrameHeight();
	ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(1280, 720 - menuBarHeight));
	ImGui::Begin("Lumina", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove);

	if (ImGui::BeginTabBar("MainTabs"))
	{
		if (ImGui::BeginTabItem("Manage Device"))
		{
			m_DeviceManager.Render();
			ImGui::EndTabItem();
		}
		
		ImGui::EndTabBar();
	}
	ImGui::End();

	m_About.Render();
} 
