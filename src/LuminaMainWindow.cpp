#include <imgui.h>
#include "LuminaMainWindow.h"
#include "LuminaHelper.h"


void LuminaMainWindow::ApplyImGuiStyle()
{
	ImGuiIO &io = ImGui::GetIO();
	io.IniFilename = NULL;
	ImGuiStyle * style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	// Define color variables for reuse
	ImVec4 colorText             = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	ImVec4 colorTextDisabled     = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	ImVec4 colorBgDark           = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	ImVec4 colorBgDarker         = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	ImVec4 colorBg               = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	ImVec4 colorHighlight        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	ImVec4 colorCloseBtn         = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	ImVec4 colorCloseBtnHover    = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	ImVec4 colorCloseBtnActive   = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	ImVec4 colorGrab             = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	ImVec4 colorTitleBgCollapsed = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	ImVec4 colorBorderShadow     = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	ImVec4 colorComboBg          = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	ImVec4 colorResizeGrip       = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	ImVec4 colorPlotHovered      = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	ImVec4 colorPlot             = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	ImVec4 colorTextSelectedBg   = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	ImVec4 colorModalDarken      = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	// Assign colors using variables
	ImVec4* colors = style->Colors;
	colors[ImGuiCol_Text] = colorText;
	colors[ImGuiCol_TextDisabled] = colorTextDisabled;
	colors[ImGuiCol_WindowBg] = colorBgDark;
	colors[ImGuiCol_ChildBg] = colorBgDarker;
	colors[ImGuiCol_PopupBg] = colorBgDarker;
	colors[ImGuiCol_Border] = colorText;
	colors[ImGuiCol_BorderShadow] = colorBorderShadow;
	colors[ImGuiCol_FrameBg] = colorBg;
	colors[ImGuiCol_FrameBgHovered] = colorTextDisabled;
	colors[ImGuiCol_FrameBgActive] = colorHighlight;
	colors[ImGuiCol_TitleBg] = colorBg;
	colors[ImGuiCol_TitleBgCollapsed] = colorTitleBgCollapsed;
	colors[ImGuiCol_TitleBgActive] = colorBgDarker;
	colors[ImGuiCol_MenuBarBg] = colorBg;
	colors[ImGuiCol_ScrollbarBg] = colorBg;
	colors[ImGuiCol_ScrollbarGrab] = colorGrab;
	colors[ImGuiCol_ScrollbarGrabHovered] = colorHighlight;
	colors[ImGuiCol_ScrollbarGrabActive] = colorBgDark;
	colors[ImGuiCol_CheckMark] = colorGrab;
	colors[ImGuiCol_SliderGrab] = colorGrab;
	colors[ImGuiCol_SliderGrabActive] = colorBgDark;
	colors[ImGuiCol_Button] = colorBg;
	colors[ImGuiCol_ButtonHovered] = colorTextDisabled;
	colors[ImGuiCol_ButtonActive] = colorHighlight;
	colors[ImGuiCol_Header] = colorBg;
	colors[ImGuiCol_HeaderHovered] = colorHighlight;
	colors[ImGuiCol_HeaderActive] = colorBgDark;
	colors[ImGuiCol_ResizeGrip] = colorResizeGrip;
	colors[ImGuiCol_ResizeGripHovered] = colorHighlight;
	colors[ImGuiCol_ResizeGripActive] = colorBgDark;
	colors[ImGuiCol_PlotLines] = colorPlot;
	colors[ImGuiCol_PlotLinesHovered] = colorPlotHovered;
	colors[ImGuiCol_PlotHistogram] = colorPlot;
	colors[ImGuiCol_PlotHistogramHovered] = colorPlotHovered;
	colors[ImGuiCol_TextSelectedBg] = colorTextSelectedBg;
	colors[ImGuiCol_ModalWindowDimBg] = colorBg;

	// Always set the default font size to 14px
	ImFontConfig config;
	config.SizePixels = 14.0f;
	io.Fonts->AddFontDefault(&config);

	// Load Ruda-Bold.ttf from the resources folder and set as default
	ImFont* rudaFont = io.Fonts->AddFontFromFileTTF("../resources/Ruda-Bold.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	if (rudaFont)
	{
		io.FontDefault = rudaFont;
	}
}

void LuminaMainWindow::Render()
{
	if (m_About.IsVisible())
	{
		ImGui::OpenPopup("About");
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
	ImGui::SetNextWindowSize(ImVec2(LuminaConfig::WindowWidth, LuminaConfig::WindowHeight - menuBarHeight));
	ImGui::Begin("Lumina", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove);

	if (ImGui::BeginTabBar("MainTabs"))
	{
		if (ImGui::BeginTabItem("Device Discovery"))
		{
			m_DeviceManager.Render();
			ImGui::EndTabItem();
		}
		
		ImGui::EndTabBar();
	}
	ImGui::End();

	m_About.Render();
} 
