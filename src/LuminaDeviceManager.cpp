#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include "LuminaDeviceManager.h"
#include <algorithm>
#include <iostream>
#include <imgui.h>
#include <chrono>

using namespace winrt;
using namespace Windows::Devices::Enumeration;

LuminaDeviceManager::LuminaDeviceManager()
    : m_IsScanning(false)
    , m_ScanTimeoutSeconds(10)
    , m_LastScanTime(std::chrono::steady_clock::now())
    , m_ScanInProgress(false)
    , m_ScanStartTime(std::chrono::steady_clock::now())
    , m_CanRenderDeviceList(true)
{
    winrt::init_apartment();
}

LuminaDeviceManager::~LuminaDeviceManager()
{
    if (m_IsScanning)
    {
        StopScan();
    }
}

void LuminaDeviceManager::StartScan()
{
    if (!m_IsScanning)
    {
        m_IsScanning = true;
        ClearDiscoveredDevices(); // remove
        ScanAsync(); //add back within a frame
    }
}

void LuminaDeviceManager::ScanAsync()
{
    // Use WinRT to scan for Bluetooth devices
    auto selector = Windows::Devices::Bluetooth::BluetoothDevice::GetDeviceSelector();
    auto asyncOp = DeviceInformation::FindAllAsync(selector);
    asyncOp.Completed([this](auto const& op, auto const& status) {
        if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
        {
            auto devices = op.GetResults();
            for (uint32_t i = 0; i < devices.Size(); ++i)
            {
                auto device = devices.GetAt(i);
                BluetoothDevice btDevice;
                btDevice.name = winrt::to_string(device.Name());
                btDevice.address = winrt::to_string(device.Id());
                btDevice.isConnected = false;
                btDevice.isPaired = device.Pairing().IsPaired();
                btDevice.signalStrength = 0; // Not available in this API
                btDevice.deviceType = "Unknown";
                // Prevent duplicates by address
                auto it = std::find_if(m_DiscoveredDevices.begin(), m_DiscoveredDevices.end(),[&btDevice](const BluetoothDevice& d) 
                { 
                    return d.address == btDevice.address; 
                });

                if (it == m_DiscoveredDevices.end()) 
                {
                    m_DiscoveredDevices.push_back(btDevice);
                }
            }
        }
        m_IsScanning = false;
    });
}

void LuminaDeviceManager::StopScan()
{
    m_IsScanning = false;
}

bool LuminaDeviceManager::GetIsScanning() const
{
    return m_IsScanning;
}

void LuminaDeviceManager::AddDevice(const BluetoothDevice& device)
{
    // Check if device already exists in paired devices
    auto it = std::find_if(m_PairedDevices.begin(), m_PairedDevices.end(),
        [&device](const BluetoothDevice& d) { return d.address == device.address; });
    
    if (it == m_PairedDevices.end())
    {
        BluetoothDevice newDevice = device;
        newDevice.isPaired = true;
        m_PairedDevices.push_back(newDevice);
    }
}

