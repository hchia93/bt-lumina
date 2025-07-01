#include <sstream>
#include <iomanip>
#include <imgui.h>
#include "LuminaDeviceManagerViewModel.h"
#include "LuminaHelper.h"

LuminaDeviceManagerViewModel::LuminaDeviceManagerViewModel()
    : m_ShowDeviceDetails(false)
    , m_SelectedDeviceAddress("")
    , m_PropertyViewModel()
    , m_ActionBluetoothSwitch()
{
    BindActionOnDeviceDiscovered();
    m_ActionBluetoothSwitch.RequestGetIsBluetoothEnabled();
    m_ActionBluetoothSwitch.HandleOnErrorMessage([this](const std::string& msg) { RaiseErrorMessage(msg); });
    m_ActionDiscoverDevice.HandleOnErrorMessage([this](const std::string& msg) { RaiseErrorMessage(msg); });
}

void LuminaDeviceManagerViewModel::BindActionOnDeviceDiscovered()
{
    m_ActionDiscoverDevice.HandleOnDevicesDiscovered([this](const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>& discovered)
        {
            for (const auto& device : discovered)
            {
                Lumina::BluetoothDevice btDevice;
                btDevice.name = winrt::to_string(device.Name());
                btDevice.address = winrt::to_string(device.Id());
                btDevice.isConnected = false;
                btDevice.isPaired = device.Pairing().IsPaired();
                btDevice.signalStrength = 0;
                btDevice.deviceType = "Unknown";
                m_DeviceManager.AddDiscoveredDevice(btDevice);
            }
        });
}

inline void LuminaDeviceManagerViewModel::RaiseErrorMessage(const std::string& message) 
{ 
    m_ErrorMessageInfo.Show(message); 
}

void LuminaDeviceManagerViewModel::Render()
{
    RenderActionList();
    ImGui::Separator();
    RenderDeviceTable();

    m_ErrorMessageInfo.Render();
}

void LuminaDeviceManagerViewModel::RenderDeviceTable()
{

    ImGui::BeginChild("DeviceListChild", ImVec2(0, 200), true);
    ImGui::Columns(4, nullptr, false);
    ImGui::Text("Discovered Device");
    ImGui::NextColumn();
    ImGui::Text("Pair State");
    ImGui::NextColumn();
    ImGui::Text("Connect State");
    ImGui::NextColumn();
    ImGui::Text("Actions");
    ImGui::NextColumn();
    ImGui::Separator();
    const auto& discoveredDevices = m_DeviceManager.GetDiscoveredDevices();
    for (auto& device : discoveredDevices)
    {
        RenderDeviceEntry(device);
    }

    ImGui::Columns(1);
    ImGui::EndChild();
    m_PropertyViewModel.Render(m_DeviceManager);
}

