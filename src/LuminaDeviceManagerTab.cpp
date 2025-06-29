#include "LuminaDeviceManagerTab.h"
#include <imgui.h>
#include <sstream>
#include <iomanip>
#include "LuminaLayoutHelper.h"

LuminaDeviceManagerTab::LuminaDeviceManagerTab()
    : m_ShowDeviceDetails(false)
    , m_SelectedDeviceAddress("")
{
    // Initialize buffers
    memset(m_DeviceNameBuffer, 0, sizeof(m_DeviceNameBuffer));
    memset(m_DeviceAddressBuffer, 0, sizeof(m_DeviceAddressBuffer));
    memset(m_DeviceTypeBuffer, 0, sizeof(m_DeviceTypeBuffer));

    m_DeviceManager.SetOnBeginScanCooldown([this]() {
        m_DeviceManager.ClearDiscoveredDevices();
        m_DeviceManager.ScanAsync();
    });
    m_DeviceManager.SetOnCompleteScanCooldown([]() {
        // No-op or set a flag if needed
    });
}

LuminaDeviceManagerTab::~LuminaDeviceManagerTab()
{
}

void LuminaDeviceManagerTab::Render()
{
    m_DeviceManager.Tick();
    if (ImGui::BeginTabBar("DeviceManagerTabs"))
    {
        if (ImGui::BeginTabItem("Device Discovery"))
        {
            RenderDeviceDiscoveryTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Device Control"))
        {
            RenderDeviceControlTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Add Device Manually"))
        {
            RenderDeviceForm();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void LuminaDeviceManagerTab::RenderDeviceDiscoveryTab()
{
    ImGui::Text("Bluetooth Device Discovery");
    ImGui::Separator();
    
    // Scan controls
    RenderScanControls();
    
    ImGui::Separator();
    
    // Discovered devices
    if (m_DeviceManager.CanRenderDeviceList())
    {
        const auto& discoveredDevices = m_DeviceManager.GetDiscoveredDevices();
        if (discoveredDevices.empty())
        {
            ImGui::Text("No devices discovered");
            if (m_DeviceManager.IsScanInProgress())
            {
                ImGui::Text("Scanning...");
            }
        }
        else
        {
            RenderDeviceList(discoveredDevices, "Discovered Devices");
        }
    }
    
    ImGui::Separator();
}

void LuminaDeviceManagerTab::RenderDeviceControlTab()
{
    ImGui::Text("Device Control Panel");
    ImGui::Separator();
    
    // Connected devices
    const auto& connectedDevices = m_DeviceManager.GetConnectedDevices();
    if (connectedDevices.empty())
    {
        ImGui::Text("No devices connected");
    }
    else
    {
        RenderDeviceList(connectedDevices, "Connected Devices");
    }
    
    ImGui::Separator();
    
    // Paired devices
    const auto& pairedDevices = m_DeviceManager.GetPairedDevices();
    if (pairedDevices.empty())
    {
        ImGui::Text("No paired devices");
    }
    else
    {
        RenderDeviceList(pairedDevices, "Paired Devices");
    }
    
    // Device details panel
    if (m_ShowDeviceDetails && !m_SelectedDeviceAddress.empty())
    {
        ImGui::Separator();
        BluetoothDevice* device = m_DeviceManager.GetDeviceByAddress(m_SelectedDeviceAddress);
        if (device)
        {
            RenderDeviceDetails(*device);
        }
    }
}

void LuminaDeviceManagerTab::RenderDeviceList(const std::vector<BluetoothDevice>& devices, const char* title)
{
    ImGui::Text("%s:", title);
    ImGui::BeginChild(title, ImVec2(0, 200), true);
    
    for (const auto& device : devices)
    {
        ImGui::PushID(device.address.c_str());
        
        // Device status icon
        if (device.isConnected)
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "●"); // Green dot for connected
        }
        else if (device.isPaired)
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "○"); // Yellow circle for paired
        }
        else
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "○"); // Gray circle for discovered
        }
        
        ImGui::SameLine();
        
        // Device name and selection
        float selectableY = ImGui::GetCursorPosY();
        if (ImGui::Selectable(device.name.c_str(), m_SelectedDeviceAddress == device.address, 0, ImVec2(300, 0)))
        {
            OnDeviceSelected(device.address);
        }
        float selectableHeight = ImGui::GetItemRectSize().y;
        float buttonY = selectableY + (selectableHeight - ImGui::GetFrameHeight()) * 0.5f;

        // Show tooltip only when hovering the device name
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
        
        ImGui::SameLine();
        ImGui::SetCursorPosY(buttonY);
        ImGui::PushItemWidth(80);
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
        ImGui::SetCursorPosY(buttonY);
        if (ImGui::Button("Remove"))
        {
            OnRemoveDevice(device.address);
        }
        ImGui::PopItemWidth();
        
        ImGui::PopID();
    }
    
    ImGui::EndChild();
}

