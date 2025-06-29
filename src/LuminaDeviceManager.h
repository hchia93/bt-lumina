#pragma once
#include <vector>
#include <string>

struct BluetoothDevice
{
    std::string name;
    std::string address;
    bool isConnected;
    bool isPaired;
    int signalStrength; // RSSI value
    std::string deviceType; // e.g., "Headphones", "Speaker", "Phone", etc.
};

class LuminaDeviceManager
{
public:
    LuminaDeviceManager();
    ~LuminaDeviceManager();

    // Device scanning
    void StartScan();
    void StopScan();
    bool IsScanning() const;
    
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

private:
    std::vector<BluetoothDevice> discoveredDevices;
    std::vector<BluetoothDevice> pairedDevices;
    std::vector<BluetoothDevice> connectedDevices;
    
    bool isScanning;
    int scanTimeoutSeconds;
    
    // Internal helper methods
    void ClearDiscoveredDevices();
    void UpdateDeviceStatus(const std::string& deviceAddress, bool connected, bool paired);
}; 