void LuminaDeviceManager::RemoveDevice(const std::string& deviceAddress)
{
    // Remove from paired devices
    auto it = std::remove_if(m_PairedDevices.begin(), m_PairedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    if (it != m_PairedDevices.end())
    {
        m_PairedDevices.erase(it, m_PairedDevices.end());
    }

    // Remove from connected devices
    it = std::remove_if(m_ConnectedDevices.begin(), m_ConnectedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    if (it != m_ConnectedDevices.end())
    {
        m_ConnectedDevices.erase(it, m_ConnectedDevices.end());
    }
}

void LuminaDeviceManager::ConnectToDevice(const std::string& deviceAddress)
{
    BluetoothDevice* device = GetDeviceByAddress(deviceAddress);
    if (device && !device->isConnected)
    {
        device->isConnected = true;
        
        // Add to connected devices list if not already there
        auto it = std::find_if(m_ConnectedDevices.begin(), m_ConnectedDevices.end(),
            [&deviceAddress](const BluetoothDevice& d) { return d.address == deviceAddress; });
        
        if (it == m_ConnectedDevices.end())
        {
            m_ConnectedDevices.push_back(*device);
        }
    }
}

void LuminaDeviceManager::DisconnectFromDevice(const std::string& deviceAddress)
{
    BluetoothDevice* device = GetDeviceByAddress(deviceAddress);
    if (device && device->isConnected)
    {
        device->isConnected = false;
        
        // Remove from connected devices list
        auto it = std::remove_if(m_ConnectedDevices.begin(), m_ConnectedDevices.end(),
            [&deviceAddress](const BluetoothDevice& d) { return d.address == deviceAddress; });
        
        if (it != m_ConnectedDevices.end())
        {
            m_ConnectedDevices.erase(it, m_ConnectedDevices.end());
        }
    }
}

const std::vector<BluetoothDevice>& LuminaDeviceManager::GetDiscoveredDevices() const
{
    return m_DiscoveredDevices;
}

const std::vector<BluetoothDevice>& LuminaDeviceManager::GetPairedDevices() const
{
    return m_PairedDevices;
}

const std::vector<BluetoothDevice>& LuminaDeviceManager::GetConnectedDevices() const
{
    return m_ConnectedDevices;
}

BluetoothDevice* LuminaDeviceManager::GetDeviceByAddress(const std::string& deviceAddress)
{
    // Search in paired devices first
    auto it = std::find_if(m_PairedDevices.begin(), m_PairedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    if (it != m_PairedDevices.end())
    {
        return &(*it);
    }
    
    // Search in discovered devices
    it = std::find_if(m_DiscoveredDevices.begin(), m_DiscoveredDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    if (it != m_DiscoveredDevices.end())
    {
        return &(*it);
    }
    
    return nullptr;
}

bool LuminaDeviceManager::IsDeviceConnected(const std::string& deviceAddress) const
{
    auto it = std::find_if(m_ConnectedDevices.begin(), m_ConnectedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    return it != m_ConnectedDevices.end();
}

bool LuminaDeviceManager::IsDevicePaired(const std::string& deviceAddress) const
{
    auto it = std::find_if(m_PairedDevices.begin(), m_PairedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    return it != m_PairedDevices.end();
}

void LuminaDeviceManager::Render()
{
    ImGui::Begin("Device Manager");
    
    // Scanning controls
    ImGui::Text("Device Discovery");
    ImGui::Separator();
    
    if (!m_IsScanning)
    {
        if (ImGui::Button("Start Scan", ImVec2(120, 0)))
        {
            StartScan();
        }
    }
    else
    {
        if (ImGui::Button("Stop Scan", ImVec2(120, 0)))
        {
            StopScan();
        }
        ImGui::SameLine();
        ImGui::Text("Scanning...");
    }
    
    ImGui::Separator();
    
    // Discovered devices
    ImGui::Text("Discovered Devices:");
    ImGui::BeginChild("DiscoveredDevices", ImVec2(400, 150), true);
    if (m_DiscoveredDevices.empty())
    {
        ImGui::Text("No devices found");
    }
    else
    {
        for (const auto& device : m_DiscoveredDevices)
        {
            ImGui::Text("%s (%s)", device.name.c_str(), device.address.c_str());
            ImGui::SameLine();
            ImGui::Text("Signal: %d dBm", device.signalStrength);
            
            if (ImGui::Button(("Add##" + device.address).c_str(), ImVec2(60, 0)))
            {
                AddDevice(device);
            }
            ImGui::SameLine();
            if (ImGui::Button(("Connect##" + device.address).c_str(), ImVec2(60, 0)))
            {
                ConnectToDevice(device.address);
            }
        }
    }
    ImGui::EndChild();
    
    ImGui::Separator();
    
    // Paired devices
    ImGui::Text("Paired Devices:");
    ImGui::BeginChild("PairedDevices", ImVec2(400, 150), true);
    if (m_PairedDevices.empty())
    {
        ImGui::Text("No paired devices");
    }
    else
    {
        for (const auto& device : m_PairedDevices)
        {
            ImGui::Text("%s (%s)", device.name.c_str(), device.address.c_str());
            ImGui::SameLine();
            if (device.isConnected)
            {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected");
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Disconnected");
            }
            
            if (!device.isConnected)
            {
                ImGui::SameLine();
                if (ImGui::Button(("Connect##paired" + device.address).c_str(), ImVec2(60, 0)))
                {
                    ConnectToDevice(device.address);
                }
            }
            else
            {
                ImGui::SameLine();
                if (ImGui::Button(("Disconnect##paired" + device.address).c_str(), ImVec2(60, 0)))
                {
                    DisconnectFromDevice(device.address);
                }
            }
            
            ImGui::SameLine();
            if (ImGui::Button(("Remove##paired" + device.address).c_str(), ImVec2(60, 0)))
            {
                RemoveDevice(device.address);
            }
        }
    }
    ImGui::EndChild();
    
    ImGui::End();
}

void LuminaDeviceManager::ClearDiscoveredDevices()
{
    m_DiscoveredDevices.clear();
}

void LuminaDeviceManager::UpdateDeviceStatus(const std::string& deviceAddress, bool connected, bool paired)
{
    BluetoothDevice* device = GetDeviceByAddress(deviceAddress);
    if (device)
    {
        device->isConnected = connected;
        device->isPaired = paired;
    }
}

void LuminaDeviceManager::Tick()
{
    auto now = std::chrono::steady_clock::now();
    if (m_IsScanning && std::chrono::duration_cast<std::chrono::seconds>(now - m_LastScanTime).count() >= 1)
    {
        m_LastScanTime = now;
        ScanAsync();
    }
}

void LuminaDeviceManager::StartScanWithGate()
{
    if (!m_ScanInProgress)
    {
        m_ScanInProgress = true;
        m_ScanStartTime = std::chrono::steady_clock::now();
        m_CanRenderDeviceList = false;
        if (m_OnBeginScanCooldown) 
        { 
            m_OnBeginScanCooldown(); 
        }
    }
}

bool LuminaDeviceManager::IsScanInProgress() const
{
    return m_ScanInProgress;
}

float LuminaDeviceManager::GetScanProgress() const
{
    if (!m_ScanInProgress)
    {
        return 0.0f;
    }
    
    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - m_ScanStartTime).count();
    return std::min(elapsed / m_ScanDuration, 1.0f);
}

void LuminaDeviceManager::UpdateScanState()
{
    if (m_ScanInProgress)
    {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration<float>(now - m_ScanStartTime).count();
        if (elapsed >= m_ScanDuration)
        {
            m_ScanInProgress = false;
            StopScan();
            m_CanRenderDeviceList = true;
            if (m_OnCompleteScanCooldown) { m_OnCompleteScanCooldown(); }
        }
    }
} 