void LuminaDeviceManagerViewModel::RenderDeviceEntry(const Lumina::BluetoothDevice& device)
{
    ImGui::PushID(device.address.c_str());
    bool selected = (m_SelectedDeviceAddress == device.address);
    if (ImGui::Selectable(device.name.c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick))
    {
        if (ImGui::IsMouseClicked(0))
        {
            OnDeviceSelected(device.address);
        }
    }
    if (ImGui::BeginPopupContextItem("##device_context"))
    {
        if (ImGui::MenuItem("Properties"))
        {
            m_PropertyViewModel.Show(device.address);
        }
        ImGui::EndPopup();
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Address: %s", device.address.c_str());
        ImGui::Text("Type: %s", device.deviceType.c_str());
        ImGui::Text("Signal: %d dBm", device.signalStrength);
        ImGui::Text("Status: %s%s",
            device.isConnected ? "Connected" : "",
            device.isPaired ? (device.isConnected ? ", " : "") + std::string("Paired") : "");
        ImGui::EndTooltip();
    }
    ImGui::NextColumn();
    if (device.isPaired)
    {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Paired");
    }
    else
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Unpaired");
    }
    ImGui::NextColumn();
    if (device.isConnected)
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected");
    }
    else
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Disconnected");
    }
    ImGui::NextColumn();
    if (device.isConnected)
    {
        if (ImGui::Button("Disconnect"))
        {
            OnDisconnectDevice(device.address);
        }
    }
    else
    {
        if (ImGui::Button("Connect"))
        {
            OnConnectDevice(device.address);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove"))
    {
        OnRemoveDevice(device.address);
    }
    ImGui::NextColumn();
    ImGui::PopID();
}

void LuminaDeviceManagerViewModel::RenderDeviceDetails(const Lumina::BluetoothDevice& device)
{
    ImGui::Text("Device Details");
    ImGui::Separator();
    ImGui::Text("Name: %s", device.name.c_str());
    ImGui::Text("Address: %s", device.address.c_str());
    ImGui::Text("Type: %s", device.deviceType.c_str());
    ImGui::Text("Signal Strength: %d dBm", device.signalStrength);
    ImGui::Text("Status: %s%s",
        device.isConnected ? "Connected" : "",
        device.isPaired ? (device.isConnected ? ", " : "") + std::string("Paired") : "");
    ImGui::Separator();
}

void LuminaDeviceManagerViewModel::RenderDeviceActions(const Lumina::BluetoothDevice& device)
{
    ImGui::Text("Actions:");
    if (device.isConnected)
    {
        if (ImGui::Button("Disconnect Device", ImVec2(120, 0)))
        {
            OnDisconnectDevice(device.address);
        }
    }
    else
    {
        if (ImGui::Button("Connect Device", ImVec2(120, 0)))
        {
            OnConnectDevice(device.address);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove Device", ImVec2(120, 0)))
    {
        OnRemoveDevice(device.address);
    }
}

void LuminaDeviceManagerViewModel::RenderActionList()
{
    bool btEnabled = m_ActionBluetoothSwitch.GetIsBluetoothEnabled();
    ImVec4 btColor = btEnabled ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 btColorHighlight = LuminaHelper::LightenColor(btColor, 0.15f);
    ImGui::PushStyleColor(ImGuiCol_Button, btColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btColorHighlight);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, btColorHighlight);

    ImGui::BeginDisabled(m_ActionBluetoothSwitch.GetIsStateRequested());
    if (ImGui::Button("BT", ImVec2(48, 0)))
    {
        m_ActionBluetoothSwitch.RequestToogleBluetoothEnabled();
    }
    ImGui::EndDisabled();

    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::BeginDisabled(m_ActionDiscoverDevice.GetIsScanRequested() || !m_ActionBluetoothSwitch.GetIsBluetoothEnabled());
    if (ImGui::Button("Scan", ImVec2(120, 0)))
    {
        m_DeviceManager.ClearDiscoveredDevices();
        m_ActionDiscoverDevice.RequestScan();
    }
    ImGui::EndDisabled();
    if (m_ActionDiscoverDevice.GetIsScanRequested())
    {
        ImGui::SameLine();
        ImGui::Text("Scanning...");
    }
}

void LuminaDeviceManagerViewModel::OnDeviceSelected(const std::string& deviceAddress)
{
    m_SelectedDeviceAddress = deviceAddress;
    m_ShowDeviceDetails = true;
}

void LuminaDeviceManagerViewModel::OnConnectDevice(const std::string& deviceAddress)
{
    m_DeviceManager.ConnectToDevice(deviceAddress);
}

void LuminaDeviceManagerViewModel::OnDisconnectDevice(const std::string& deviceAddress)
{
    m_DeviceManager.DisconnectFromDevice(deviceAddress);
}

void LuminaDeviceManagerViewModel::OnRemoveDevice(const std::string& deviceAddress)
{
    m_DeviceManager.RemoveDevice(deviceAddress);
    if (m_SelectedDeviceAddress == deviceAddress)
    {
        m_SelectedDeviceAddress = "";
        m_ShowDeviceDetails = false;
    }
}