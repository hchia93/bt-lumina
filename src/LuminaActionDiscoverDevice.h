#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <atomic>
#include <mutex>
#include <map>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.System.Threading.h>

class LuminaActionDiscoverDevice
{
public:
    LuminaActionDiscoverDevice();
    ~LuminaActionDiscoverDevice();
    LuminaActionDiscoverDevice(const LuminaActionDiscoverDevice&) = delete;
    LuminaActionDiscoverDevice& operator=(const LuminaActionDiscoverDevice&) = delete;

    void RequestScan();
    void StopScan();
    bool GetIsScanRequested() const;

    // Set scan timeout in seconds (default: 30 seconds)
    void SetScanTimeout(int timeoutSeconds) { m_ScanTimeoutSeconds = timeoutSeconds; }
    int GetScanTimeout() const { return m_ScanTimeoutSeconds; }

    // Set callback to report discovered devices
    void HandleOnDevicesDiscovered(const std::function<void(const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>&)>& callback);
    void HandleOnErrorMessage(const std::function<void(const std::string&)>& callback) { m_OnErrorMessageGenerated = callback; }

private:
    // Bluetooth LE Advertisement Watcher
    winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher m_watcher{ nullptr };

    // Timer for scan timeout
    winrt::Windows::System::Threading::ThreadPoolTimer m_timeoutTimer{ nullptr };

    // Device tracking
    struct DiscoveredDeviceInfo
    {
        uint64_t bluetoothAddress;
        std::string name;
        int16_t rssi;
        std::chrono::steady_clock::time_point lastSeen;
        bool isConnectable;
    };

    std::map<uint64_t, DiscoveredDeviceInfo> m_discoveredDevices;
    std::mutex m_devicesMutex;

    // State tracking
    std::atomic<bool> m_Requested = false;
    int m_ScanTimeoutSeconds = 30;

    // Event tokens for cleanup
    winrt::event_token m_receivedToken;
    winrt::event_token m_stoppedToken;

    // Callbacks
    std::function<void(const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>&)> m_OnDevicesDiscovered;
    std::function<void(const std::string&)> m_OnErrorMessageGenerated;

    // Internal methods
    void StartBluetoothLEScanning();
    void StopScanning_Internal();
    void OnAdvertisementReceived(
        winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const& sender,
        winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs const& args);
    void OnScanStopped(
        winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const& sender,
        winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs const& args);
    void OnScanTimeout();

    DiscoveredDeviceInfo ExtractDeviceInfo(const winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs& args);
    winrt::fire_and_forget ConvertToDeviceInformation(const DiscoveredDeviceInfo& deviceInfo);
    std::string BluetoothAddressToString(uint64_t address);
};