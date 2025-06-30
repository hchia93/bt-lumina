#include <algorithm>
#include <chrono>
#include <winrt/base.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Foundation.Collections.h>
#include "LuminaActionDiscoverDevice.h"

LuminaActionDiscoverDevice::LuminaActionDiscoverDevice()
    : m_IsScanning(false)
    , m_ScanTimeoutSeconds(10)
    , m_LastScanTime(std::chrono::steady_clock::now())
    , m_ScanInProgress(false)
    , m_ScanStartTime(std::chrono::steady_clock::now())
    , m_CanRenderDeviceList(true)
    , m_IsShuttingDown(false)
{
    try
    {
        winrt::init_apartment();
    }
    catch (...)
    {
    }
}

LuminaActionDiscoverDevice::~LuminaActionDiscoverDevice()
{
    m_IsShuttingDown = true;
    if (m_IsScanning) StopScan();
}

void LuminaActionDiscoverDevice::StartScan()
{
    if (m_IsShuttingDown)
    {
        return;
    }
    if (!m_IsScanning)
    {
        m_IsScanning = true;
        ScanAsync();
    }
}

void LuminaActionDiscoverDevice::ScanAsync()
{
    if (m_IsShuttingDown)
    {
        return;
    }
    try
    {
        auto selector = winrt::Windows::Devices::Bluetooth::BluetoothDevice::GetDeviceSelector();
        auto asyncOp = winrt::Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(selector);
        asyncOp.Completed([this](auto const& op, auto const& status)
        {
            if (m_IsShuttingDown) return;
            if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
            {
                try
                {
                    std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation> discovered;
                    winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Enumeration::DeviceInformation> devices = op.GetResults();
                    for (const auto& device : devices)
                    {
                        discovered.push_back(device);
                    }
                    if (m_OnDevicesDiscovered)
                    {
                        m_OnDevicesDiscovered(discovered);
                    }
                }
                catch (...)
                {
                }
            }
            m_IsScanning = false;
        });
    }
    catch (...)
    {
        m_IsScanning = false;
    }
}

void LuminaActionDiscoverDevice::StopScan()
{
    m_IsScanning = false;
}

bool LuminaActionDiscoverDevice::GetIsScanning() const {
    return m_IsScanning;
}

void LuminaActionDiscoverDevice::Tick()
{
    auto now = std::chrono::steady_clock::now();
    if (m_IsScanning && std::chrono::duration_cast<std::chrono::seconds>(now - m_LastScanTime).count() >= 1)
    {
        m_LastScanTime = now;
        ScanAsync();
    }
}

void LuminaActionDiscoverDevice::StartScanWithGate()
{
    if (!m_ScanInProgress)
    {
        m_ScanInProgress = true;
        m_ScanStartTime = std::chrono::steady_clock::now();
        m_CanRenderDeviceList = false;
        if (m_OnBeginScanCooldown) { m_OnBeginScanCooldown(); }
    }
}

bool LuminaActionDiscoverDevice::IsScanInProgress() const
{
    return m_ScanInProgress;
}

float LuminaActionDiscoverDevice::GetScanProgress() const
{
    if (!m_ScanInProgress)
    {
        return 0.0f;
    }
    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - m_ScanStartTime).count();
    return std::min(elapsed / m_ScanCooldown, 1.0f);
}

void LuminaActionDiscoverDevice::UpdateScanState()
{
    if (m_ScanInProgress)
    {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration<float>(now - m_ScanStartTime).count();
        if (elapsed >= m_ScanCooldown)
        {
            m_ScanInProgress = false;
            StopScan();
            m_CanRenderDeviceList = true;
            if (m_OnCompleteScanCooldown) { m_OnCompleteScanCooldown(); }
        }
    }
} 