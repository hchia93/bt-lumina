#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <atomic>
#include <winrt/Windows.Devices.Enumeration.h>

class LuminaActionDiscoverDevice
{
public:
    LuminaActionDiscoverDevice();
    LuminaActionDiscoverDevice(const LuminaActionDiscoverDevice&) = delete;
    LuminaActionDiscoverDevice& operator=(const LuminaActionDiscoverDevice&) = delete;

    void RequestScan();
    bool GetIsScanRequested() const;

    // Set callback to report discovered devices
    void HandleOnDevicesDiscovered(const std::function<void(const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>&)>& callback);
    void HandleOnErrorMessage(const std::function<void(const std::string&)>& callback) { m_OnErrorMessageGenerated = callback; }

private:
    // tracks async state
    std::atomic<bool>  m_Requested = false;

    bool m_CanRenderDeviceList = true;

    std::function<void(const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>&)> m_OnDevicesDiscovered;
    std::function<void(const std::string&)> m_OnErrorMessageGenerated;

    void ScanDeviceAsync_Internal();
};