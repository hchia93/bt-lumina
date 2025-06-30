#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <functional>

// Add WinRT Bluetooth includes
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Enumeration.h>

namespace Lumina
{
    struct BluetoothDevice
    {
        std::string name;
        std::string address;
        bool isConnected;
        bool isPaired;
        int signalStrength; // RSSI value. Not available in this API
        std::string deviceType; // Ideally should be enum after knowing all possible device type
    };
}

class LuminaDeviceManager
{
public:
    LuminaDeviceManager();
    ~LuminaDeviceManager();

    // Device management
    void AddDevice(const Lumina::BluetoothDevice& device);
    void RemoveDevice(const std::string& deviceAddress);
    void ConnectToDevice(const std::string& deviceAddress);
    void DisconnectFromDevice(const std::string& deviceAddress);

    // Device queries
    const std::vector<Lumina::BluetoothDevice>& GetPairedDevices() const;
    const std::vector<Lumina::BluetoothDevice>& GetConnectedDevices() const;
    const std::vector<Lumina::BluetoothDevice>& GetDiscoveredDevices() const;
    void AddDiscoveredDevice(const Lumina::BluetoothDevice& device);
    void ClearDiscoveredDevices();

    // Device information
    Lumina::BluetoothDevice* GetDeviceByAddress(const std::string& deviceAddress);
    bool IsDeviceConnected(const std::string& deviceAddress) const;
    bool IsDevicePaired(const std::string& deviceAddress) const;

    void Render();

    // Cleanup method
    void Cleanup();

private:
    std::vector<Lumina::BluetoothDevice> m_DiscoveredDevices;
    std::vector<Lumina::BluetoothDevice> m_PairedDevices;
    std::vector<Lumina::BluetoothDevice> m_ConnectedDevices;

    // Flag to prevent new async operations during cleanup
    bool m_IsShuttingDown = false;

    void UpdateDeviceStatus(const std::string& deviceAddress, bool connected, bool paired);
};