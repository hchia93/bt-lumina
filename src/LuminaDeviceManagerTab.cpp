#include "LuminaDeviceManagerTab.h"
#include <imgui.h>
#include <sstream>
#include <iomanip>

LuminaDeviceManagerTab::LuminaDeviceManagerTab()
    : showDeviceDetails(false)
    , selectedDeviceAddress("")
{
    // Initialize buffers
    memset(deviceNameBuffer, 0, sizeof(deviceNameBuffer));
    memset(deviceAddressBuffer, 0, sizeof(deviceAddressBuffer));
    memset(deviceTypeBuffer, 0, sizeof(deviceTypeBuffer));
}

LuminaDeviceManagerTab::~LuminaDeviceManagerTab()
{
}

void LuminaDeviceManagerTab::Render()
{
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
    const auto& discoveredDevices = deviceManager.GetDiscoveredDevices();
    if (discoveredDevices.empty())
    {
        ImGui::Text("No devices discovered");
        if (deviceManager.IsScanning())
        {
            ImGui::Text("Scanning...");
        }
    }
    else
    {
        RenderDeviceList(discoveredDevices, "Discovered Devices");
    }
    
    ImGui::Separator();
    
    // Add device form
    RenderDeviceForm();
}

void LuminaDeviceManagerTab::RenderDeviceControlTab()
{
    ImGui::Text("Device Control Panel");
    ImGui::Separator();
    
    // Connected devices
    const auto& connectedDevices = deviceManager.GetConnectedDevices();
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
    const auto& pairedDevices = deviceManager.GetPairedDevices();
    if (pairedDevices.empty())
    {
        ImGui::Text("No paired devices");
    }
    else
    {
        RenderDeviceList(pairedDevices, "Paired Devices");
    }
    
    // Device details panel
    if (showDeviceDetails && !selectedDeviceAddress.empty())
    {
        ImGui::Separator();
        BluetoothDevice* device = deviceManager.GetDeviceByAddress(selectedDeviceAddress);
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
        if (ImGui::Selectable(device.name.c_str(), selectedDeviceAddress == device.address))
        {
            OnDeviceSelected(device.address);
        }
        
        // Device info on hover
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
        
        // Action buttons
        ImGui::SameLine();
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
    ImGui::Text("Scan Controls:");
    
    if (deviceManager.IsScanning())
    {
        if (ImGui::Button("Stop Scan", ImVec2(120, 0)))
        {
            deviceManager.StopScan();
        }
        ImGui::SameLine();
        ImGui::Text("Scanning in progress...");
    }
    else
    {
        if (ImGui::Button("Start Scan", ImVec2(120, 0)))
        {
            deviceManager.StartScan();
        }
        ImGui::SameLine();
        ImGui::Text("Ready to scan");
    }
}

void LuminaDeviceManagerTab::RenderDeviceForm()
{
    ImGui::Text("Add New Device:");
    ImGui::Separator();
    
    ImGui::InputText("Device Name", deviceNameBuffer, sizeof(deviceNameBuffer));
    ImGui::InputText("Device Address", deviceAddressBuffer, sizeof(deviceAddressBuffer));
    ImGui::InputText("Device Type", deviceTypeBuffer, sizeof(deviceTypeBuffer));
    
    if (ImGui::Button("Add Device", ImVec2(120, 0)))
    {
        OnAddDevice();
    }
}

void LuminaDeviceManagerTab::OnDeviceSelected(const std::string& deviceAddress)
{
    selectedDeviceAddress = deviceAddress;
    showDeviceDetails = true;
}

void LuminaDeviceManagerTab::OnConnectDevice(const std::string& deviceAddress)
{
    deviceManager.ConnectToDevice(deviceAddress);
}

void LuminaDeviceManagerTab::OnDisconnectDevice(const std::string& deviceAddress)
{
    deviceManager.DisconnectFromDevice(deviceAddress);
}

void LuminaDeviceManagerTab::OnRemoveDevice(const std::string& deviceAddress)
{
    deviceManager.RemoveDevice(deviceAddress);
    if (selectedDeviceAddress == deviceAddress)
    {
        selectedDeviceAddress = "";
        showDeviceDetails = false;
    }
}

void LuminaDeviceManagerTab::OnAddDevice()
{
    if (strlen(deviceNameBuffer) > 0 && strlen(deviceAddressBuffer) > 0)
    {
        BluetoothDevice newDevice;
        newDevice.name = deviceNameBuffer;
        newDevice.address = deviceAddressBuffer;
        newDevice.deviceType = strlen(deviceTypeBuffer) > 0 ? deviceTypeBuffer : "Unknown";
        newDevice.isConnected = false;
        newDevice.isPaired = false;
        newDevice.signalStrength = -50; // Default signal strength
        
        deviceManager.AddDevice(newDevice);
        
        // Clear form
        memset(deviceNameBuffer, 0, sizeof(deviceNameBuffer));
        memset(deviceAddressBuffer, 0, sizeof(deviceAddressBuffer));
        memset(deviceTypeBuffer, 0, sizeof(deviceTypeBuffer));
    }
} 