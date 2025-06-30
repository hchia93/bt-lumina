#include <algorithm>
#include <iostream>
#include <chrono>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Radios.h>
#include <imgui.h>
#include "LuminaDeviceManager.h"

using namespace winrt;
using namespace Windows::Devices::Enumeration;

LuminaDeviceManager::LuminaDeviceManager()
    : m_IsShuttingDown(false)
{
    try
    {
        winrt::init_apartment();
    }
    catch (...)
    {
        // Handle initialization error
    }
}

LuminaDeviceManager::~LuminaDeviceManager()
{
    Cleanup();
}

void LuminaDeviceManager::Cleanup()
{
    m_IsShuttingDown = true;

    // Clear all device lists
    m_DiscoveredDevices.clear();
    m_PairedDevices.clear();
    m_ConnectedDevices.clear();
}

void LuminaDeviceManager::AddDevice(const Lumina::BluetoothDevice& device)
{
    // Check if device already exists in paired devices
    auto it = std::find_if(m_PairedDevices.begin(), m_PairedDevices.end(),
        [&device](const Lumina::BluetoothDevice& d) { return d.address == device.address; });

    if (it == m_PairedDevices.end())
    {
        Lumina::BluetoothDevice newDevice = device;
        newDevice.isPaired = true;
        m_PairedDevices.push_back(newDevice);
    }
}

void LuminaDeviceManager::RemoveDevice(const std::string& deviceAddress)
{
    if (m_IsShuttingDown)
    {
        return;
    }

    // Remove from paired devices
    auto it = std::remove_if(m_PairedDevices.begin(), m_PairedDevices.end(),
        [&deviceAddress](const Lumina::BluetoothDevice& device) { return device.address == deviceAddress; });
    if (it != m_PairedDevices.end())
    {
        m_PairedDevices.erase(it, m_PairedDevices.end());
    }
    // Remove from connected devices
    it = std::remove_if(m_ConnectedDevices.begin(), m_ConnectedDevices.end(),
        [&deviceAddress](const Lumina::BluetoothDevice& device) { return device.address == deviceAddress; });
    if (it != m_ConnectedDevices.end())
    {
        m_ConnectedDevices.erase(it, m_ConnectedDevices.end());
    }

    try
    {
        // Use WinRT to unpair (remove) the device from the system
        auto asyncOp = DeviceInformation::CreateFromIdAsync(winrt::to_hstring(deviceAddress));
        asyncOp.Completed([this, deviceAddress](auto const& op, auto const& status)
            {
                if (m_IsShuttingDown) return;

                if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
                {
                    try
                    {
                        auto devInfo = op.GetResults();
                        if (devInfo && devInfo.Pairing().IsPaired())
                        {
                            auto unpairOp = devInfo.Pairing().UnpairAsync();
                            unpairOp.Completed([this](auto const& asyncOp, auto const& status)
                                {
                                    if (m_IsShuttingDown) return;
                                    // Optionally handle unpair result
                                });
                        }
                    }
                    catch (...)
                    {
                        // Handle any errors during unpairing
                    }
                }
                else if (status == winrt::Windows::Foundation::AsyncStatus::Canceled)
                {
                    // Operation was canceled, this is expected during shutdown
                }
            });
    }
    catch (...)
    {
        // Handle any errors during device removal
    }
}

void LuminaDeviceManager::ConnectToDevice(const std::string& deviceAddress)
{
    if (m_IsShuttingDown)
    {
        return;
    }

    Lumina::BluetoothDevice* device = GetDeviceByAddress(deviceAddress);
    if (device && !device->isConnected)
    {
        try
        {
            // Use WinRT to pair (connect) the device
            std::string id = device->address;
            auto idH = winrt::to_hstring(id);
            auto asyncOp = DeviceInformation::CreateFromIdAsync(idH);
            asyncOp.Completed([this, device](auto const& op, auto const& status)
                {
                    if (m_IsShuttingDown) return;

                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
                    {
                        try
                        {
                            auto devInfo = op.GetResults();
                            if (devInfo && devInfo.Pairing().CanPair() && !devInfo.Pairing().IsPaired())
                            {
                                auto pairOp = devInfo.Pairing().PairAsync();
                                pairOp.Completed([this, device](auto const& asyncOp, auto const& status)
                                    {
                                        if (m_IsShuttingDown) return;

                                        if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
                                        {
                                            try
                                            {
                                                auto result = asyncOp.GetResults();
                                                if (result.Status() == Windows::Devices::Enumeration::DevicePairingResultStatus::Paired ||
                                                    result.Status() == Windows::Devices::Enumeration::DevicePairingResultStatus::AlreadyPaired)
                                                {
                                                    device->isConnected = true;
                                                    device->isPaired = true;
                                                    // Add to connected devices list if not already there
                                                    auto it = std::find_if(m_ConnectedDevices.begin(), m_ConnectedDevices.end(),
                                                        [device](const Lumina::BluetoothDevice& d) { return d.address == device->address; });
                                                    if (it == m_ConnectedDevices.end())
                                                    {
                                                        m_ConnectedDevices.push_back(*device);
                                                    }
                                                }
                                            }
                                            catch (...)
                                            {
                                                // Handle any errors during pairing result processing
                                            }
                                        }
                                        else if (status == winrt::Windows::Foundation::AsyncStatus::Canceled)
                                        {
                                            // Operation was canceled, this is expected during shutdown
                                        }
                                    });
                            }
                        }
                        catch (...)
                        {
                            // Handle any errors during device information processing
                        }
                    }
                    else if (status == winrt::Windows::Foundation::AsyncStatus::Canceled)
                    {
                        // Operation was canceled, this is expected during shutdown
                    }
                });
        }
        catch (...)
        {
            // Handle any errors during device connection
        }
    }
}

