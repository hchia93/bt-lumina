#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <functional>

// Add WinRT Bluetooth includes
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Enumeration.h>

struct BluetoothDevice
{
    std::string name;
    std::string address;
    bool isConnected;
    bool isPaired;
    int signalStrength; // RSSI value. Not available in this API
    std::string deviceType; // Ideally should be enum after knowing all possible device type
};

class LuminaDeviceManager
{
public:
    LuminaDeviceManager();
    ~LuminaDeviceManager();

    // Device scanning
    void StartScan();
    void StopScan();
    bool GetIsScanning() const;
    void ScanAsync(); // New: WinRT async scan
    void Tick(); // Call this every frame
    
    // Device management
    void AddDevice(const BluetoothDevice& device);
    void RemoveDevice(const std::string& deviceAddress);
    void ConnectToDevice(const std::string& deviceAddress);
    void DisconnectFromDevice(const std::string& deviceAddress);
    
    // Device queries
    const std::vector<BluetoothDevice>& GetDiscoveredDevices() const;
    const std::vector<BluetoothDevice>& GetPairedDevices() const;
    const std::vector<BluetoothDevice>& GetConnectedDevices() const;
    
    // Device information
    BluetoothDevice* GetDeviceByAddress(const std::string& deviceAddress);
    bool IsDeviceConnected(const std::string& deviceAddress) const;
    bool IsDevicePaired(const std::string& deviceAddress) const;

    void Render();
    void ClearDiscoveredDevices();

    void StartScanWithGate();
    bool IsScanInProgress() const;
    float GetScanProgress() const;
    void UpdateScanState();

    void SetOnBeginScanCooldown(const std::function<void()>& cb) { m_OnBeginScanCooldown = cb; }
    void SetOnCompleteScanCooldown(const std::function<void()>& cb) { m_OnCompleteScanCooldown = cb; }
    bool CanRenderDeviceList() const { return m_CanRenderDeviceList; }

private:
    std::vector<BluetoothDevice> m_DiscoveredDevices;
    std::vector<BluetoothDevice> m_PairedDevices;
    std::vector<BluetoothDevice> m_ConnectedDevices;
    
    bool m_IsScanning;
    int m_ScanTimeoutSeconds;
    std::chrono::steady_clock::time_point m_LastScanTime;

    // Scan gating/progress
    bool m_ScanInProgress = false;
    std::chrono::steady_clock::time_point m_ScanStartTime;
    float m_ScanDuration = 2.0f;

    std::function<void()> m_OnBeginScanCooldown;
    std::function<void()> m_OnCompleteScanCooldown;
    bool m_CanRenderDeviceList = true;

    void UpdateDeviceStatus(const std::string& deviceAddress, bool connected, bool paired);
}; 