#include <sstream>
#include <iomanip>
#include <imgui.h>
#include "LuminaDeviceManagerViewModel.h"
#include "LuminaHelper.h"

LuminaDeviceManagerViewModel::LuminaDeviceManagerViewModel()
    : m_ShowDeviceDetails(false)
    , m_SelectedDeviceAddress("")
    , m_PropertyViewModel()
    , m_BluetoothSwitch()
{
    m_DiscoverDevice.SetOnBeginScanCooldown([this]()
        {
            m_DeviceManager.ClearDiscoveredDevices();
        });
    m_DiscoverDevice.SetOnCompleteScanCooldown([]()
        {
            // No-op or set a flag if needed
        });
    m_DiscoverDevice.SetOnDevicesDiscovered([this](const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>& discovered)
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

    m_BluetoothSwitch.FetchBluetoothEnabledState();
}

LuminaDeviceManagerViewModel::~LuminaDeviceManagerViewModel()
{
}

void LuminaDeviceManagerViewModel::Render()
{
    m_DiscoverDevice.Tick();
    RenderDeviceDiscoveryTab();
}

void LuminaDeviceManagerViewModel::RenderDeviceDiscoveryTab()
{
    RenderActionList();
    ImGui::Separator();
    if (m_DiscoverDevice.CanRenderDeviceList())
    {
        const auto& discoveredDevices = m_DeviceManager.GetDiscoveredDevices();
        if (discoveredDevices.empty())
        {
            ImGui::Text("No devices discovered");
        }
        else
        {
            RenderDeviceList(discoveredDevices, nullptr);
        }
    }
    m_PropertyViewModel.Render(m_DeviceManager);
}

void LuminaDeviceManagerViewModel::RenderDeviceList(const std::vector<Lumina::BluetoothDevice>& devices, const char* title)
{
    ImGui::BeginChild(title ? title : "DeviceListChild", ImVec2(0, 200), true);
    // Table headers
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
    for (const auto& device : devices)
    {
        ImGui::PushID(device.address.c_str());
        // Discovered Device (name) - clickable and right-clickable
        bool selected = (m_SelectedDeviceAddress == device.address);
        if (ImGui::Selectable(device.name.c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (ImGui::IsMouseClicked(0)) // left click
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
        // Pair State
        if (device.isPaired)
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Paired");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Unpaired");
        }
        ImGui::NextColumn();
        // Connect State
        if (device.isConnected)
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Disconnected");
        }
        ImGui::NextColumn();
        // Actions
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
    ImGui::Columns(1);
    ImGui::EndChild();
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
    RenderDeviceActions(device);
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
    bool btEnabled = m_BluetoothSwitch.GetIsBluetoothEnabled();
    ImVec4 btColor = btEnabled ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 btColorHighlight = LuminaHelper::LightenColor(btColor, 0.15f);
    ImGui::PushStyleColor(ImGuiCol_Button, btColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btColorHighlight);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, btColorHighlight);

    ImGui::BeginDisabled(m_BluetoothSwitch.GetIsStateRequested());
    if (ImGui::Button("BT", ImVec2(48, 0)))
    {
        m_BluetoothSwitch.SetBluetoothEnabledAsync(!btEnabled, [this]()
            {
                // Optionally handle post-toggle actions
            });
    }
    ImGui::EndDisabled();

    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    m_DiscoverDevice.UpdateScanState();
    ImGui::BeginDisabled(m_DiscoverDevice.IsScanInProgress() || !m_BluetoothSwitch.GetIsBluetoothEnabled());
    if (ImGui::Button("Scan", ImVec2(120, 0)))
    {
        m_DeviceManager.ClearDiscoveredDevices();
        m_DiscoverDevice.StartScanWithGate();
    }
    ImGui::EndDisabled();
    if (m_DiscoverDevice.IsScanInProgress())
    {
        ImGui::SameLine();
        ImGui::ProgressBar(m_DiscoverDevice.GetScanProgress(), ImVec2(120, 0), nullptr);
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