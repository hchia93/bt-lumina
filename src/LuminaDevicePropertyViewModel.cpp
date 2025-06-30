#include "LuminaDevicePropertyViewModel.h"
#include <imgui.h>

LuminaDevicePropertyViewModel::LuminaDevicePropertyViewModel()
    : m_Visible(false)
    , m_DeviceAddress("")
{
}

void LuminaDevicePropertyViewModel::Show(const std::string& deviceAddress)
{
    m_DeviceAddress = deviceAddress;
    m_Visible = true;
}

void LuminaDevicePropertyViewModel::Hide()
{
    m_Visible = false;
    m_DeviceAddress.clear();
}

bool LuminaDevicePropertyViewModel::IsVisible() const
{
    return m_Visible && !m_DeviceAddress.empty();
}

void LuminaDevicePropertyViewModel::Render(LuminaDeviceManager& deviceManager)
{
    if (!IsVisible())
    {
        return;
    }
        
    Lumina::BluetoothDevice* device = deviceManager.GetDeviceByAddress(m_DeviceAddress);
    if (!device)
    {
        return;
    }
      
    ImGui::SetNextWindowSize(ImVec2(350, 220), ImGuiCond_Appearing);
    if (ImGui::Begin("Device Properties", &m_Visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        ImGui::Text("Device Name: %s", device->name.c_str());
        ImGui::Text("Address: %s", device->address.c_str());
        ImGui::Text("Type: %s", device->deviceType.c_str());
        ImGui::Text("Signal Strength: %d dBm", device->signalStrength);
        ImGui::Text("Paired: %s", device->isPaired ? "Yes" : "No");
        ImGui::Text("Connected: %s", device->isConnected ? "Yes" : "No");
        if (ImGui::Button("Close"))
        {
            Hide();
        }
    }
    ImGui::End();
}