#include "LuminaDeviceManager.h"
#include <algorithm>
#include <iostream>
#include <imgui.h>

LuminaDeviceManager::LuminaDeviceManager()
    : isScanning(false)
    , scanTimeoutSeconds(10)
{
    // Initialize with some sample paired devices for testing
    BluetoothDevice sampleDevice1;
    sampleDevice1.name = "My Bluetooth Headphones";
    sampleDevice1.address = "00:11:22:33:44:55";
    sampleDevice1.isConnected = false;
    sampleDevice1.isPaired = true;
    sampleDevice1.signalStrength = -45;
    sampleDevice1.deviceType = "Headphones";
    pairedDevices.push_back(sampleDevice1);

    BluetoothDevice sampleDevice2;
    sampleDevice2.name = "Living Room Speaker";
    sampleDevice2.address = "AA:BB:CC:DD:EE:FF";
    sampleDevice2.isConnected = true;
    sampleDevice2.isPaired = true;
    sampleDevice2.signalStrength = -52;
    sampleDevice2.deviceType = "Speaker";
    pairedDevices.push_back(sampleDevice2);
    connectedDevices.push_back(sampleDevice2);
}

LuminaDeviceManager::~LuminaDeviceManager()
{
    if (isScanning)
    {
        StopScan();
    }
}

void LuminaDeviceManager::StartScan()
{
    if (!isScanning)
    {
        isScanning = true;
        ClearDiscoveredDevices();
        std::cout << "Started scanning for Bluetooth devices..." << std::endl;
        
        // TODO: Implement actual Bluetooth scanning
        // For now, add some sample discovered devices
        BluetoothDevice newDevice1;
        newDevice1.name = "Unknown Device 1";
        newDevice1.address = "11:22:33:44:55:66";
        newDevice1.isConnected = false;
        newDevice1.isPaired = false;
        newDevice1.signalStrength = -60;
        newDevice1.deviceType = "Unknown";
        discoveredDevices.push_back(newDevice1);

        BluetoothDevice newDevice2;
        newDevice2.name = "iPhone 15";
        newDevice2.address = "22:33:44:55:66:77";
        newDevice2.isConnected = false;
        newDevice2.isPaired = false;
        newDevice2.signalStrength = -48;
        newDevice2.deviceType = "Phone";
        discoveredDevices.push_back(newDevice2);
    }
}

void LuminaDeviceManager::StopScan()
{
    if (isScanning)
    {
        isScanning = false;
        std::cout << "Stopped scanning for Bluetooth devices." << std::endl;
    }
}

bool LuminaDeviceManager::IsScanning() const
{
    return isScanning;
}

void LuminaDeviceManager::AddDevice(const BluetoothDevice& device)
{
    // Check if device already exists in paired devices
    auto it = std::find_if(pairedDevices.begin(), pairedDevices.end(),
        [&device](const BluetoothDevice& d) { return d.address == device.address; });
    
    if (it == pairedDevices.end())
    {
        BluetoothDevice newDevice = device;
        newDevice.isPaired = true;
        pairedDevices.push_back(newDevice);
        std::cout << "Added device: " << device.name << " (" << device.address << ")" << std::endl;
    }
    else
    {
        std::cout << "Device already exists: " << device.name << std::endl;
    }
}

void LuminaDeviceManager::RemoveDevice(const std::string& deviceAddress)
{
    // Remove from paired devices
    auto it = std::remove_if(pairedDevices.begin(), pairedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    if (it != pairedDevices.end())
    {
        pairedDevices.erase(it, pairedDevices.end());
        std::cout << "Removed paired device: " << deviceAddress << std::endl;
    }

    // Remove from connected devices
    it = std::remove_if(connectedDevices.begin(), connectedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    if (it != connectedDevices.end())
    {
        connectedDevices.erase(it, connectedDevices.end());
        std::cout << "Removed connected device: " << deviceAddress << std::endl;
    }
}

void LuminaDeviceManager::ConnectToDevice(const std::string& deviceAddress)
{
    BluetoothDevice* device = GetDeviceByAddress(deviceAddress);
    if (device && !device->isConnected)
    {
        device->isConnected = true;
        
        // Add to connected devices list if not already there
        auto it = std::find_if(connectedDevices.begin(), connectedDevices.end(),
            [&deviceAddress](const BluetoothDevice& d) { return d.address == deviceAddress; });
        
        if (it == connectedDevices.end())
        {
            connectedDevices.push_back(*device);
        }
        
        std::cout << "Connected to device: " << device->name << std::endl;
    }
    else if (!device)
    {
        std::cout << "Device not found: " << deviceAddress << std::endl;
    }
}

void LuminaDeviceManager::DisconnectFromDevice(const std::string& deviceAddress)
{
    BluetoothDevice* device = GetDeviceByAddress(deviceAddress);
    if (device && device->isConnected)
    {
        device->isConnected = false;
        
        // Remove from connected devices list
        auto it = std::remove_if(connectedDevices.begin(), connectedDevices.end(),
            [&deviceAddress](const BluetoothDevice& d) { return d.address == deviceAddress; });
        
        if (it != connectedDevices.end())
        {
            connectedDevices.erase(it, connectedDevices.end());
        }
        
        std::cout << "Disconnected from device: " << device->name << std::endl;
    }
}

const std::vector<BluetoothDevice>& LuminaDeviceManager::GetDiscoveredDevices() const
{
    return discoveredDevices;
}

const std::vector<BluetoothDevice>& LuminaDeviceManager::GetPairedDevices() const
{
    return pairedDevices;
}

const std::vector<BluetoothDevice>& LuminaDeviceManager::GetConnectedDevices() const
{
    return connectedDevices;
}

BluetoothDevice* LuminaDeviceManager::GetDeviceByAddress(const std::string& deviceAddress)
{
    // Search in paired devices first
    auto it = std::find_if(pairedDevices.begin(), pairedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    if (it != pairedDevices.end())
    {
        return &(*it);
    }
    
    // Search in discovered devices
    it = std::find_if(discoveredDevices.begin(), discoveredDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    if (it != discoveredDevices.end())
    {
        return &(*it);
    }
    
    return nullptr;
}

bool LuminaDeviceManager::IsDeviceConnected(const std::string& deviceAddress) const
{
    auto it = std::find_if(connectedDevices.begin(), connectedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    return it != connectedDevices.end();
}

bool LuminaDeviceManager::IsDevicePaired(const std::string& deviceAddress) const
{
    auto it = std::find_if(pairedDevices.begin(), pairedDevices.end(),
        [&deviceAddress](const BluetoothDevice& device) { return device.address == deviceAddress; });
    
    return it != pairedDevices.end();
}

void LuminaDeviceManager::Render()
{
    ImGui::Begin("Device Manager");
    
    // Scanning controls
    ImGui::Text("Device Discovery");
    ImGui::Separator();
    
    if (!isScanning)
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
    if (discoveredDevices.empty())
    {
        ImGui::Text("No devices found");
    }
    else
    {
        for (const auto& device : discoveredDevices)
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
    if (pairedDevices.empty())
    {
        ImGui::Text("No paired devices");
    }
    else
    {
        for (const auto& device : pairedDevices)
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
    discoveredDevices.clear();
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