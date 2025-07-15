#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <winrt/base.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.Storage.Streams.h>
#include "LuminaActionDiscoverDevice.h"
#include "LuminaDevice.h"
#include "LuminaHelper.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::System::Threading;

LuminaActionDiscoverDevice::LuminaActionDiscoverDevice()
    : m_Requested(false)
    , m_ScanTimeoutSeconds(30)
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

LuminaActionDiscoverDevice::~LuminaActionDiscoverDevice()
{
    StopScanning_Internal();
}

void LuminaActionDiscoverDevice::RequestScan()
{
    if (!m_Requested)
    {
        StartBluetoothLEScanning();
    }
}

void LuminaActionDiscoverDevice::StopScan()
{
    StopScanning_Internal();
}

void LuminaActionDiscoverDevice::StartBluetoothLEScanning()
{
    if (m_Requested)
    {
        return; // Already scanning
    }

    try
    {
        m_Requested = true;

        // Clear previous discoveries
        {
            std::lock_guard<std::mutex> lock(m_devicesMutex);
            m_discoveredDevices.clear();
        }

        // Create the watcher
        m_watcher = BluetoothLEAdvertisementWatcher();

        // Configure scanning parameters
        m_watcher.ScanningMode(BluetoothLEScanningMode::Active);

        // Set signal strength filter to catch devices in pairing mode
        m_watcher.SignalStrengthFilter().InRangeThresholdInDBm(-70);
        m_watcher.SignalStrengthFilter().OutOfRangeThresholdInDBm(-80);
        m_watcher.SignalStrengthFilter().OutOfRangeTimeout(Windows::Foundation::TimeSpan(std::chrono::seconds(5)));

        // Set up event handlers
        m_receivedToken = m_watcher.Received({ this, &LuminaActionDiscoverDevice::OnAdvertisementReceived });
        m_stoppedToken = m_watcher.Stopped({ this, &LuminaActionDiscoverDevice::OnScanStopped });

        // Start scanning
        m_watcher.Start();

        // Set up timeout timer
        Windows::Foundation::TimeSpan timeout{ std::chrono::seconds(m_ScanTimeoutSeconds) };
        m_timeoutTimer = ThreadPoolTimer::CreateTimer(
            [this](ThreadPoolTimer const&) { OnScanTimeout(); },
            timeout
        );

        if (m_OnErrorMessageGenerated)
        {
            m_OnErrorMessageGenerated("Started Bluetooth LE scanning for " + std::to_string(m_ScanTimeoutSeconds) + " seconds...");
        }
    }
    catch (winrt::hresult_error const& ex)
    {
        if (m_OnErrorMessageGenerated)
        {
            std::wstring error = L"Failed to start BLE scanning: " + std::wstring(ex.message());
            std::string errorStr = LuminaHelper::WideStringToUtf8(error);

            m_OnErrorMessageGenerated(errorStr);
        }
        m_Requested = false;
    }
    catch (...)
    {
        if (m_OnErrorMessageGenerated)
        {
            m_OnErrorMessageGenerated("Bluetooth LE scanning threw an exception.");
        }
        m_Requested = false;
    }
}

void LuminaActionDiscoverDevice::StopScanning_Internal()
{
    if (!m_Requested)
    {
        return;
    }

    try
    {
        // Cancel timeout timer
        if (m_timeoutTimer)
        {
            m_timeoutTimer.Cancel();
            m_timeoutTimer = nullptr;
        }

        // Stop watcher
        if (m_watcher)
        {
            // Remove event handlers
            if (m_receivedToken.value != 0)
            {
                m_watcher.Received(m_receivedToken);
                m_receivedToken = {};
            }
            if (m_stoppedToken.value != 0)
            {
                m_watcher.Stopped(m_stoppedToken);
                m_stoppedToken = {};
            }

            if (m_watcher.Status() == BluetoothLEAdvertisementWatcherStatus::Started)
            {
                m_watcher.Stop();
            }
            m_watcher = nullptr;
        }

        m_Requested = false;
    }
    catch (...)
    {
        // Ignore errors during cleanup
        m_Requested = false;
    }
}

void LuminaActionDiscoverDevice::OnAdvertisementReceived(
    BluetoothLEAdvertisementWatcher const& sender,
    BluetoothLEAdvertisementReceivedEventArgs const& args)
{
    try
    {
        DiscoveredDeviceInfo deviceInfo = ExtractDeviceInfo(args);

        bool isNewDevice = false;
        {
            std::lock_guard<std::mutex> lock(m_devicesMutex);

            // Check if this is a new device
            auto it = m_discoveredDevices.find(deviceInfo.bluetoothAddress);
            isNewDevice = (it == m_discoveredDevices.end());

            // Update device info
            m_discoveredDevices[deviceInfo.bluetoothAddress] = deviceInfo;
        }

        if (isNewDevice)
        {
            // Convert to DeviceInformation and notify
            ConvertToDeviceInformation(deviceInfo);
        }
    }
    catch (...)
    {
        // Handle parsing errors silently
    }
}

