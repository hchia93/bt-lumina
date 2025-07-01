#include <algorithm>
#include <chrono>
#include <winrt/base.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Foundation.Collections.h>
#include "LuminaActionDiscoverDevice.h"
#include "LuminaDevice.h"

LuminaActionDiscoverDevice::LuminaActionDiscoverDevice()
    : m_Requested(false)
    , m_CanRenderDeviceList(true)
{
    try
    {
        winrt::init_apartment();
    }
    catch (...)
    {
    }
}

void LuminaActionDiscoverDevice::RequestScan()
{
    if (!m_Requested)
    {
        ScanDeviceAsync_Internal();
    }
}

void LuminaActionDiscoverDevice::ScanDeviceAsync_Internal()
{
    m_Requested = true;

    try
    {
        auto selector = winrt::Windows::Devices::Bluetooth::BluetoothDevice::GetDeviceSelector();
        auto asyncOp = winrt::Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(selector);
        asyncOp.Completed([this](auto const& op, auto const& status)
            {
                std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation> discovered;
                winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Enumeration::DeviceInformation> devices;
                switch (status)
                {
                case winrt::Windows::Foundation::AsyncStatus::Completed:
                    try
                    {
                        devices = op.GetResults();
                        for (const auto& device : devices)
                        {
                            discovered.push_back(device);
                        }
                        if (m_OnDevicesDiscovered)
                        {
                            m_OnDevicesDiscovered(discovered);
                            m_Requested = false;
                        }

                        if (devices.Size() == 0);
                        {
                            m_OnErrorMessageGenerated("No device found.");
                        }
                    }
                    catch (...)
                    {
                        if (m_OnErrorMessageGenerated)
                        {
                            m_OnErrorMessageGenerated("Failed to get device discovery results.");
                        }
                    }
                    m_Requested = false;
                    break;
                case winrt::Windows::Foundation::AsyncStatus::Canceled:
                    if (m_OnErrorMessageGenerated)
                    {
                        m_OnErrorMessageGenerated("Device discovery was canceled.");
                    }
                    m_Requested = false;
                    break;
                case winrt::Windows::Foundation::AsyncStatus::Error:
                    if (m_OnErrorMessageGenerated)
                    {
                        m_OnErrorMessageGenerated("Device discovery failed.");
                    }
                    m_Requested = false;
                    break;
                case winrt::Windows::Foundation::AsyncStatus::Started:
                    break;
                default:
                    m_Requested = false;
                    break;
                }
            });
    }
    catch (...)
    {
        if (m_OnErrorMessageGenerated)
        {
            m_OnErrorMessageGenerated("Device discovery threw an exception.");
        }
        m_Requested = false;
    }
}

bool LuminaActionDiscoverDevice::GetIsScanRequested() const
{
    return m_Requested;
}

void LuminaActionDiscoverDevice::HandleOnDevicesDiscovered(const std::function<void(const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>&)>& callback)
{
    m_OnDevicesDiscovered = callback;
}