void LuminaDeviceManagerTab::RenderDeviceDetails(const BluetoothDevice& device)
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

void LuminaDeviceManagerTab::RenderDeviceActions(const BluetoothDevice& device)
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

void LuminaDeviceManagerTab::RenderScanControls()
{
    m_DeviceManager.UpdateScanState();

    ImGui::BeginDisabled(m_DeviceManager.IsScanInProgress());
    if (ImGui::Button("Scan", ImVec2(120, 0)))
    {
        m_DeviceManager.ClearDiscoveredDevices();
        m_DeviceManager.StartScanWithGate();
    }
    ImGui::EndDisabled();

    if (m_DeviceManager.IsScanInProgress())
    {
        ImGui::SameLine();
        ImGui::ProgressBar(m_DeviceManager.GetScanProgress(), ImVec2(200, 0), nullptr);
        ImGui::SameLine();
        ImGui::Text("Scanning...");
    }
}

void LuminaDeviceManagerTab::RenderDeviceForm()
{
    ImGui::Text("Add New Device:");
    ImGui::Separator();
    
    ImGui::InputText("Device Name", m_DeviceNameBuffer, sizeof(m_DeviceNameBuffer));
    ImGui::InputText("Device Address", m_DeviceAddressBuffer, sizeof(m_DeviceAddressBuffer));
    ImGui::InputText("Device Type", m_DeviceTypeBuffer, sizeof(m_DeviceTypeBuffer));
    
    if (ImGui::Button("Add Device", ImVec2(120, 0)))
    {
        OnAddDevice();
    }
}

void LuminaDeviceManagerTab::OnDeviceSelected(const std::string& deviceAddress)
{
    m_SelectedDeviceAddress = deviceAddress;
    m_ShowDeviceDetails = true;
}

void LuminaDeviceManagerTab::OnConnectDevice(const std::string& deviceAddress)
{
    m_DeviceManager.ConnectToDevice(deviceAddress);
}

void LuminaDeviceManagerTab::OnDisconnectDevice(const std::string& deviceAddress)
{
    m_DeviceManager.DisconnectFromDevice(deviceAddress);
}

void LuminaDeviceManagerTab::OnRemoveDevice(const std::string& deviceAddress)
{
    m_DeviceManager.RemoveDevice(deviceAddress);
    if (m_SelectedDeviceAddress == deviceAddress)
    {
        m_SelectedDeviceAddress = "";
        m_ShowDeviceDetails = false;
    }
}

void LuminaDeviceManagerTab::OnAddDevice()
{
    if (strlen(m_DeviceNameBuffer) > 0 && strlen(m_DeviceAddressBuffer) > 0)
    {
        BluetoothDevice newDevice;
        newDevice.name = m_DeviceNameBuffer;
        newDevice.address = m_DeviceAddressBuffer;
        newDevice.deviceType = strlen(m_DeviceTypeBuffer) > 0 ? m_DeviceTypeBuffer : "Unknown";
        newDevice.isConnected = false;
        newDevice.isPaired = false;
        newDevice.signalStrength = -50; // Default signal strength
        
        m_DeviceManager.AddDevice(newDevice);
        
        // Clear form
        memset(m_DeviceNameBuffer, 0, sizeof(m_DeviceNameBuffer));
        memset(m_DeviceAddressBuffer, 0, sizeof(m_DeviceAddressBuffer));
        memset(m_DeviceTypeBuffer, 0, sizeof(m_DeviceTypeBuffer));
    }
} 