void LuminaActionDiscoverDevice::OnScanStopped(
    BluetoothLEAdvertisementWatcher const& sender,
    BluetoothLEAdvertisementWatcherStoppedEventArgs const& args)
{
    m_Requested = false;

    // Check if stopped due to error
    if (args.Error() != Windows::Devices::Bluetooth::BluetoothError::Success)
    {
        if (m_OnErrorMessageGenerated)
        {
            m_OnErrorMessageGenerated("Bluetooth scanning stopped due to error.");
        }
    }
}

void LuminaActionDiscoverDevice::OnScanTimeout()
{
    // Report final results
    std::vector<DiscoveredDeviceInfo> finalDevices;
    {
        std::lock_guard<std::mutex> lock(m_devicesMutex);
        for (const auto& pair : m_discoveredDevices)
        {
            finalDevices.push_back(pair.second);
        }
    }

    if (finalDevices.empty() && m_OnErrorMessageGenerated)
    {
        m_OnErrorMessageGenerated("No new devices found during scan.");
    }

    // Stop scanning
    StopScanning_Internal();
}

LuminaActionDiscoverDevice::DiscoveredDeviceInfo LuminaActionDiscoverDevice::ExtractDeviceInfo(
    const BluetoothLEAdvertisementReceivedEventArgs& args)
{
    DiscoveredDeviceInfo info;
    info.bluetoothAddress = args.BluetoothAddress();
    info.rssi = args.RawSignalStrengthInDBm();
    info.lastSeen = std::chrono::steady_clock::now();
    info.isConnectable = false;

    // Extract device name
    std::wstring localName;
    if (localName.empty())
    {
        // Try to get name from advertisement data
        auto advertisement = args.Advertisement();
        if (!advertisement.LocalName().empty())
        {
            localName = advertisement.LocalName().c_str();
        }
        else
        {
            // Fallback: create name from address
            localName = L"BLE Device " + std::to_wstring(info.bluetoothAddress & 0xFFFF);
        }
    }
    info.name = LuminaHelper::WideStringToUtf8(localName);

    // Check if device is connectable by looking at advertisement flags
    auto advertisement = args.Advertisement();

    // Check advertisement flags for connectability
    for (auto const& section : advertisement.DataSections())
    {
        if (section.DataType() == 0x01) // Flags data type
        {
            try
            {
                auto reader = Windows::Storage::Streams::DataReader::FromBuffer(section.Data());
                if (section.Data().Length() > 0)
                {
                    uint8_t flags = reader.ReadByte();
                    // Check if LE General Discoverable Mode flag is set
                    info.isConnectable = (flags & 0x02) != 0;
                }
            }
            catch (...)
            {
                // Ignore errors reading flags
            }
        }
    }

    // If no flags found, assume connectable for devices with names or service UUIDs
    if (!info.isConnectable)
    {
        info.isConnectable = !localName.empty() || (advertisement.ServiceUuids().Size() != 0);
    }

    return info;
}

winrt::fire_and_forget LuminaActionDiscoverDevice::ConvertToDeviceInformation(const DiscoveredDeviceInfo& deviceInfo)
{
    try
    {
        // Try to get the actual Bluetooth device
        auto bluetoothAddress = deviceInfo.bluetoothAddress;
        auto bluetoothDevice = co_await Windows::Devices::Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(bluetoothAddress);

        if (bluetoothDevice != nullptr)
        {
            // Get DeviceInformation from the Bluetooth device
            auto deviceInfo_winrt = bluetoothDevice.DeviceInformation();

            std::vector<Windows::Devices::Enumeration::DeviceInformation> devices;
            devices.push_back(deviceInfo_winrt);

            if (m_OnDevicesDiscovered)
            {
                m_OnDevicesDiscovered(devices);
            }
        }
        else
        {
            // Create a mock DeviceInformation-like structure for BLE devices that can't be directly accessed
            // This is a fallback - you might need to modify your callback to handle BLE-specific data
            if (m_OnErrorMessageGenerated)
            {
                std::string msg = "Found BLE device: " + deviceInfo.name +
                    " (" + BluetoothAddressToString(deviceInfo.bluetoothAddress) +
                    "), RSSI: " + std::to_string(deviceInfo.rssi) + " dBm";
                m_OnErrorMessageGenerated(msg);
            }
        }
    }
    catch (...)
    {
        // Device might not be accessible yet - this is common for BLE devices in pairing mode
        if (m_OnErrorMessageGenerated)
        {
            std::string msg = "Detected BLE device in pairing mode: " + deviceInfo.name +
                " (" + BluetoothAddressToString(deviceInfo.bluetoothAddress) +
                "), RSSI: " + std::to_string(deviceInfo.rssi) + " dBm";
            m_OnErrorMessageGenerated(msg);
        }
    }
}

std::string LuminaActionDiscoverDevice::BluetoothAddressToString(uint64_t address)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');

    for (int i = 5; i >= 0; --i)
    {
        if (i < 5) ss << ":";
        ss << std::setw(2) << ((address >> (i * 8)) & 0xFF);
    }

    return ss.str();
}

bool LuminaActionDiscoverDevice::GetIsScanRequested() const
{
    return m_Requested;
}

void LuminaActionDiscoverDevice::HandleOnDevicesDiscovered(const std::function<void(const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>&)>& callback)
{
    m_OnDevicesDiscovered = callback;
}