void LuminaDeviceManager::DisconnectFromDevice(const std::string& deviceAddress)
{
    Lumina::BluetoothDevice* device = GetDeviceByAddress(deviceAddress);
    if (device && device->isConnected)
    {
        device->isConnected = false;

        // Remove from connected devices list
        auto it = std::remove_if(m_ConnectedDevices.begin(), m_ConnectedDevices.end(),
            [&deviceAddress](const Lumina::BluetoothDevice& d) { return d.address == deviceAddress; });

        if (it != m_ConnectedDevices.end())
        {
            m_ConnectedDevices.erase(it, m_ConnectedDevices.end());
        }
    }
}

const std::vector<Lumina::BluetoothDevice>& LuminaDeviceManager::GetDiscoveredDevices() const
{
    return m_DiscoveredDevices;
}

void LuminaDeviceManager::AddDiscoveredDevice(const Lumina::BluetoothDevice& device)
{
    // Prevent duplicates by address
    auto it = std::find_if(m_DiscoveredDevices.begin(), m_DiscoveredDevices.end(),
        [&device](const Lumina::BluetoothDevice& d) { return d.address == device.address; });
    if (it == m_DiscoveredDevices.end())
    {
        m_DiscoveredDevices.push_back(device);
    }
}

void LuminaDeviceManager::ClearDiscoveredDevices()
{
    m_DiscoveredDevices.clear();
}

const std::vector<Lumina::BluetoothDevice>& LuminaDeviceManager::GetPairedDevices() const
{
    return m_PairedDevices;
}

const std::vector<Lumina::BluetoothDevice>& LuminaDeviceManager::GetConnectedDevices() const
{
    return m_ConnectedDevices;
}

Lumina::BluetoothDevice* LuminaDeviceManager::GetDeviceByAddress(const std::string& deviceAddress)
{
    // Search in paired devices first
    auto it = std::find_if(m_PairedDevices.begin(), m_PairedDevices.end(),
        [&deviceAddress](const Lumina::BluetoothDevice& device) { return device.address == deviceAddress; });

    if (it != m_PairedDevices.end())
    {
        return &(*it);
    }

    // Search in discovered devices
    it = std::find_if(m_DiscoveredDevices.begin(), m_DiscoveredDevices.end(),
        [&deviceAddress](const Lumina::BluetoothDevice& device) { return device.address == deviceAddress; });

    if (it != m_DiscoveredDevices.end())
    {
        return &(*it);
    }

    return nullptr;
}

bool LuminaDeviceManager::IsDeviceConnected(const std::string& deviceAddress) const
{
    auto it = std::find_if(m_ConnectedDevices.begin(), m_ConnectedDevices.end(),
        [&deviceAddress](const Lumina::BluetoothDevice& device) { return device.address == deviceAddress; });

    return it != m_ConnectedDevices.end();
}

bool LuminaDeviceManager::IsDevicePaired(const std::string& deviceAddress) const
{
    auto it = std::find_if(m_PairedDevices.begin(), m_PairedDevices.end(),
        [&deviceAddress](const Lumina::BluetoothDevice& device) { return device.address == deviceAddress; });

    return it != m_PairedDevices.end();
}

void LuminaDeviceManager::Render()
{
    ImGui::Begin("Device Manager");

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

void LuminaDeviceManager::UpdateDeviceStatus(const std::string& deviceAddress, bool connected, bool paired)
{
    Lumina::BluetoothDevice* device = GetDeviceByAddress(deviceAddress);
    if (device)
    {
        device->isConnected = connected;
        device->isPaired = paired;
    }
}